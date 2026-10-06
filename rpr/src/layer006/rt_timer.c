#include "layer006/rt_timer.h"

#include "layer002/rt_error.h"
#include "layer004/rt_monotonic_clock.h"

static rt_s rt_timer_comparison_callback(const void *item1, const void *item2, RT_UNUSED void *context, rt_n *comparison_result)
{
	struct rt_timer *timer1 = *(struct rt_timer**)item1;
	struct rt_timer *timer2 = *(struct rt_timer**)item2;

	if (timer1->next_execution < timer2->next_execution)
		*comparison_result = -1;
	else if (timer1->next_execution == timer2->next_execution)
		*comparison_result = 0;
	else
		*comparison_result = 1;

	return RT_OK;
}

static rt_un32 RT_STDCALL rt_timer_thread_callback(void *parameter)
{
	struct rt_timer_manager *timer_manager = (struct rt_timer_manager*)parameter;
	rt_un32 timeout = RT_TYPE_MAX_UN32;
	rt_b signaled;
	struct rt_sortable_array_header* sortable_array_header;
	struct rt_timer *timer;
	rt_un64 current_clock_value;
	rt_un64 timeout_milliseconds;
	rt_un item_index;
	rt_s ret = RT_FAILED;

	while (RT_TRUE) {
		if (RT_UNLIKELY(!rt_event_wait_with_timeout(&timer_manager->event, timeout, &signaled)))
			goto end;

		if (timer_manager->stop)
			break;

		if (RT_UNLIKELY(!rt_critical_section_enter(&timer_manager->critical_section)))
			goto end;

		/* Consume all timers which are waiting for an execution. */
		while (RT_TRUE) {
			sortable_array_header = RT_SORTABLE_ARRAY_GET_HEADER(timer_manager->sortable_array);

			/* If the array is empty, wait for a new timer to be added. */
			if (!sortable_array_header->array_header.size) {
				timeout = RT_TYPE_MAX_UN32;
				break;
			}

			/* Retrieve the first timer of the array. */
			timer = *(struct rt_timer**)timer_manager->sortable_array;

			if (RT_UNLIKELY(!rt_monotonic_clock_get(&current_clock_value))) {
				rt_critical_section_leave(&timer_manager->critical_section);
				goto end;
			}

			if (timer->next_execution <= current_clock_value) {
				timer->last_execution = current_clock_value;
				timer->next_execution = timer->last_execution + (rt_un64)timer->period_ms * 1000000;

				if (RT_UNLIKELY(!rt_sortable_array_delete_item_index(&timer_manager->sortable_array, 0))) {
					rt_critical_section_leave(&timer_manager->critical_section);
					goto end;
				}
				if (RT_UNLIKELY(!rt_sortable_array_add_item(&timer_manager->sortable_array, &timer, &item_index))) {
					rt_critical_section_leave(&timer_manager->critical_section);
					goto end;
				}

				/* Call the timer callback. */
				if (RT_UNLIKELY(!timer->callback(timer->parameter))) {
					rt_critical_section_leave(&timer_manager->critical_section);
					goto end;
				}

			} else {
				/* Round up to not wake up before the next execution. */
				timeout_milliseconds = (timer->next_execution - current_clock_value + 999999) / 1000000;
				/* RT_TYPE_MAX_UN32 would mean waiting forever. */
				if (timeout_milliseconds >= RT_TYPE_MAX_UN32)
					timeout = RT_TYPE_MAX_UN32 - 1;
				else if (!timeout_milliseconds)
					timeout = 1;
				else
					timeout = (rt_un32)timeout_milliseconds;
				break;
			}
		}

		if (RT_UNLIKELY(!rt_critical_section_leave(&timer_manager->critical_section)))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_timer_manager_create(struct rt_timer_manager *timer_manager, struct rt_heap *heap)
{
	rt_b sortable_array_created = RT_FALSE;
	rt_b critical_section_created = RT_FALSE;
	rt_b thread_created = RT_FALSE;
	rt_b event_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	timer_manager->stop = RT_FALSE;

	if (RT_UNLIKELY(!rt_sortable_array_create(&timer_manager->sortable_array, 0, sizeof(struct rt_timer*), 0, heap, &rt_timer_comparison_callback, RT_NULL)))
		goto end;
	sortable_array_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_critical_section_create(&timer_manager->critical_section, RT_TRUE)))
		goto end;
	critical_section_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_event_create(&timer_manager->event)))
		goto end;
	event_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_thread_create(&timer_manager->thread, &rt_timer_thread_callback, timer_manager)))
		goto end;
	thread_created = RT_TRUE;

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!ret)) {
		if (thread_created)
			rt_thread_free(&timer_manager->thread);

		if (event_created)
			rt_event_free(&timer_manager->event);

		if (critical_section_created)
			rt_critical_section_free(&timer_manager->critical_section);

		if (sortable_array_created)
			rt_array_free(&timer_manager->sortable_array);
	}

	return ret;
}

rt_s rt_timer_create(struct rt_timer *timer, struct rt_timer_manager *timer_manager, rt_un period_ms, rt_timer_callback_t callback, void *parameter)
{
	rt_un item_index;
	rt_s ret = RT_FAILED;

	timer->timer_manager = timer_manager;
	timer->period_ms = period_ms;
	timer->callback = callback;
	timer->parameter = parameter;

	/* Consider the last execution to be now. */
	if (RT_UNLIKELY(!rt_monotonic_clock_get(&timer->last_execution)))
		goto end;

	timer->next_execution = timer->last_execution + (rt_un64)timer->period_ms * 1000000;

	if (RT_UNLIKELY(!rt_critical_section_enter(&timer_manager->critical_section)))
		goto end;

	/* Add the new timer to the existing ones. */
	if (RT_UNLIKELY(!rt_sortable_array_add_item(&timer_manager->sortable_array, &timer, &item_index))) {
		rt_critical_section_leave(&timer_manager->critical_section);
		goto end;
	}

	if (RT_UNLIKELY(!rt_critical_section_leave(&timer_manager->critical_section)))
		goto end;

	/* Signal the thread that there is a new timer. */
	if (RT_UNLIKELY(!rt_event_signal(&timer_manager->event))) {
		/* Remove the timer as the caller will not free it. */
		rt_timer_free(timer);
		goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_timer_free(struct rt_timer *timer)
{
	struct rt_timer_manager *timer_manager = timer->timer_manager;
	struct rt_sortable_array_header *sortable_array_header;
	struct rt_timer **timers;
	rt_un item_index;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_critical_section_enter(&timer_manager->critical_section)))
		goto end;

	/* Search the timer by address. */
	sortable_array_header = RT_SORTABLE_ARRAY_GET_HEADER(timer_manager->sortable_array);
	timers = (struct rt_timer**)timer_manager->sortable_array;
	for (item_index = 0; item_index < sortable_array_header->array_header.size; item_index++) {
		if (timers[item_index] == timer)
			break;
	}
	if (RT_UNLIKELY(item_index == sortable_array_header->array_header.size)) {
		/* The timer is not managed by its manager, maybe already freed. */
		rt_critical_section_leave(&timer_manager->critical_section);
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	/* The array remains sorted. */
	if (RT_UNLIKELY(!rt_sortable_array_delete_item_index(&timer_manager->sortable_array, item_index))) {
		rt_critical_section_leave(&timer_manager->critical_section);
		goto end;
	}

	if (RT_UNLIKELY(!rt_critical_section_leave(&timer_manager->critical_section)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_timer_manager_free(struct rt_timer_manager *timer_manager)
{
	rt_s ret = RT_OK;

	timer_manager->stop = RT_TRUE;

	/* Send a signal to the thread so it can stop. */
	if (RT_UNLIKELY(!rt_event_signal(&timer_manager->event)))
		ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_thread_join_and_check(&timer_manager->thread)))
		ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_thread_free(&timer_manager->thread)))
		ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_event_free(&timer_manager->event)))
		ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_critical_section_free(&timer_manager->critical_section)))
		ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_array_free(&timer_manager->sortable_array)))
		ret = RT_FAILED;
	
	return ret;
}

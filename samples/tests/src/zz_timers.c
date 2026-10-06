#include <rpr.h>

#include "zz_utils.h"

static rt_s zz_timers_log(const rt_char *label)
{
	struct rt_time_info time_info;
	rt_un milliseconds;
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_time_info_create_accurate(&time_info, &milliseconds)))
		goto end;

	buffer_size = 0;
	if (RT_UNLIKELY(!zz_append_time_info(&time_info, &milliseconds, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append(_R(" - "), 3, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append(label, rt_char_get_size(label), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_timers_callback(void *parameter)
{
	const rt_char *label = (rt_char*)parameter;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_timers_log(label)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_timers_do(struct rt_timer_manager *timer_manager)
{
	struct rt_timer timer1;
	rt_b timer1_created = RT_FALSE;
	struct rt_timer timer2;
	rt_b timer2_created = RT_FALSE;
	rt_char character;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_timer_create(&timer1, timer_manager, 500, &zz_timers_callback, _R("500 ms"))))
		goto end;
	timer1_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_timer_create(&timer2, timer_manager, 2000, &zz_timers_callback, _R("2000 ms"))))
		goto end;
	timer2_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_console_read_char(&character)))
		goto end;

	timer1_created = RT_FALSE;
	if (RT_UNLIKELY(!rt_timer_free(&timer1)))
		goto end;

	if (RT_UNLIKELY(!rt_console_read_char(&character)))
		goto end;

	ret = RT_OK;
end:
	if (timer2_created) {
		if (RT_UNLIKELY(!rt_timer_free(&timer2)))
			ret = RT_FAILED;
	}
	if (timer1_created) {
		if (RT_UNLIKELY(!rt_timer_free(&timer1)))
			ret = RT_FAILED;
	}
	return ret;
}

rt_s zz_timers(void)
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	struct rt_timer_manager timer_manager;
	rt_b timer_manager_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_timers_log(_R("Starting..."))))
		goto end;

	if (RT_UNLIKELY(!rt_runtime_heap_create(&runtime_heap)))
		goto end;
	runtime_heap_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_timer_manager_create(&timer_manager, &runtime_heap.heap)))
		goto end;
	timer_manager_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_timers_do(&timer_manager))) goto end;

	ret = RT_OK;
end:
	if (timer_manager_created) {
		if (RT_UNLIKELY(!rt_timer_manager_free(&timer_manager)))
			ret = RT_FAILED;
	}

	if (runtime_heap_created) {
		if (RT_UNLIKELY(!runtime_heap.heap.close(&runtime_heap.heap)))
			ret = RT_FAILED;
	}

	return ret;
}

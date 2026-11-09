#include "layer006/rt_thread_safe_ring.h"

#include "layer001/rt_memory.h"
#include "layer002/rt_error.h"

rt_s rt_thread_safe_ring_create(struct rt_thread_safe_ring *thread_safe_ring, rt_un capacity, rt_un item_size, struct rt_heap *heap)
{
	struct rt_ring *ring = &thread_safe_ring->ring;
	rt_b ring_created = RT_FALSE;
	struct rt_critical_section *critical_section = &thread_safe_ring->critical_section;
	rt_b critical_section_created = RT_FALSE;
	struct rt_condition_variable *not_full_condition_variable = &thread_safe_ring->not_full_condition_variable;
	rt_b not_full_condition_variable_created = RT_FALSE;
	struct rt_condition_variable *not_empty_condition_variable = &thread_safe_ring->not_empty_condition_variable;
	rt_b not_empty_condition_variable_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_ring_create(ring, capacity, item_size, heap)))
		goto end;
	ring_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_critical_section_create(critical_section, RT_FALSE)))
		goto end;
	critical_section_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_condition_variable_create(not_full_condition_variable, critical_section)))
		goto end;
	not_full_condition_variable_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_condition_variable_create(not_empty_condition_variable, critical_section)))
		goto end;
	not_empty_condition_variable_created = RT_TRUE;

	thread_safe_ring->closed = RT_FALSE;

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!ret)) {
		if (not_empty_condition_variable_created)
			rt_condition_variable_free(not_empty_condition_variable);
		if (not_full_condition_variable_created)
			rt_condition_variable_free(not_full_condition_variable);
		if (critical_section_created)
			rt_critical_section_free(critical_section);
		if (ring_created)
			rt_ring_free(ring);
	}

	return ret;
}

rt_s rt_thread_safe_ring_push(struct rt_thread_safe_ring *thread_safe_ring, const void *item)
{
	rt_b leave_critical_section = RT_FALSE;
	void *new_item;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_critical_section_enter(&thread_safe_ring->critical_section)))
		goto end;
	leave_critical_section = RT_TRUE;

	/* Wait for a free space in the ring. */
	while (thread_safe_ring->ring.size == thread_safe_ring->ring.array.size && !thread_safe_ring->closed) {
		if (RT_UNLIKELY(!rt_condition_variable_wait(&thread_safe_ring->not_full_condition_variable)))
			goto end;
	}

	if (thread_safe_ring->closed) {
		rt_error_set_last(RT_ERROR_CANCELED);
		goto end;
	} else {
		/* Signaling early so that if it fails, the ring is left untouched. */
		if (RT_UNLIKELY(!rt_condition_variable_signal(&thread_safe_ring->not_empty_condition_variable)))
			goto end;

		if (RT_UNLIKELY(!rt_ring_new_item(&thread_safe_ring->ring, &new_item)))
			goto end;

		RT_MEMORY_COPY(item, new_item, thread_safe_ring->ring.array.item_size);
	}

	ret = RT_OK;
end:
	if (leave_critical_section) {
		if (RT_UNLIKELY(!rt_critical_section_leave(&thread_safe_ring->critical_section)))
			ret = RT_FAILED;
	}
	return ret;
}

rt_s rt_thread_safe_ring_pop(struct rt_thread_safe_ring *thread_safe_ring, void *item, rt_b *closed)
{
	rt_b leave_critical_section = RT_FALSE;
	void *old_item;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_critical_section_enter(&thread_safe_ring->critical_section)))
		goto end;
	leave_critical_section = RT_TRUE;

	/* Wait for an item to be available. */
	while (!thread_safe_ring->ring.size && !thread_safe_ring->closed) {
		if (RT_UNLIKELY(!rt_condition_variable_wait(&thread_safe_ring->not_empty_condition_variable)))
			goto end;
	}

	if (thread_safe_ring->ring.size) {
		/* Signaling early so that if it fails, the ring is left untouched. */
		if (RT_UNLIKELY(!rt_condition_variable_signal(&thread_safe_ring->not_full_condition_variable)))
			goto end;

		if (RT_UNLIKELY(!rt_ring_pop_item(&thread_safe_ring->ring, &old_item)))
			goto end;

		RT_MEMORY_COPY(old_item, item, thread_safe_ring->ring.array.item_size);

		*closed = RT_FALSE;
	} else {
		*closed = RT_TRUE;
	}

	ret = RT_OK;
end:
	if (leave_critical_section) {
		if (RT_UNLIKELY(!rt_critical_section_leave(&thread_safe_ring->critical_section)))
			ret = RT_FAILED;
	}
	return ret;
}

rt_s rt_thread_safe_ring_close(struct rt_thread_safe_ring *thread_safe_ring)
{
	rt_b leave_critical_section = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_critical_section_enter(&thread_safe_ring->critical_section)))
		goto end;
	leave_critical_section = RT_TRUE;

	thread_safe_ring->closed = RT_TRUE;

	ret = RT_OK;

	if (RT_UNLIKELY(!rt_condition_variable_broadcast(&thread_safe_ring->not_full_condition_variable)))
		ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_condition_variable_broadcast(&thread_safe_ring->not_empty_condition_variable)))
		ret = RT_FAILED;

end:
	if (leave_critical_section) {
		if (RT_UNLIKELY(!rt_critical_section_leave(&thread_safe_ring->critical_section)))
			ret = RT_FAILED;
	}
	return ret;
}

rt_s rt_thread_safe_ring_free(struct rt_thread_safe_ring *thread_safe_ring)
{
	rt_s ret = RT_OK;

	if (RT_UNLIKELY(!rt_condition_variable_free(&thread_safe_ring->not_empty_condition_variable)))
		ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_condition_variable_free(&thread_safe_ring->not_full_condition_variable)))
		ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_critical_section_free(&thread_safe_ring->critical_section)))
		ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_ring_free(&thread_safe_ring->ring)))
		ret = RT_FAILED;

	return ret;
}

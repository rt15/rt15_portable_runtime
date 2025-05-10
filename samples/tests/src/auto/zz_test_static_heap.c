#include <rpr.h>

rt_s zz_test_static_heap(void)
{
	rt_un heap_buffer_capacity = 0;
	void *heap_buffer = RT_NULL;
	rt_char8 buffer[200];
	rt_char8* area;
	void *old_heap_buffer;
	rt_un i;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_static_heap_free(&heap_buffer))) goto end;
	if (RT_UNLIKELY(heap_buffer)) goto end;

	/* Smaller than buffer. */
	if (RT_UNLIKELY(!rt_static_heap_alloc_if_needed(buffer, 200, &heap_buffer, &heap_buffer_capacity, (void**)&area, 100))) goto end;
	if (RT_UNLIKELY(area != buffer)) goto end;
	if (RT_UNLIKELY(heap_buffer)) goto end;
	if (RT_UNLIKELY(heap_buffer_capacity)) goto end;
	for (i = 0; i < 100; i++)
		area[i] = 12;

	/* Same size as buffer. */
	if (RT_UNLIKELY(!rt_static_heap_alloc_if_needed(buffer, 200, &heap_buffer, &heap_buffer_capacity, (void**)&area, 200))) goto end;
	if (RT_UNLIKELY(area != buffer)) goto end;
	if (RT_UNLIKELY(heap_buffer)) goto end;
	if (RT_UNLIKELY(heap_buffer_capacity)) goto end;
	for (i = 0; i < 200; i++)
		area[i] = 12;

	/* Bigger than buffer. */
	if (RT_UNLIKELY(!rt_static_heap_alloc_if_needed(buffer, 200, &heap_buffer, &heap_buffer_capacity, (void**)&area, 201))) goto end;
	if (RT_UNLIKELY(area == buffer)) goto end;
	if (RT_UNLIKELY(area != heap_buffer)) goto end;
	if (RT_UNLIKELY(heap_buffer_capacity != 201)) goto end;
	for (i = 0; i < 201; i++)
		area[i] = 12;

	/* Bigger than heap buffer. */
	if (RT_UNLIKELY(!rt_static_heap_alloc_if_needed(buffer, 200, &heap_buffer, &heap_buffer_capacity, (void**)&area, 5000))) goto end;
	if (RT_UNLIKELY(area == buffer)) goto end;
	if (RT_UNLIKELY(area != heap_buffer)) goto end;
	if (RT_UNLIKELY(heap_buffer_capacity != 5000)) goto end;
	/* Realloc should have been used. */
	for (i = 0; i < 201; i++) {
		if (RT_UNLIKELY(area[i] != 12))
			goto end;
	}
	for (i = 0; i < 5000; i++)
		area[i] = 12;
	old_heap_buffer = heap_buffer;

	/* Smaller than heap buffer. */
	if (RT_UNLIKELY(!rt_static_heap_alloc_if_needed(buffer, 200, &heap_buffer, &heap_buffer_capacity, (void**)&area, 4500))) goto end;
	if (RT_UNLIKELY(area == buffer)) goto end;
	if (RT_UNLIKELY(area != heap_buffer)) goto end;
	if (RT_UNLIKELY(heap_buffer != old_heap_buffer)) goto end;
	if (RT_UNLIKELY(heap_buffer_capacity != 5000)) goto end;
	for (i = 0; i < 4500; i++)
		area[i] = 12;

	ret = RT_OK;
end:
	if (heap_buffer) {
		if (RT_UNLIKELY(!rt_static_heap_free(&heap_buffer)))
			ret = RT_FAILED;
	}
	if (RT_UNLIKELY(heap_buffer))
		ret = RT_FAILED;

	return ret;
}

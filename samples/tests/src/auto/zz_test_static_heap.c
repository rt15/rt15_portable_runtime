#include <rpr.h>

rt_s zz_test_static_heap()
{
	rt_un heap_buffer_size = 0;
	void *heap_buffer = RT_NULL;
	rt_char8 buffer[200];
	rt_char8* area;
	void *old_heap_buffer;
	rt_un i;
	rt_s ret;

	if (!rt_static_heap_free(&heap_buffer)) goto error;
	if (heap_buffer) goto error;

	/* Smaller than buffer. */
	if (!rt_static_heap_alloc_if_needed(buffer, 200, &heap_buffer, &heap_buffer_size, (void**)&area, 100)) goto error;
	if (area != buffer) goto error;
	if (heap_buffer) goto error;
	if (heap_buffer_size) goto error;
	for (i = 0; i < 100; i++)
		area[i] = 12;

	/* Same size as buffer. */
	if (!rt_static_heap_alloc_if_needed(buffer, 200, &heap_buffer, &heap_buffer_size, (void**)&area, 200)) goto error;
	if (area != buffer) goto error;
	if (heap_buffer) goto error;
	if (heap_buffer_size) goto error;
	for (i = 0; i < 200; i++)
		area[i] = 12;

	/* Bigger than buffer. */
	if (!rt_static_heap_alloc_if_needed(buffer, 200, &heap_buffer, &heap_buffer_size, (void**)&area, 201)) goto error;
	if (area == buffer) goto error;
	if (area != heap_buffer) goto error;
	if (heap_buffer_size != 201) goto error;
	for (i = 0; i < 201; i++)
		area[i] = 12;

	/* Bigger than heap buffer. */
	if (!rt_static_heap_alloc_if_needed(buffer, 200, &heap_buffer, &heap_buffer_size, (void**)&area, 5000)) goto error;
	if (area == buffer) goto error;
	if (area != heap_buffer) goto error;
	if (heap_buffer_size != 5000) goto error;
	/* Realloc should have been used. */
	for (i = 0; i < 201; i++) {
		if (area[i] != 12)
			goto error;
	}
	for (i = 0; i < 5000; i++)
		area[i] = 12;
	old_heap_buffer = heap_buffer;

	/* Smaller than heap buffer. */
	if (!rt_static_heap_alloc_if_needed(buffer, 200, &heap_buffer, &heap_buffer_size, (void**)&area, 4500)) goto error;
	if (area == buffer) goto error;
	if (area != heap_buffer) goto error;
	if (heap_buffer != old_heap_buffer) goto error;
	if (heap_buffer_size != 5000) goto error;
	for (i = 0; i < 4500; i++)
		area[i] = 12;

	ret = RT_OK;
free:
	if (heap_buffer) {
		if (!rt_static_heap_free(&heap_buffer) && ret)
			goto error;
	}
	if (heap_buffer)
		goto error;
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
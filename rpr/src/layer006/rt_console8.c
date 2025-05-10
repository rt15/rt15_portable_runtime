#include "layer006/rt_console8.h"

#include "layer003/rt_char.h"
#include "layer003/rt_char8.h"
#include "layer004/rt_static_heap.h"
#include "layer005/rt_console.h"
#include "layer005/rt_runtime_heap.h"

rt_s rt_console8_write_str(const rt_char8 *str, enum rt_encoding encoding)
{
	return rt_console8_write_with_size(str, encoding, rt_char8_get_size(str), RT_FALSE);
}

rt_s rt_console8_write_str_with_size(const rt_char8 *str, enum rt_encoding encoding, rt_un size)
{
	return rt_console8_write_with_size(str, encoding, size, RT_FALSE);
}

rt_s rt_console8_write_error(const rt_char8 *error, enum rt_encoding encoding)
{
	return rt_console8_write_with_size(error, encoding, rt_char8_get_size(error), RT_TRUE);
}

rt_s rt_console8_write_error_with_size(const rt_char8 *error, enum rt_encoding encoding, rt_un size)
{
	return rt_console8_write_with_size(error, encoding, size, RT_TRUE);
}

rt_s rt_console8_write(const rt_char8 *message, enum rt_encoding encoding, rt_b error)
{
	return rt_console8_write_with_size(message, encoding, rt_char8_get_size(message), error);
}

rt_s rt_console8_write_with_size(const rt_char8 *message, enum rt_encoding encoding, rt_un size, rt_b error)
{
	rt_char buffer[RT_CHAR_BIG_STRING_SIZE];
	void *heap_buffer = RT_NULL;
	rt_un heap_buffer_capacity = 0;
	rt_char *output;
	rt_un output_size;
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_runtime_heap_create(&runtime_heap))) goto end;
	runtime_heap_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_encoding_decode(message, size, encoding, buffer, RT_CHAR_BIG_STRING_SIZE, &heap_buffer, &heap_buffer_capacity, &output, &output_size, &runtime_heap.heap))) goto end;

	if (RT_UNLIKELY(!rt_console_write_with_size(output, output_size, error))) goto end;

	ret = RT_OK;
end:
	if (heap_buffer) {
		if (RT_UNLIKELY(!runtime_heap.heap.free(&runtime_heap.heap, &heap_buffer)))
			ret = RT_FAILED;	
	}

	if (runtime_heap_created) {
		if (RT_UNLIKELY(!runtime_heap.heap.close(&runtime_heap.heap)))
			ret = RT_FAILED;
	}

	return ret;
}

rt_s rt_console8_read_line(enum rt_encoding encoding, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_char local_buffer[RT_CHAR_BIG_STRING_SIZE];
	void *heap_buffer = RT_NULL;
	rt_un heap_buffer_capacity = 0;
	rt_char *area;
	rt_un area_capacity;
	rt_un area_size;
	rt_char8 *output;
	rt_s ret = RT_FAILED;

	/* Makes sure the rt_char buffer is big enough. */
	area_capacity = buffer_capacity * 2;

	if (RT_UNLIKELY(!rt_static_heap_alloc_if_needed(local_buffer, RT_CHAR_BIG_STRING_SIZE, &heap_buffer, &heap_buffer_capacity, (void**)&area, area_capacity * sizeof(rt_char)))) goto end; 

	if (RT_UNLIKELY(!rt_console_read_line(area, area_capacity, &area_size))) goto end;

	if (RT_UNLIKELY(!rt_encoding_encode(area, area_size, encoding, buffer, buffer_capacity, RT_NULL, RT_NULL, &output, buffer_size, RT_NULL))) goto end;

	ret = RT_OK;
end:
	if (heap_buffer) {
		if (RT_UNLIKELY(!rt_static_heap_free(&heap_buffer)))
			ret = RT_FAILED;
	}

	return ret;
}

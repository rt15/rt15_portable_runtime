#include <rpr.h>

static rt_s zz_check_stream(struct rt_memory_output_stream *memory_output_stream, rt_b heap_buffer, const rt_char8 *buffer, rt_un expected_size, rt_un expected_buffer_capacity, rt_un expected_heap_buffer_capacity)
{
	rt_un i;
	rt_char8 *data;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(memory_output_stream->size != expected_size))
		goto end;

	data = rt_memory_output_stream_get_data(memory_output_stream);
	for (i = 0; i < expected_size; i++) {
		if (RT_UNLIKELY(data[i] != 'a'))
			goto end;
	}

	if (RT_UNLIKELY(memory_output_stream->buffer != buffer))
		goto end;

	if (heap_buffer) {
		if (RT_UNLIKELY(data == buffer)) goto end;
		if (RT_UNLIKELY(!memory_output_stream->heap_buffer)) goto end;
		if (RT_UNLIKELY(data != memory_output_stream->heap_buffer)) goto end;
	} else {
		if (RT_UNLIKELY(data != buffer)) goto end;
		if (RT_UNLIKELY(memory_output_stream->heap_buffer)) goto end;
		if (RT_UNLIKELY(data != memory_output_stream->buffer)) goto end;
	}

	if (RT_UNLIKELY(memory_output_stream->buffer_capacity != expected_buffer_capacity)) goto end;
	if (RT_UNLIKELY(memory_output_stream->heap_buffer_capacity != expected_heap_buffer_capacity)) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_buffer_and_heap(struct rt_heap* heap, rt_char8 *input)
{
	struct rt_memory_output_stream memory_output_stream;
	rt_b memory_output_stream_created = RT_FALSE;
	rt_char8 buffer[64];
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_memory_output_stream_create(&memory_output_stream, buffer, 64, heap, 64)))
		goto end;
	memory_output_stream_created = RT_TRUE;

	/* Empty stream. */
	if (RT_UNLIKELY(!zz_check_stream(&memory_output_stream, RT_FALSE, buffer, 0, 64, 0))) goto end;

	/* Write 64. */
	if (RT_UNLIKELY(!memory_output_stream.output_stream.write(&memory_output_stream.output_stream, input, 64))) goto end;
	if (RT_UNLIKELY(!zz_check_stream(&memory_output_stream, RT_FALSE, buffer, 64, 64, 0))) goto end;

	/* Write 1. */
	if (RT_UNLIKELY(!memory_output_stream.output_stream.write(&memory_output_stream.output_stream, input, 1))) goto end;
	if (RT_UNLIKELY(!zz_check_stream(&memory_output_stream, RT_TRUE, buffer, 65, 64, 130))) goto end;

	ret = RT_OK;
end:
	if (memory_output_stream_created) {
		if (RT_UNLIKELY(!rt_memory_output_stream_free(&memory_output_stream)))
			ret = RT_FAILED;
	}

	return ret;
}

static rt_s zz_test_heap(struct rt_heap* heap, rt_char8 *input)
{
	struct rt_memory_output_stream memory_output_stream;
	rt_b memory_output_stream_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_memory_output_stream_create(&memory_output_stream, RT_NULL, 0, heap, 64)))
		goto end;
	memory_output_stream_created = RT_TRUE;

	/* Empty stream. */
	if (RT_UNLIKELY(!zz_check_stream(&memory_output_stream, RT_TRUE, RT_NULL, 0, 0, 64))) goto end;

	/* Write 16. */
	if (RT_UNLIKELY(!memory_output_stream.output_stream.write(&memory_output_stream.output_stream, input, 16))) goto end;
	if (RT_UNLIKELY(!zz_check_stream(&memory_output_stream, RT_TRUE, RT_NULL, 16, 0, 64))) goto end;

	/* Write 48. */
	if (RT_UNLIKELY(!memory_output_stream.output_stream.write(&memory_output_stream.output_stream, input, 48))) goto end;
	if (RT_UNLIKELY(!zz_check_stream(&memory_output_stream, RT_TRUE, RT_NULL, 64, 0, 64))) goto end;

	/* Write 1. */
	if (RT_UNLIKELY(!memory_output_stream.output_stream.write(&memory_output_stream.output_stream, input, 1))) goto end;
	if (RT_UNLIKELY(!zz_check_stream(&memory_output_stream, RT_TRUE, RT_NULL, 65, 0, 130))) goto end;

	ret = RT_OK;
end:
	if (memory_output_stream_created) {
		if (RT_UNLIKELY(!rt_memory_output_stream_free(&memory_output_stream)))
			ret = RT_FAILED;
	}

	return ret;
}

static rt_s zz_test_buffer(rt_char8 *input)
{
	struct rt_memory_output_stream memory_output_stream;
	rt_b memory_output_stream_created = RT_FALSE;
	rt_char8 buffer[64];
	rt_s ret = RT_FAILED;

	/* Nothing to store data. */
	if (RT_UNLIKELY(rt_memory_output_stream_create(&memory_output_stream, RT_NULL, 0, RT_NULL, 1))) {
		memory_output_stream_created = RT_TRUE;
		goto end;
	}

	/* Buffer too small. */
	if (RT_UNLIKELY(rt_memory_output_stream_create(&memory_output_stream, buffer, 64, RT_NULL, 65))) {
		memory_output_stream_created = RT_TRUE;
		goto end;
	}

	/* Buffer good enough. */
	if (RT_UNLIKELY(!rt_memory_output_stream_create(&memory_output_stream, buffer, 64, RT_NULL, 64)))
		goto end;
	memory_output_stream_created = RT_TRUE;

	/* Empty stream. */
	if (RT_UNLIKELY(!zz_check_stream(&memory_output_stream, RT_FALSE, buffer, 0, 64, 0))) goto end;

	/* Write 16. */
	if (RT_UNLIKELY(!memory_output_stream.output_stream.write(&memory_output_stream.output_stream, input, 16))) goto end;
	if (RT_UNLIKELY(!zz_check_stream(&memory_output_stream, RT_FALSE, buffer, 16, 64, 0))) goto end;

	/* Write 48. */
	if (RT_UNLIKELY(!memory_output_stream.output_stream.write(&memory_output_stream.output_stream, input, 48))) goto end;
	if (RT_UNLIKELY(!zz_check_stream(&memory_output_stream, RT_FALSE, buffer, 64, 64, 0))) goto end;

	/* Fails to write to a full stream. */
	if (RT_UNLIKELY(memory_output_stream.output_stream.write(&memory_output_stream.output_stream, input, 1))) goto end;

	ret = RT_OK;
end:
	if (memory_output_stream_created) {
		if (RT_UNLIKELY(!rt_memory_output_stream_free(&memory_output_stream)))
			ret = RT_FAILED;
	}

	return ret;
}

rt_s zz_test_memory_output_stream(void)
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	rt_char8 input[128];
	rt_un i;
	rt_s ret = RT_FAILED;

	/* Prepare input. */
	for (i = 0; i < 128; i++)
		input[i] = 'a';

	if (RT_UNLIKELY(!rt_runtime_heap_create(&runtime_heap)))
		goto end;
	runtime_heap_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_test_buffer(input))) goto end;
	if (RT_UNLIKELY(!zz_test_heap(&runtime_heap.heap, input))) goto end;
	if (RT_UNLIKELY(!zz_test_buffer_and_heap(&runtime_heap.heap, input))) goto end;

	ret = RT_OK;
end:
	if (runtime_heap_created) {
		if (RT_UNLIKELY(!runtime_heap.heap.close(&runtime_heap.heap)))
			ret = RT_FAILED;
	}

	return ret;
}

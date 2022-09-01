#include <rpr.h>

static rt_s zz_check_stream(struct rt_memory_output_stream *memory_output_stream, rt_b heap_buffer, const rt_char8 *buffer, rt_un expected_size, rt_un expected_buffer_capacity, rt_un expected_heap_buffer_capacity)
{
	rt_un i;
	rt_char8 *data;
	rt_s ret;

	if (memory_output_stream->size != expected_size)
		goto error;

	data = rt_memory_output_stream_get_data(memory_output_stream);
	for (i = 0; i < expected_size; i++) {
		if (data[i] != 'a')
			goto error;
	}

	if (memory_output_stream->buffer != buffer)
		goto error;

	if (heap_buffer) {
		if (data == buffer) goto error;
		if (!memory_output_stream->heap_buffer) goto error;
		if (data != memory_output_stream->heap_buffer) goto error;
	} else {
		if (data != buffer) goto error;
		if (memory_output_stream->heap_buffer) goto error;
		if (data != memory_output_stream->buffer) goto error;
	}

	if (memory_output_stream->buffer_capacity != expected_buffer_capacity) goto error;
	if (memory_output_stream->heap_buffer_capacity != expected_heap_buffer_capacity) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_buffer_and_heap(struct rt_heap* heap, rt_char8 *input)
{
	struct rt_memory_output_stream memory_output_stream;
	rt_b memory_output_stream_created = RT_FALSE;
	rt_char8 buffer[64];
	rt_s ret;

	if (!rt_memory_output_stream_create(&memory_output_stream, buffer, 64, heap, 64))
		goto error;
	memory_output_stream_created = RT_TRUE;

	/* Empty stream. */
	if (!zz_check_stream(&memory_output_stream, RT_FALSE, buffer, 0, 64, 0)) goto error;

	/* Write 64. */
	if (!memory_output_stream.output_stream.write(&memory_output_stream.output_stream, input, 64)) goto error;
	if (!zz_check_stream(&memory_output_stream, RT_FALSE, buffer, 64, 64, 0)) goto error;

	/* Write 1. */
	if (!memory_output_stream.output_stream.write(&memory_output_stream.output_stream, input, 1)) goto error;
	if (!zz_check_stream(&memory_output_stream, RT_TRUE, buffer, 65, 64, 130)) goto error;

	ret = RT_OK;
free:
	if (memory_output_stream_created) {
		memory_output_stream_created = RT_FALSE;
		if (!rt_memory_output_stream_free(&memory_output_stream) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_heap(struct rt_heap* heap, rt_char8 *input)
{
	struct rt_memory_output_stream memory_output_stream;
	rt_b memory_output_stream_created = RT_FALSE;
	rt_s ret;

	if (!rt_memory_output_stream_create(&memory_output_stream, RT_NULL, 0, heap, 64))
		goto error;
	memory_output_stream_created = RT_TRUE;

	/* Empty stream. */
	if (!zz_check_stream(&memory_output_stream, RT_TRUE, RT_NULL, 0, 0, 64)) goto error;

	/* Write 16. */
	if (!memory_output_stream.output_stream.write(&memory_output_stream.output_stream, input, 16)) goto error;
	if (!zz_check_stream(&memory_output_stream, RT_TRUE, RT_NULL, 16, 0, 64)) goto error;

	/* Write 48. */
	if (!memory_output_stream.output_stream.write(&memory_output_stream.output_stream, input, 48)) goto error;
	if (!zz_check_stream(&memory_output_stream, RT_TRUE, RT_NULL, 64, 0, 64)) goto error;

	/* Write 1. */
	if (!memory_output_stream.output_stream.write(&memory_output_stream.output_stream, input, 1)) goto error;
	if (!zz_check_stream(&memory_output_stream, RT_TRUE, RT_NULL, 65, 0, 130)) goto error;

	ret = RT_OK;
free:
	if (memory_output_stream_created) {
		memory_output_stream_created = RT_FALSE;
		if (!rt_memory_output_stream_free(&memory_output_stream) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_buffer(rt_char8 *input)
{
	struct rt_memory_output_stream memory_output_stream;
	rt_b memory_output_stream_created = RT_FALSE;
	rt_char8 buffer[64];
	rt_s ret;

	/* Nothing to store data. */
	if (rt_memory_output_stream_create(&memory_output_stream, RT_NULL, 0, RT_NULL, 1)) {
		memory_output_stream_created = RT_TRUE;
		goto error;
	}

	/* Buffer too small. */
	if (rt_memory_output_stream_create(&memory_output_stream, buffer, 64, RT_NULL, 65)) {
		memory_output_stream_created = RT_TRUE;
		goto error;
	}

	/* Buffer good enough. */
	if (!rt_memory_output_stream_create(&memory_output_stream, buffer, 64, RT_NULL, 64))
		goto error;
	memory_output_stream_created = RT_TRUE;

	/* Empty stream. */
	if (!zz_check_stream(&memory_output_stream, RT_FALSE, buffer, 0, 64, 0)) goto error;

	/* Write 16. */
	if (!memory_output_stream.output_stream.write(&memory_output_stream.output_stream, input, 16)) goto error;
	if (!zz_check_stream(&memory_output_stream, RT_FALSE, buffer, 16, 64, 0)) goto error;

	/* Write 48. */
	if (!memory_output_stream.output_stream.write(&memory_output_stream.output_stream, input, 48)) goto error;
	if (!zz_check_stream(&memory_output_stream, RT_FALSE, buffer, 64, 64, 0)) goto error;

	/* Fails to write to a full stream. */
	if (memory_output_stream.output_stream.write(&memory_output_stream.output_stream, input, 1)) goto error;

	ret = RT_OK;
free:
	if (memory_output_stream_created) {
		memory_output_stream_created = RT_FALSE;
		if (!rt_memory_output_stream_free(&memory_output_stream) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_memory_output_stream()
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	rt_char8 input[128];
	rt_un i;
	rt_s ret;

	/* Prepare input. */
	for (i = 0; i < 128; i++)
		input[i] = 'a';

	if (!rt_runtime_heap_create(&runtime_heap))
		goto error;
	runtime_heap_created = RT_TRUE;

	if (!zz_test_buffer(input)) goto error;
	if (!zz_test_heap(&runtime_heap.heap, input)) goto error;
	if (!zz_test_buffer_and_heap(&runtime_heap.heap, input)) goto error;

	ret = RT_OK;
free:
	if (runtime_heap_created) {
		runtime_heap_created = RT_FALSE;
		if (!runtime_heap.heap.close(&runtime_heap.heap) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

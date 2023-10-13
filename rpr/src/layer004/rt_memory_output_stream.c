#include "layer004/rt_memory_output_stream.h"

#include "layer001/rt_memory.h"
#include "layer002/rt_error.h"

static rt_s rt_memory_output_stream_set_capacity(struct rt_memory_output_stream *memory_output_stream, rt_un capacity)
{
	struct rt_heap *heap;
	rt_s ret;

	if (memory_output_stream->buffer_capacity < capacity && memory_output_stream->heap_buffer_capacity < capacity) {
		heap = memory_output_stream->heap;
		if (RT_LIKELY(heap)) {
			if (memory_output_stream->heap_buffer) {
				/* There was already a heap buffer, but not big enough. */
				if (RT_UNLIKELY(!heap->realloc(heap, &memory_output_stream->heap_buffer, capacity)))
					goto error;
			} else {
				/* No heap buffer yet. */
				if (RT_UNLIKELY(!heap->alloc(heap, &memory_output_stream->heap_buffer, capacity)))
					goto error;

				/* Copy existing data from buffer to heap buffer. */
				if (memory_output_stream->size) {
					RT_MEMORY_COPY(memory_output_stream->buffer, memory_output_stream->heap_buffer, memory_output_stream->size);
				}
			}
			memory_output_stream->heap_buffer_capacity = capacity;
		} else {
			/* Buffer is not enough and no heap has been provided. Notify error. */
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	rt_memory_output_stream_free(memory_output_stream);
	goto free;
}

rt_s rt_memory_output_stream_write(struct rt_output_stream *output_stream, const rt_char8 *data, rt_un bytes_to_write)
{
	struct rt_memory_output_stream *memory_output_stream = (struct rt_memory_output_stream*)output_stream;
	rt_un capacity;
	rt_un required_size = memory_output_stream->size + bytes_to_write;
	rt_char8* rt_memory_output_stream_data;
	rt_s ret;

	if (memory_output_stream->heap_buffer)
		capacity = memory_output_stream->heap_buffer_capacity;
	else
		capacity = memory_output_stream->buffer_capacity;


	if (required_size > capacity) {
		if (RT_UNLIKELY(!rt_memory_output_stream_set_capacity(memory_output_stream, required_size * 2)))
			goto error;
	}

	if (memory_output_stream->heap_buffer) {
		rt_memory_output_stream_data = memory_output_stream->heap_buffer;
	} else {
		rt_memory_output_stream_data = memory_output_stream->buffer;
	}
	RT_MEMORY_COPY(data, &rt_memory_output_stream_data[memory_output_stream->size], bytes_to_write);
	memory_output_stream->size += bytes_to_write;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_memory_output_stream_flush(RT_UNUSED struct rt_output_stream *output_stream)
{
	return RT_OK;
}

rt_s rt_memory_output_stream_create(struct rt_memory_output_stream *memory_output_stream, rt_char8 *buffer, rt_un buffer_capacity, struct rt_heap *heap, rt_un initial_capacity)
{
	memory_output_stream->output_stream.write = &rt_memory_output_stream_write;
	memory_output_stream->output_stream.flush = &rt_memory_output_stream_flush;

	memory_output_stream->size = 0;

	memory_output_stream->buffer = buffer;
	memory_output_stream->buffer_capacity = buffer_capacity;

	memory_output_stream->heap_buffer = RT_NULL;
	memory_output_stream->heap_buffer_capacity = 0;
	memory_output_stream->heap = heap;

	return rt_memory_output_stream_set_capacity(memory_output_stream, initial_capacity);
}

rt_char8 *rt_memory_output_stream_get_data(struct rt_memory_output_stream *memory_output_stream)
{
	rt_char8 *result;

	if (memory_output_stream->heap_buffer)
		result = memory_output_stream->heap_buffer;
	else
		result = memory_output_stream->buffer;

	return result;
}

rt_s rt_memory_output_stream_free(struct rt_memory_output_stream *memory_output_stream)
{
	struct rt_heap *heap = memory_output_stream->heap;
	rt_s ret;

	if (memory_output_stream->heap_buffer) {
		ret = heap->free(heap, &memory_output_stream->heap_buffer);
	} else {
		ret = RT_OK;
	}

	return ret;
}

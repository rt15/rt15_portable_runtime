#ifndef RT_MEMORY_OUTPUT_STREAM_H
#define RT_MEMORY_OUTPUT_STREAM_H

#include "layer000/rt_types.h"
#include "layer001/rt_output_stream.h"
#include "layer003/rt_heap.h"

struct rt_memory_output_stream {
	struct rt_output_stream output_stream;

	/* Number of bytes written in the output stream. */
	rt_un size;

	void *buffer;
	rt_un buffer_capacity;

	struct rt_heap *heap;
	void *heap_buffer;
	rt_un heap_buffer_capacity;
};

/**
 * <p>
 * Call <tt>rt_memory_output_stream_free</tt> when you are done with <tt>memory_output_stream</tt> and the associated memory.
 * </p>
 *
 * @param buffer Can be RT_NULL if <tt>buffer_capacity</tt> is zero.
 * @param heap Can be RT_NULL if you want to work only with <tt>buffer</tt>.
 * @param initial_capacity This function will make sure that it disposes of this initial capacity, either with the provided buffer or using the heap.
 */
rt_s rt_memory_output_stream_create(struct rt_memory_output_stream *memory_output_stream, rt_char8 *buffer, rt_un buffer_capacity, struct rt_heap *heap, rt_un initial_capacity);

rt_char8 *rt_memory_output_stream_get_data(struct rt_memory_output_stream *memory_output_stream);

rt_s rt_memory_output_stream_free(struct rt_memory_output_stream *memory_output_stream);

/**
 * In most cases, you should not need this function.<br>
 * Use <tt>output_stream->write</tt> instead.
 */
rt_s rt_memory_output_stream_write(struct rt_output_stream *output_stream, const rt_char8 *data, rt_un bytes_to_write);

/**
 * Does nothing.
 */
rt_s rt_memory_output_stream_flush(struct rt_output_stream *output_stream);

#endif /* RT_MEMORY_OUTPUT_STREAM_H */

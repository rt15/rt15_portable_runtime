#ifndef RT_BUFFERED_INPUT_STREAM_H
#define RT_BUFFERED_INPUT_STREAM_H

#include "layer000/rt_types.h"
#include "layer001/rt_input_stream.h"

/**
 * @file
 * This is a proxy that reads from the real input stream by chunks of <tt>cache_capacity</tt> bytes.<br>
 * The idea is for example to read few characters from the stream without performing a lot of system calls.
 *
 * <p>
 * If <tt>bytes_to_read</tt> is greater or equal to <tt>cache_capacity</tt>, then the cache is not used.<br>
 * All the requested bytes are read directly from the real input stream.
 * </p>
 */

struct rt_buffered_input_stream {
	struct rt_input_stream input_stream;
	struct rt_input_stream *real_input_stream;
	rt_char8 *cache;
	rt_un cache_capacity;
	rt_un cache_index;
	rt_un cache_size;
};

rt_s rt_buffered_input_stream_create(struct rt_buffered_input_stream *buffered_input_stream, struct rt_input_stream *real_input_stream, rt_char8 *cache, rt_un cache_capacity);

rt_s rt_buffered_input_stream_read(struct rt_input_stream *input_stream, rt_char8 *buffer, rt_un bytes_to_read, rt_un *bytes_read);

#endif /* RT_BUFFERED_INPUT_STREAM_H */

#ifndef RT_BUFFERED_OUTPUT_STREAM_H
#define RT_BUFFERED_OUTPUT_STREAM_H

#include "layer000/rt_types.h"
#include "layer001/rt_output_stream.h"

/**
 * @file
 * This is a proxy that writes to a buffer of <tt>cache_capacity</tt> bytes before writing to the real output stream.<br>
 * The idea is for example to write few characters into the stream without performing a lot of system calls.
 *
 * <p>
 * It is mandatory to call <tt>flush</tt> to ensure that everything is sent to the kernel before closing the stream.
 * </p>
 */

struct rt_buffered_output_stream {
	struct rt_output_stream output_stream;
	struct rt_output_stream *real_output_stream;
	rt_char8 *buffer;
	rt_un buffer_capacity;
	rt_un buffer_size;
};

RT_API rt_s rt_buffered_output_stream_create(struct rt_buffered_output_stream *buffered_output_stream, struct rt_output_stream *real_output_stream, rt_char8 *buffer, rt_un buffer_capacity);

RT_API rt_s rt_buffered_output_stream_write(struct rt_output_stream *output_stream, const rt_char8 *data, rt_un bytes_to_write);

RT_API rt_s rt_buffered_output_stream_flush(struct rt_output_stream *output_stream);

#endif /* RT_BUFFERED_OUTPUT_STREAM_H */

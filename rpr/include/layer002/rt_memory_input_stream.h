#ifndef RT_MEMORY_INPUT_STREAM_H
#define RT_MEMORY_INPUT_STREAM_H

#include "layer000/rt_types.h"
#include "layer001/rt_input_stream.h"

struct rt_memory_input_stream {
	struct rt_input_stream input_stream;
	const rt_char8 *memory;
	rt_un memory_size;
	rt_un offset;
};

RT_API rt_s rt_memory_input_stream_create(struct rt_memory_input_stream *memory_input_stream, const rt_char8 *memory, rt_un memory_size);

/**
 * In most cases, you should not need this function.<br>
 * Use <tt>input_stream->read</tt> instead.
 */
RT_API rt_s rt_memory_input_stream_read(struct rt_input_stream *input_stream, rt_char8 *buffer, rt_un bytes_to_read, rt_un *bytes_read);

#endif /* RT_MEMORY_INPUT_STREAM_H */

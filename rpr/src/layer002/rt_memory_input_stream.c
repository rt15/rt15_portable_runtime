#include "layer002/rt_memory_input_stream.h"

#include "layer001/rt_memory.h"

rt_s rt_memory_input_stream_read(struct rt_input_stream *input_stream, rt_char8 *buffer, rt_un bytes_to_read, rt_un *bytes_read)
{
	struct rt_memory_input_stream *memory_input_stream = (struct rt_memory_input_stream*)input_stream;
	rt_un offset = memory_input_stream->offset;
	rt_un remaining_bytes = memory_input_stream->memory_size - offset;
	rt_un actual_bytes_to_read;

	if (bytes_to_read > remaining_bytes) {
		actual_bytes_to_read = remaining_bytes;
	} else {
		actual_bytes_to_read = bytes_to_read;
	}

	if (actual_bytes_to_read) {
		RT_MEMORY_COPY(&memory_input_stream->memory[offset], buffer, actual_bytes_to_read);
		memory_input_stream->offset = offset + actual_bytes_to_read;
		*bytes_read = actual_bytes_to_read;
	} else {
		*bytes_read = 0;
	}

	return RT_OK;
}

rt_s rt_memory_input_stream_create(struct rt_memory_input_stream *memory_input_stream, const rt_char8 *memory, rt_un memory_size)
{
	memory_input_stream->input_stream.read = &rt_memory_input_stream_read;
	memory_input_stream->memory = memory;
	memory_input_stream->memory_size = memory_size;
	memory_input_stream->offset = 0;

	return RT_OK;
}

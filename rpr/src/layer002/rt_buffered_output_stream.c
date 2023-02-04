#include "layer002/rt_buffered_output_stream.h"

#include "layer001/rt_memory.h"

rt_s rt_buffered_output_stream_create(struct rt_buffered_output_stream *buffered_output_stream, struct rt_output_stream *real_output_stream, rt_char8 *buffer, rt_un buffer_capacity)
{
	buffered_output_stream->output_stream.write = &rt_buffered_output_stream_write;
	buffered_output_stream->output_stream.flush = &rt_buffered_output_stream_flush;
	buffered_output_stream->real_output_stream = real_output_stream;
	buffered_output_stream->buffer = buffer;
	buffered_output_stream->buffer_capacity = buffer_capacity;
	buffered_output_stream->buffer_size = 0;

	return RT_OK;
}

rt_s rt_buffered_output_stream_write(struct rt_output_stream *output_stream, const rt_char8 *data, rt_un bytes_to_write)
{
	struct rt_buffered_output_stream *buffered_output_stream = (struct rt_buffered_output_stream*)output_stream;
	struct rt_output_stream *real_output_stream = buffered_output_stream->real_output_stream;
	rt_char8 *buffer = buffered_output_stream->buffer;
	rt_un buffer_capacity = buffered_output_stream->buffer_capacity;
	rt_un buffer_size = buffered_output_stream->buffer_size;
	rt_s ret;

	if (bytes_to_write > buffer_capacity) {
		/* The buffer is too small, we won't use it. But we must write its content first. */
		if (buffer_size) {
			if (!real_output_stream->write(real_output_stream, buffer, buffer_size))
				goto error;
			buffered_output_stream->buffer_size = 0;
		}
		if (!real_output_stream->write(real_output_stream, data, bytes_to_write))
			goto error;
	} else {
		if (bytes_to_write > buffer_capacity - buffer_size) {
			/* Not enough space left in the buffer, let's write it. */
			if (!real_output_stream->write(real_output_stream, buffer, buffer_size))
				goto error;

			/* The buffer is free, let's use it. */
			RT_MEMORY_COPY(data, buffer, bytes_to_write);
			buffered_output_stream->buffer_size = bytes_to_write;
		} else {
			/* There is enough space in the buffer. */
			RT_MEMORY_COPY(data, &buffer[buffer_size], bytes_to_write);
			buffered_output_stream->buffer_size += bytes_to_write;
		}
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_buffered_output_stream_flush(struct rt_output_stream *output_stream)
{
	struct rt_buffered_output_stream *buffered_output_stream = (struct rt_buffered_output_stream*)output_stream;
	struct rt_output_stream *real_output_stream = buffered_output_stream->real_output_stream;
	rt_char8 *buffer = buffered_output_stream->buffer;
	rt_un buffer_size = buffered_output_stream->buffer_size;
	rt_s ret;

	if (buffer_size) {
		if (!real_output_stream->write(real_output_stream, buffer, buffer_size))
			goto error;
		buffered_output_stream->buffer_size = 0;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

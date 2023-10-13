#include "layer002/rt_buffered_input_stream.h"

#include "layer001/rt_memory.h"

rt_s rt_buffered_input_stream_create(struct rt_buffered_input_stream *buffered_input_stream, struct rt_input_stream *real_input_stream, rt_char8 *cache, rt_un cache_capacity)
{
	buffered_input_stream->input_stream.read = &rt_buffered_input_stream_read;
	buffered_input_stream->real_input_stream = real_input_stream;
	buffered_input_stream->cache = cache;
	buffered_input_stream->cache_capacity = cache_capacity;
	buffered_input_stream->cache_index = 0;
	buffered_input_stream->cache_size = 0;

	return RT_OK;
}

rt_s rt_buffered_input_stream_read(struct rt_input_stream *input_stream, rt_char8 *buffer, rt_un bytes_to_read, rt_un *bytes_read)
{
	struct rt_buffered_input_stream *buffered_input_stream = (struct rt_buffered_input_stream*)input_stream;
	rt_char8 *cache = buffered_input_stream->cache;
	rt_un cache_capacity = buffered_input_stream->cache_capacity;
	rt_un cache_index = buffered_input_stream->cache_index;
	rt_un cache_size = buffered_input_stream->cache_size;
	rt_un total_bytes_read;
	struct rt_input_stream *real_input_stream;
	rt_s ret;

	if (bytes_to_read <= cache_size) {
		/* There are enough bytes in the cache. */
		RT_MEMORY_COPY(&cache[cache_index], buffer, bytes_to_read);
		buffered_input_stream->cache_index += bytes_to_read;
		buffered_input_stream->cache_size -= bytes_to_read;
		*bytes_read = bytes_to_read;
	} else {
		/* We will need to use the real stream. */
		real_input_stream = buffered_input_stream->real_input_stream;

		/* Copy existing data, if any, from the cache into the buffer. */
		if (cache_size) {
			RT_MEMORY_COPY(&cache[cache_index], buffer, cache_size);
			total_bytes_read = cache_size;
			bytes_to_read -= cache_size;
		} else {
			total_bytes_read = 0;
		}

		/* At this point the cache is empty. */
		if (bytes_to_read >= cache_capacity) {
			/* The cache is not big enough. Directly use the buffer. */
			if (RT_UNLIKELY(!real_input_stream->read(real_input_stream, &buffer[total_bytes_read], bytes_to_read, bytes_read)))
				goto error;
			total_bytes_read += *bytes_read;

			/* The cache is empty. */
			buffered_input_stream->cache_index = 0;
			buffered_input_stream->cache_size = 0;

		} else {
			/* Put some new data into the cache. */
			if (RT_UNLIKELY(!real_input_stream->read(real_input_stream, cache, cache_capacity, &cache_size)))
				goto error;

			if (bytes_to_read > cache_size)
				bytes_to_read = cache_size;

			if (bytes_to_read)
				RT_MEMORY_COPY(cache, &buffer[total_bytes_read], bytes_to_read);

			buffered_input_stream->cache_index = bytes_to_read;
			buffered_input_stream->cache_size = cache_size - bytes_to_read;
			total_bytes_read += bytes_to_read;
		}

		*bytes_read = total_bytes_read;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

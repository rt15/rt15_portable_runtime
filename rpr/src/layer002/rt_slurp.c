#include "layer002/rt_slurp.h"

rt_s rt_slurp(struct rt_input_stream *input_stream, struct rt_output_stream *output_stream, rt_char8 *buffer, rt_un buffer_capacity)
{
	rt_un bytes_read;
	rt_s ret = RT_FAILED;

	while (RT_TRUE) {
		if (RT_UNLIKELY(!input_stream->read(input_stream, buffer, buffer_capacity, &bytes_read)))
			goto end;

		/* We read until there is for sure nothing left. */
		if (!bytes_read)
			break;

		if (RT_UNLIKELY(!output_stream->write(output_stream, buffer, bytes_read)))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

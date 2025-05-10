#include "zz_utils.h"

rt_s zz_start_chrono(struct rt_chrono *chrono)
{
	return rt_chrono_create(chrono);
}

rt_s zz_stop_chrono(rt_char8 *identifier, struct rt_chrono *chrono, struct rt_output_stream *output_stream)
{
	rt_un duration;
	rt_char8 buffer[200];
	rt_un buffer_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_chrono_get_duration(chrono, &duration)))
		goto end;

	duration = duration / 1000;

	buffer_size = rt_char8_get_size(identifier);
	if (RT_UNLIKELY(!rt_char8_copy(identifier, buffer_size, buffer, 200))) goto end;
	if (RT_UNLIKELY(!rt_char8_append(": ", 2, buffer, 200, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char8_append_n(duration, 10, buffer, 200, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char8_append(" ms\n", 4, buffer, 200, &buffer_size))) goto end;
	if (RT_UNLIKELY(!output_stream->write(output_stream, buffer, buffer_size))) goto end;

	ret = RT_OK;
end:
	return ret;
}

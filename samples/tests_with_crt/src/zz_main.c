#include <stdio.h>
#include <stdlib.h>

#define RT_DEFINE_USE_CRT

#include <rpr.h>

rt_s zz_test_memory_compare(struct rt_output_stream *output_stream);
rt_s zz_test_memory_copy(struct rt_output_stream *output_stream);
rt_s zz_test_memory_move(struct rt_output_stream *output_stream);
rt_s zz_test_memory_set(struct rt_output_stream *output_stream);
rt_s zz_test_memory_zero(struct rt_output_stream *output_stream);

rt_s zz_test(void)
{
	struct rt_io_device io_device;
	struct rt_output_stream *output_stream;
	rt_s ret;

	if (RT_UNLIKELY(!rt_io_device_create_from_std_output(&io_device)))
		goto error;

	output_stream = &io_device.output_stream;

	if (RT_UNLIKELY(!zz_test_memory_compare(output_stream))) goto error;
	if (RT_UNLIKELY(!output_stream->write(output_stream, "\n", 1))) goto error;

	if (RT_UNLIKELY(!zz_test_memory_copy(output_stream))) goto error;
	if (RT_UNLIKELY(!output_stream->write(output_stream, "\n", 1))) goto error;

	if (RT_UNLIKELY(!zz_test_memory_move(output_stream))) goto error;
	if (RT_UNLIKELY(!output_stream->write(output_stream, "\n", 1))) goto error;

	if (RT_UNLIKELY(!zz_test_memory_set(output_stream))) goto error;
	if (RT_UNLIKELY(!output_stream->write(output_stream, "\n", 1))) goto error;

	if (RT_UNLIKELY(!zz_test_memory_zero(output_stream))) goto error;
	if (RT_UNLIKELY(!output_stream->write(output_stream, "\n", 1))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

int main()
{
	int ret;

	if (zz_test())
		ret = 0;
	else
		ret = 1;
	return ret;
}

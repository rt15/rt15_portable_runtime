#include <rpr.h>

static rt_s zz_test_pipe_with_close(void)
{
	rt_char8 buffer[8];
	struct rt_pipe pipe;
	struct rt_io_device *input_io_device = RT_NULL;
	struct rt_io_device *output_io_device = RT_NULL;
	rt_b input_created = RT_FALSE;
	rt_b output_created = RT_FALSE;
	struct rt_input_stream *input_stream;
	struct rt_output_stream *output_stream;
	rt_un bytes_read;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_pipe_create(&pipe))) goto end;
	input_io_device = &pipe.input_io_device;
	output_io_device = &pipe.output_io_device;
	input_created = RT_TRUE;
	output_created = RT_TRUE;

	input_stream = &input_io_device->input_stream;
	output_stream = &output_io_device->output_stream;

	if (RT_UNLIKELY(!output_stream->write(output_stream, "foo", 3))) goto end;

	/* Close the pipe so that input_stream->read will read 3 bytes then encounter EOF. */
	output_created = RT_FALSE;
	if (RT_UNLIKELY(!rt_io_device_free(output_io_device))) goto end;

	RT_MEMORY_ZERO(buffer, 8);
	if (RT_UNLIKELY(!input_stream->read(input_stream, buffer, 8, &bytes_read))) goto end;

	if (RT_UNLIKELY(bytes_read != 3)) goto end;

	if (RT_UNLIKELY(RT_MEMORY_COMPARE(buffer, "foo\0\0\0\0\0", 8))) goto end;

	ret = RT_OK;
end:
	if (output_created) {
		if (RT_UNLIKELY(!rt_io_device_free(output_io_device)))
			ret = RT_FAILED;
	}
	if (input_created) {
		if (RT_UNLIKELY(!rt_io_device_free(input_io_device)))
			ret = RT_FAILED;
	}

	return ret;
}

static rt_s zz_test_pipe_with_fixed_size(struct rt_pipe *pipe)
{
	struct rt_input_stream *input_stream;
	struct rt_output_stream *output_stream;
	rt_char8 buffer[4];
	rt_un bytes_read;
	rt_s ret = RT_FAILED;

	input_stream = &pipe->input_io_device.input_stream;
	output_stream = &pipe->output_io_device.output_stream;

	if (RT_UNLIKELY(!output_stream->write(output_stream, "Foo", 3))) goto end;

	RT_MEMORY_ZERO(buffer, 4);
	if (RT_UNLIKELY(!input_stream->read(input_stream, buffer, 3, &bytes_read))) goto end;

	if (RT_UNLIKELY(RT_MEMORY_COMPARE(buffer, "Foo\0", 4))) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_pipe(void)
{
	struct rt_pipe pipe;
	rt_b pipe_created = RT_FALSE;
	rt_b is_console;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_pipe_create(&pipe)))
		goto end;
	pipe_created = RT_TRUE;

	/* Check rt_io_device_is_console. */
	if (RT_UNLIKELY(!rt_io_device_is_console(&pipe.input_io_device, &is_console))) goto end;
	if (RT_UNLIKELY(is_console)) goto end;
	if (RT_UNLIKELY(!rt_io_device_is_console(&pipe.output_io_device, &is_console))) goto end;
	if (RT_UNLIKELY(is_console)) goto end;

	if (RT_UNLIKELY(!zz_test_pipe_with_fixed_size(&pipe))) goto end;
	if (RT_UNLIKELY(!zz_test_pipe_with_close())) goto end;

	ret = RT_OK;
end:
	if (pipe_created) {
		if (RT_UNLIKELY(!rt_pipe_free(&pipe)))
			ret = RT_FAILED;
	}

	return ret;
}

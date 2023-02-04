#include <rpr.h>

static rt_s zz_test_pipe_with_close()
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
	rt_s ret;

	if (!rt_pipe_create(&pipe)) goto error;
	input_io_device = &pipe.input_io_device;
	output_io_device = &pipe.output_io_device;
	input_created = RT_TRUE;
	output_created = RT_TRUE;

	input_stream = &input_io_device->input_stream;
	output_stream = &output_io_device->output_stream;

	if (!output_stream->write(output_stream, "foo", 3)) goto error;

	/* Close the pipe so that input_stream->read will read 3 bytes then encounter EOF. */
	output_created = RT_FALSE;
	if (!rt_io_device_free(output_io_device)) goto error;

	RT_MEMORY_ZERO(buffer, 8);
	if (!input_stream->read(input_stream, buffer, 8, &bytes_read)) goto error;

	if (bytes_read != 3) goto error;

	if (RT_MEMORY_COMPARE(buffer, "foo\0\0\0\0\0", 8)) goto error;

	ret = RT_OK;
free:
	if (output_created) {
		output_created = RT_FALSE;
		if (!rt_io_device_free(output_io_device) && ret)
			goto error;
	}
	if (input_created) {
		input_created = RT_FALSE;
		if (!rt_io_device_free(input_io_device) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;

}

static rt_s zz_test_pipe_with_fixed_size(struct rt_pipe *pipe)
{
	struct rt_input_stream *input_stream;
	struct rt_output_stream *output_stream;
	rt_char8 buffer[4];
	rt_un bytes_read;
	rt_s ret;

	input_stream = &pipe->input_io_device.input_stream;
	output_stream = &pipe->output_io_device.output_stream;

	if (!output_stream->write(output_stream, "Foo", 3)) goto error;

	RT_MEMORY_ZERO(buffer, 4);
	if (!input_stream->read(input_stream, buffer, 3, &bytes_read)) goto error;

	if (RT_MEMORY_COMPARE(buffer, "Foo\0", 4)) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_pipe()
{
	struct rt_pipe pipe;
	rt_b pipe_created = RT_FALSE;
	rt_b is_console;
	rt_s ret;

	if (!rt_pipe_create(&pipe))
		goto error;
	pipe_created = RT_TRUE;

	/* Check rt_io_device_is_console. */
	if (!rt_io_device_is_console(&pipe.input_io_device, &is_console)) goto error;
	if (is_console) goto error;
	if (!rt_io_device_is_console(&pipe.output_io_device, &is_console)) goto error;
	if (is_console) goto error;

	if (!zz_test_pipe_with_fixed_size(&pipe)) goto error;
	if (!zz_test_pipe_with_close()) goto error;

	ret = RT_OK;
free:
	if (pipe_created) {
		pipe_created = RT_FALSE;
		if (!rt_pipe_free(&pipe) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

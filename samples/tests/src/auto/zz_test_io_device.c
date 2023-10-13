#include <rpr.h>

static rt_s zz_test_io_device_inheritance(struct rt_io_device *io_device)
{
	rt_b inheritable;
	rt_s ret;

	/* All files must not be inheritable by default. */
	if (RT_UNLIKELY(!rt_io_device_is_inheritable(io_device, &inheritable))) goto error;
	if (RT_UNLIKELY(inheritable)) goto error;

	/* Switch to inheritable. */
	if (RT_UNLIKELY(!rt_io_device_set_inheritable(io_device, RT_TRUE))) goto error;
	if (RT_UNLIKELY(!rt_io_device_is_inheritable(io_device, &inheritable))) goto error;
	if (RT_UNLIKELY(!inheritable)) goto error;

	/* Try again. */
	if (RT_UNLIKELY(!rt_io_device_set_inheritable(io_device, RT_TRUE))) goto error;
	if (RT_UNLIKELY(!rt_io_device_is_inheritable(io_device, &inheritable))) goto error;
	if (RT_UNLIKELY(!inheritable)) goto error;

	/* Switch back to non-inheritable. */
	if (RT_UNLIKELY(!rt_io_device_set_inheritable(io_device, RT_FALSE))) goto error;
	if (RT_UNLIKELY(!rt_io_device_is_inheritable(io_device, &inheritable))) goto error;
	if (RT_UNLIKELY(inheritable)) goto error;

	/* Try again. */
	if (RT_UNLIKELY(!rt_io_device_set_inheritable(io_device, RT_FALSE))) goto error;
	if (RT_UNLIKELY(!rt_io_device_is_inheritable(io_device, &inheritable))) goto error;
	if (RT_UNLIKELY(inheritable)) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_io_device()
{
	struct rt_pipe pipe;
	rt_b pipe_created = RT_FALSE;
	rt_s ret;

	if (RT_UNLIKELY(!rt_pipe_create(&pipe)))
		goto error;
	pipe_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_test_io_device_inheritance(&pipe.input_io_device))) goto error;

	ret = RT_OK;
free:
	if (pipe_created) {
		pipe_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_pipe_free(&pipe) && ret))
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

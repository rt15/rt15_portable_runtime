#include <rpr.h>

static rt_s zz_test_io_device_inheritance(struct rt_io_device *io_device)
{
	rt_b inheritable;
	rt_s ret = RT_FAILED;

	/* All files must not be inheritable by default. */
	if (RT_UNLIKELY(!rt_io_device_is_inheritable(io_device, &inheritable))) goto end;
	if (RT_UNLIKELY(inheritable)) goto end;

	/* Switch to inheritable. */
	if (RT_UNLIKELY(!rt_io_device_set_inheritable(io_device, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!rt_io_device_is_inheritable(io_device, &inheritable))) goto end;
	if (RT_UNLIKELY(!inheritable)) goto end;

	/* Try again. */
	if (RT_UNLIKELY(!rt_io_device_set_inheritable(io_device, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!rt_io_device_is_inheritable(io_device, &inheritable))) goto end;
	if (RT_UNLIKELY(!inheritable)) goto end;

	/* Switch back to non-inheritable. */
	if (RT_UNLIKELY(!rt_io_device_set_inheritable(io_device, RT_FALSE))) goto end;
	if (RT_UNLIKELY(!rt_io_device_is_inheritable(io_device, &inheritable))) goto end;
	if (RT_UNLIKELY(inheritable)) goto end;

	/* Try again. */
	if (RT_UNLIKELY(!rt_io_device_set_inheritable(io_device, RT_FALSE))) goto end;
	if (RT_UNLIKELY(!rt_io_device_is_inheritable(io_device, &inheritable))) goto end;
	if (RT_UNLIKELY(inheritable)) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_io_device(void)
{
	struct rt_pipe pipe;
	rt_b pipe_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_pipe_create(&pipe)))
		goto end;
	pipe_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_test_io_device_inheritance(&pipe.input_io_device))) goto end;

	ret = RT_OK;
end:
	if (pipe_created) {
		if (RT_UNLIKELY(!rt_pipe_free(&pipe)))
			ret = RT_FAILED;
	}
	return ret;
}

#include "layer003/rt_pipe.h"

#include "layer001/rt_os_headers.h"

rt_s rt_pipe_create(struct rt_pipe *pipe)
{
#ifdef RT_DEFINE_WINDOWS
	rt_h input_handle;
	rt_h output_handle;
#else
	rt_n32 pipes[2];
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	/* In case of failure, returns zero and set last error. */
	if (RT_UNLIKELY(!CreatePipe(&input_handle, &output_handle, RT_NULL, 0)))
		goto error;
	rt_io_device_create_from_handle(&pipe->input_io_device, input_handle);
	rt_io_device_create_from_handle(&pipe->output_io_device, output_handle);
#else
	/* In case of failure, returns -1 and set errno. */
	if (RT_UNLIKELY(pipe2(pipes, O_CLOEXEC) == -1))
		goto error;
	rt_io_device_create_from_file_descriptor(&pipe->input_io_device, pipes[0]);
	rt_io_device_create_from_file_descriptor(&pipe->output_io_device, pipes[1]);
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_pipe_free(struct rt_pipe *pipe)
{
	rt_s ret;

	/* In all cases, we must try to free both devices. If one fail, then this function fails. */
	ret = RT_OK;
	if (!rt_io_device_free(&pipe->input_io_device)) ret = RT_FAILED;
	if (!rt_io_device_free(&pipe->output_io_device)) ret = RT_FAILED;

	return ret;
}

#include <stdio.h>
#include <stdlib.h>

#include <rpr.h>
#include <rpr_main.h>

/* Layer 001. */
rt_s zz_test_atomic();
rt_s zz_test_memory();

/* Layer 002. */
rt_s zz_test_io_device();
rt_s zz_test_sleep();

/* Layer 003. */
rt_s zz_test_char8();
rt_s zz_test_pipe();
rt_s zz_test_thread();


static rt_s zz_test()
{
	rt_s ret;
	struct rt_io_device io_device;
	struct rt_output_stream *output_stream;

	if (!rt_io_device_create_from_std_output(&io_device))
		goto error;

	output_stream = &io_device.output_stream;
	if (!output_stream->write(output_stream, "Hello, world!\n", rt_char8_get_size("Hello, world!\n")))
		goto error;

	/* Layer 001. */
	if (!zz_test_atomic()) goto error;
	if (!zz_test_memory()) goto error;

	/* Layer 002. */
	if (!zz_test_io_device()) goto error;
	if (!zz_test_sleep()) goto error;

	/* Layer 003. */
	if (!zz_test_char8()) goto error;
	if (!zz_test_pipe()) goto error;
	if (!zz_test_thread()) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_un16 rpr_main(rt_n32 argc, rt_char *argv[])
{
	int ret;

	if (zz_test())
		ret = 0;
	else
		ret = 1;
	return ret;
}

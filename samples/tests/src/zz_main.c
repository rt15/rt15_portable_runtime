#include <stdio.h>
#include <stdlib.h>

#include <rpr.h>
#include <rpr_mem.h>
#include <rpr_main.h>

rt_s zz_test_char8();
rt_s zz_test_sleep();

rt_s zz_test()
{
	rt_s ret;
	struct rt_io_device io_device;
	struct rt_output_stream *output_stream;

	if (!rt_io_device_create_from_std_output(&io_device))
		goto error;

	output_stream = &io_device.output_stream;
	if (!output_stream->write(output_stream, "Hello, world!\n", rt_char8_get_size("Hello, world!\n")))
		goto error;

	if (!zz_test_sleep()) goto error;
	if (!zz_test_char8()) goto error;

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

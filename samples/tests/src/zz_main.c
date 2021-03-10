#include <stdio.h>
#include <stdlib.h>

#include <rpr.h>
#include <rpr_main.h>

/* Layer 001. */
rt_s zz_test_atomic();
rt_s zz_test_memory();

/* Layer 002. */
rt_s zz_test_critical_section();
rt_s zz_test_event();
rt_s zz_test_io_device();
rt_s zz_test_sleep();
rt_s zz_test_virtual_memory();

/* Layer 003. */
rt_s zz_test_char8();
rt_s zz_test_pipe();
rt_s zz_test_thread();


static rt_s zz_test()
{
	rt_s ret;
	struct rt_io_device io_device;
	struct rt_output_stream *output_stream = RT_NULL;

	if (!rt_io_device_create_from_std_output(&io_device))
		goto error;

	output_stream = &io_device.output_stream;
	if (!output_stream->write(output_stream, "Testing...\n", rt_char8_get_size("Testing...\n")))
		goto error;

	if (!rt_check_rpr()) goto error;

	/* Layer 001. */
	if (!zz_test_atomic()) goto error;
	if (!zz_test_memory()) goto error;

	/* Layer 002. */
	if (!zz_test_critical_section()) goto error;
	if (!zz_test_event()) goto error;
	if (!zz_test_io_device()) goto error;
	if (!zz_test_sleep()) goto error;
	if (!zz_test_virtual_memory()) goto error;

	/* Layer 003. */
	if (!zz_test_char8()) goto error;
	if (!zz_test_pipe()) goto error;
	if (!zz_test_thread()) goto error;

	if (!output_stream->write(output_stream, "Tests successful!\n", rt_char8_get_size("Tests successful!\n")))
		goto error;

	ret = RT_OK;
free:
	return ret;
error:
	if (output_stream)
		output_stream->write(output_stream, "Tests failed!\n", rt_char8_get_size("Tests failed!\n"));

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

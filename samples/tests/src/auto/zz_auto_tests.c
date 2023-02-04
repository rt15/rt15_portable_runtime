#include <rpr.h>

/* Layer 001. */
rt_s zz_test_atomic();
rt_s zz_test_memory();

/* Layer 002. */
rt_s zz_test_binary_search();
rt_s zz_test_critical_section();
rt_s zz_test_event();
rt_s zz_test_io_device();
rt_s zz_test_memory_input_stream();
rt_s zz_test_quick_sort();
rt_s zz_test_sleep();
rt_s zz_test_slurp();
rt_s zz_test_virtual_memory();

/* Layer 003. */
rt_s zz_test_buffered_input_stream();
rt_s zz_test_char();
rt_s zz_test_char8();
rt_s zz_test_env_var();
rt_s zz_test_fast_initialization();
rt_s zz_test_file();
rt_s zz_test_heap();
rt_s zz_test_pipe();
rt_s zz_test_read_lines();
rt_s zz_test_thread();

/* Layer 004. */
rt_s zz_test_address();
rt_s zz_test_array();
rt_s zz_test_base64();
rt_s zz_test_command_line_args();
rt_s zz_test_encoding();
rt_s zz_test_file_path();
rt_s zz_test_memory_output_stream();
rt_s zz_test_os_version();
rt_s zz_test_random();
rt_s zz_test_small_file();
rt_s zz_test_static_heap();
rt_s zz_test_url();

/* Layer 005. */
rt_s zz_test_deduce_encoding();
rt_s zz_test_env_vars();
rt_s zz_test_file_system();
rt_s zz_test_hash_table();
rt_s zz_test_list();
rt_s zz_test_socket_address();
rt_s zz_test_sortable_array();
rt_s zz_test_unicode_code_point();

/* Layer 006. */
rt_s zz_test_error_message();
rt_s zz_test_properties();
rt_s zz_test_socket();

/* Layer 007. */
rt_s zz_test_process();
rt_s zz_test_select();

rt_s zz_auto_tests()
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
	if (!zz_test_binary_search()) goto error;
	if (!zz_test_critical_section()) goto error;
	if (!zz_test_event()) goto error;
	if (!zz_test_io_device()) goto error;
	if (!zz_test_memory_input_stream()) goto error;
	if (!zz_test_quick_sort()) goto error;
	if (!zz_test_sleep()) goto error;
	if (!zz_test_slurp()) goto error;
	if (!zz_test_virtual_memory()) goto error;

	/* Layer 003. */
	if (!zz_test_buffered_input_stream()) goto error;
	if (!zz_test_char()) goto error;
	if (!zz_test_char8()) goto error;
	if (!zz_test_env_var()) goto error;
	if (!zz_test_fast_initialization()) goto error;
	if (!zz_test_file()) goto error;
	if (!zz_test_heap()) goto error;
	if (!zz_test_pipe()) goto error;
	if (!zz_test_read_lines()) goto error;
	if (!zz_test_thread()) goto error;

	/* Layer 004. */
	if (!zz_test_address()) goto error;
	if (!zz_test_array()) goto error;
	if (!zz_test_base64()) goto error;
	if (!zz_test_command_line_args()) goto error;
	if (!zz_test_encoding()) goto error;
	if (!zz_test_file_path()) goto error;
	if (!zz_test_memory_output_stream()) goto error;
	if (!zz_test_os_version()) goto error;
	if (!zz_test_random()) goto error;
	if (!zz_test_small_file()) goto error;
	if (!zz_test_static_heap()) goto error;
	if (!zz_test_url()) goto error;

	/* Layer 005. */
	if (!zz_test_deduce_encoding()) goto error;
	if (!zz_test_env_vars()) goto error;
	if (!zz_test_file_system()) goto error;
	if (!zz_test_hash_table()) goto error;
	if (!zz_test_list()) goto error;
	if (!zz_test_socket_address()) goto error;
	if (!zz_test_sortable_array()) goto error;
	if (!zz_test_unicode_code_point()) goto error;

	/* Layer 006. */
	if (!zz_test_error_message()) goto error;
	if (!zz_test_properties()) goto error;
	if (!zz_test_socket()) goto error;

	/* Layer 007. */
	if (!zz_test_process()) goto error;
	if (!zz_test_select()) goto error;

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

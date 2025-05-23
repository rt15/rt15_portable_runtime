#include <rpr.h>

/* Layer 001. */
rt_s zz_test_atomic(void);
rt_s zz_test_memory(void);

/* Layer 002. */
rt_s zz_test_binary_search(void);
rt_s zz_test_buffered_input_stream(void);
rt_s zz_test_critical_section(void);
rt_s zz_test_double(void);
rt_s zz_test_event(void);
rt_s zz_test_float(void);
rt_s zz_test_io_device(void);
rt_s zz_test_memory_input_stream(void);
rt_s zz_test_quick_sort(void);
rt_s zz_test_shared_mutex(void);
rt_s zz_test_sleep(void);
rt_s zz_test_slurp(void);
rt_s zz_test_virtual_memory(void);

/* Layer 003. */
rt_s zz_test_char(void);
rt_s zz_test_char8(void);
rt_s zz_test_date(void);
rt_s zz_test_env_var(void);
rt_s zz_test_fast_initialization(void);
rt_s zz_test_file(void);
rt_s zz_test_file_mapping(void);
rt_s zz_test_heap(void);
rt_s zz_test_library(void);
rt_s zz_test_pipe(void);
rt_s zz_test_read_lines(void);
rt_s zz_test_semaphore(void);
rt_s zz_test_shared_memory(void);
rt_s zz_test_spinlock(void);
rt_s zz_test_thread(void);

/* Layer 004. */
rt_s zz_test_address(void);
rt_s zz_test_array(void);
rt_s zz_test_base64(void);
rt_s zz_test_command_line_args(void);
rt_s zz_test_encoding(void);
rt_s zz_test_file_path(void);
rt_s zz_test_memory_output_stream(void);
rt_s zz_test_os_version(void);
rt_s zz_test_random(void);
rt_s zz_test_readers_writer_mutex(void);
rt_s zz_test_small_file(void);
rt_s zz_test_static_heap(void);
rt_s zz_test_time(void);
rt_s zz_test_url(void);
rt_s zz_test_uuid(void);

/* Layer 005. */
rt_s zz_test_deduce_encoding(void);
rt_s zz_test_env_vars(void);
rt_s zz_test_file_system(void);
rt_s zz_test_hash_table(void);
rt_s zz_test_list(void);
rt_s zz_test_socket_address(void);
rt_s zz_test_sortable_array(void);
rt_s zz_test_unicode_code_point(void);

/* Layer 006. */
rt_s zz_test_error_message(void);
rt_s zz_test_properties(void);
rt_s zz_test_socket(void);

/* Layer 007. */
rt_s zz_test_process(void);
rt_s zz_test_select(void);

static rt_s zz_auto_tests_log(const rt_char8 *test, struct rt_output_stream *output_stream)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!output_stream->write(output_stream, test, rt_char8_get_size(test))))
		goto end;

	if (RT_UNLIKELY(!output_stream->write(output_stream, "...\n", 4)))
		goto end;

	ret = RT_OK;
end:
	return ret;

}

rt_s zz_auto_tests(void)
{
	rt_s ret = RT_FAILED;
	struct rt_io_device io_device;
	struct rt_output_stream *output_stream = RT_NULL;

	if (RT_UNLIKELY(!rt_io_device_create_from_std_output(&io_device)))
		goto end;

	output_stream = &io_device.output_stream;
	if (RT_UNLIKELY(!output_stream->write(output_stream, "Testing...\n", 11)))
		goto end;

	if (RT_UNLIKELY(!zz_auto_tests_log("rt_check_rpr", output_stream))) goto end;
	if (RT_UNLIKELY(!rt_check_rpr())) goto end;

	/* Layer 001. */
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_atomic", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_atomic())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_memory", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_memory())) goto end;

	/* Layer 002. */
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_binary_search", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_binary_search())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_buffered_input_stream", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_buffered_input_stream())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_critical_section", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_critical_section())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_double", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_double())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_event", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_event())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_float", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_float())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_io_device", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_io_device())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_memory_input_stream", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_memory_input_stream())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_quick_sort", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_quick_sort())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_shared_mutex", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_shared_mutex())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_sleep", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_sleep())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_slurp", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_slurp())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_virtual_memory", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_virtual_memory())) goto end;

	/* Layer 003. */
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_char", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_char())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_char8", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_char8())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_date", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_date())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_env_var", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_env_var())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_fast_initialization", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_fast_initialization())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_file", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_file())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_file_mapping", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_file_mapping())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_heap", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_heap())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_library", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_library())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_pipe", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_pipe())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_read_lines", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_read_lines())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_semaphore", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_semaphore())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_shared_memory", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_shared_memory())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_spinlock", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_spinlock())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_thread", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_thread())) goto end;

	/* Layer 004. */
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_address", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_address())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_array", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_array())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_base64", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_base64())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_command_line_args", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_command_line_args())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_encoding", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_encoding())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_file_path", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_file_path())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_memory_output_stream", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_memory_output_stream())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_os_version", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_os_version())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_random", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_random())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_readers_writer_mutex", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_readers_writer_mutex())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_small_file", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_small_file())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_static_heap", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_static_heap())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_time", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_time())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_url", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_url())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_uuid", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_uuid())) goto end;

	/* Layer 005. */
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_deduce_encoding", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_deduce_encoding())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_env_vars", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_env_vars())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_file_system", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_file_system())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_hash_table", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_list", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_list())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_socket_address", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_socket_address())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_sortable_array", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_sortable_array())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_unicode_code_point", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_unicode_code_point())) goto end;

	/* Layer 006. */
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_error_message", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_error_message())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_properties", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_properties())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_socket", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_socket())) goto end;

	/* Layer 007. */
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_process", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_process())) goto end;
	if (RT_UNLIKELY(!zz_auto_tests_log("zz_test_select", output_stream))) goto end;
	if (RT_UNLIKELY(!zz_test_select())) goto end;

	if (RT_UNLIKELY(!output_stream->write(output_stream, "Tests successful!\n", 18)))
		goto end;

	ret = RT_OK;
end:
	if (!ret) {
		if (output_stream)
			output_stream->write(output_stream, "Tests failed!\n", rt_char8_get_size("Tests failed!\n"));
	}

	return ret;
}

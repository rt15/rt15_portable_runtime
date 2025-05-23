#include <rpr.h>

#include "zz_utils.h"

static rt_s zz_test_process_redirect_std_in_to_pipe(const rt_char* executable_path)
{
	rt_char temp_file[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
	struct rt_file file;
	rt_b file_created;
	rt_b delete_temp_file;
	struct rt_pipe pipe;
	struct rt_io_device *input_io_device;
	struct rt_io_device *output_io_device;
	rt_b input_created = RT_FALSE;
	rt_b output_created = RT_FALSE;
	const rt_char *application_path_and_args[3];
	struct rt_process process;
	rt_b process_created = RT_FALSE;
	struct rt_output_stream *output_stream;
	rt_un32 exit_code;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_temp_file_create(&file, _R("Zz"), temp_file, RT_FILE_PATH_SIZE, &buffer_size))) goto end;
	file_created = RT_TRUE;
	delete_temp_file = RT_TRUE;

	if (RT_UNLIKELY(!rt_pipe_create(&pipe))) goto end;
	input_io_device = &pipe.input_io_device;
	output_io_device = &pipe.output_io_device;
	input_created = RT_TRUE;
	output_created = RT_TRUE;

	application_path_and_args[0] = executable_path;
	application_path_and_args[1] = _R("--read-line");
	application_path_and_args[2] = RT_NULL;

	if (RT_UNLIKELY(!rt_process_create_with_redirections(&process, RT_TRUE, RT_NULL, RT_NULL, input_io_device, &file.io_device, RT_NULL, application_path_and_args))) goto end;
	process_created = RT_TRUE;

	output_stream = &output_io_device->output_stream;
	if (RT_UNLIKELY(!output_stream->write(output_stream, "123\n", 4))) goto end;

	if (RT_UNLIKELY(!rt_process_join(&process))) goto end;
	if (RT_UNLIKELY(!rt_process_get_exit_code(&process, &exit_code))) goto end;
	if (RT_UNLIKELY(exit_code)) goto end;

	if (RT_UNLIKELY(!zz_check_file_content(temp_file, "\"123\"\n"))) goto end;

	ret = RT_OK;
end:
	if (process_created) {
		if (RT_UNLIKELY(!rt_process_free(&process)))
			ret = RT_FAILED;
	}
	if (output_created) {
		if (RT_UNLIKELY(!rt_io_device_free(output_io_device)))
			ret = RT_FAILED;
	}
	if (input_created) {
		if (RT_UNLIKELY(!rt_io_device_free(input_io_device)))
			ret = RT_FAILED;
	}
	if (file_created) {
		if (RT_UNLIKELY(!rt_io_device_free(&file.io_device)))
			ret = RT_FAILED;
	}
	if (delete_temp_file) {
		if (RT_UNLIKELY(!rt_file_system_delete_file(temp_file)))
			ret = RT_FAILED;
	}

	return ret;
}

static rt_s zz_test_process_redirect_std_out_to_file(const rt_char* executable_path)
{
	rt_char temp_file[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
	rt_b file_created = RT_FALSE;
	rt_b delete_temp_file = RT_FALSE;
	struct rt_file file;
	const rt_char *application_path_and_args[13];
	struct rt_process process;
	rt_b process_created = RT_FALSE;
	rt_un32 exit_code;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_temp_file_create(&file, _R("Zz"), temp_file, RT_FILE_PATH_SIZE, &buffer_size))) goto end;
	file_created = RT_TRUE;
	delete_temp_file = RT_TRUE;

	application_path_and_args[0]  = executable_path;
	application_path_and_args[1]  = _R("--parse-args");
	application_path_and_args[2]  = _R("foo");
	application_path_and_args[3]  = _R("a b");
	application_path_and_args[4]  = _R("c:\\");
	application_path_and_args[5]  = _R("c:\\Program Files\\");
	application_path_and_args[6]  = _R("c:\\\\Program Files\\\\");
	application_path_and_args[7]  = _R("\"bar\"");
	application_path_and_args[8]  = _R("b\\a\\r");
	application_path_and_args[9]  = _R("b\\\t\\r");
	application_path_and_args[10] = _R("b\"r");
	application_path_and_args[11] = _R("b\\\"r");
	application_path_and_args[12] = RT_NULL;

	if (RT_UNLIKELY(!rt_process_create_with_redirections(&process, RT_TRUE, RT_NULL, RT_NULL, RT_NULL, &file.io_device, RT_NULL, application_path_and_args))) goto end;
	process_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_process_join(&process))) goto end;
	if (RT_UNLIKELY(!rt_process_get_exit_code(&process, &exit_code))) goto end;
	if (RT_UNLIKELY(exit_code)) goto end;

	if (RT_UNLIKELY(!zz_check_file_content(temp_file, "\n"
							  "Non-options:"            "\n"
							  "foo"                     "\n"
							  "a b"                     "\n"
							  "c:\\"                    "\n"
							  "c:\\Program Files\\"     "\n"
							  "c:\\\\Program Files\\\\" "\n"
							  "\"bar\""                 "\n"
							  "b\\a\\r"                 "\n"
							  "b\\\t\\r"                "\n"
							  "b\"r"                    "\n"
							  "b\\\"r"                  "\n")))
		goto end;

	ret = RT_OK;
end:
	if (process_created) {
		if (RT_UNLIKELY(!rt_process_free(&process)))
			ret = RT_FAILED;
	}
	if (file_created) {
		if (RT_UNLIKELY(!rt_io_device_free(&file.io_device)))
			ret = RT_FAILED;
	}
	if (delete_temp_file) {
		if (RT_UNLIKELY(!rt_file_system_delete_file(temp_file)))
			ret = RT_FAILED;
	}

	return ret;
}

static rt_s zz_test_process_redirect_std_err_to_file(const rt_char* executable_path)
{
	rt_char temp_file[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
	rt_b file_created = RT_FALSE;
	rt_b delete_temp_file = RT_FALSE;
	struct rt_file file;
	const rt_char *application_path_and_args[3];
	struct rt_process process;
	rt_b process_created = RT_FALSE;
	rt_un32 exit_code;
	rt_un64 file_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_temp_file_create(&file, _R("Zz"), temp_file, RT_FILE_PATH_SIZE, &buffer_size))) goto end;
	file_created = RT_TRUE;
	delete_temp_file = RT_TRUE;

	application_path_and_args[0] = executable_path;
	application_path_and_args[1] = _R("--bad");
	application_path_and_args[2] = RT_NULL;

	if (RT_UNLIKELY(!rt_process_create_with_redirections(&process, RT_TRUE, RT_NULL, RT_NULL, RT_NULL, RT_NULL, &file.io_device, application_path_and_args))) goto end;
	process_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_process_join(&process))) goto end;
	if (RT_UNLIKELY(!rt_process_get_exit_code(&process, &exit_code))) goto end;
	if (RT_UNLIKELY(!exit_code)) goto end;

	if (RT_UNLIKELY(!rt_file_system_get_file_size(temp_file, &file_size))) goto end;
	if (RT_UNLIKELY(file_size < 10)) goto end;

	ret = RT_OK;
end:
	if (process_created) {
		if (RT_UNLIKELY(!rt_process_free(&process)))
			ret = RT_FAILED;
	}
	if (file_created) {
		if (RT_UNLIKELY(!rt_io_device_free(&file.io_device)))
			ret = RT_FAILED;
	}
	if (delete_temp_file) {
		if (RT_UNLIKELY(!rt_file_system_delete_file(temp_file)))
			ret = RT_FAILED;
	}

	return ret;
}

static rt_s zz_test_process_env(const rt_char* executable_path)
{
	struct rt_pipe pipe;
	struct rt_io_device *input_io_device;
	struct rt_io_device *output_io_device;
	rt_b input_created = RT_FALSE;
	rt_b output_created = RT_FALSE;
	struct rt_input_stream *input_stream;
	struct rt_env_vars env_vars;
	rt_b env_vars_created = RT_FALSE;
	const rt_char *application_path_and_args[4];
	struct rt_process process;
	rt_b process_created = RT_FALSE;
	rt_un32 exit_code;
	rt_char8 pipe_content[256];
	rt_un bytes_read;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_pipe_create(&pipe))) goto end;
	input_io_device = &pipe.input_io_device;
	output_io_device = &pipe.output_io_device;
	input_created = RT_TRUE;
	output_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_env_vars_create(&env_vars))) goto end;
	env_vars_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_env_vars_merge_env_var(&env_vars, _R("RT_PROCESS_VAR"), _R("VAR_VALUE")))) goto end;

	application_path_and_args[0] = executable_path;
	application_path_and_args[1] = _R("--display-env-var");
	application_path_and_args[2] = _R("RT_PROCESS_VAR");
	application_path_and_args[3] = RT_NULL;

	if (RT_UNLIKELY(!rt_process_create_with_redirections(&process, RT_TRUE, RT_NULL, &env_vars, RT_NULL, output_io_device, RT_NULL, application_path_and_args))) goto end;
	process_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_process_join(&process))) goto end;
	if (RT_UNLIKELY(!rt_process_get_exit_code(&process, &exit_code))) goto end;
	if (RT_UNLIKELY(exit_code)) goto end;

	/* Close writing pipe on parent side. */
	output_created = RT_FALSE;
	if (RT_UNLIKELY(!rt_io_device_free(output_io_device))) goto end;

	input_stream = &input_io_device->input_stream;
	if (RT_UNLIKELY(!input_stream->read(input_stream, pipe_content, 256 - 1, &bytes_read))) goto end;
	pipe_content[bytes_read] = 0;
	if (RT_UNLIKELY(!rt_char8_equals("RT_PROCESS_VAR=VAR_VALUE\n", rt_char8_get_size("RT_PROCESS_VAR=VAR_VALUE\n"), pipe_content, bytes_read))) goto end;

	ret = RT_OK;
end:
	if (process_created) {
		if (RT_UNLIKELY(!rt_process_free(&process)))
			ret = RT_FAILED;
	}
	if (env_vars_created) {
		if (RT_UNLIKELY(!rt_env_vars_free(&env_vars)))
			ret = RT_FAILED;
	}
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

static rt_s zz_test_process_failed(void)
{
	const rt_char *application_path_and_args[3];
	struct rt_process process;
	rt_un32 exit_code;
	rt_s ret = RT_FAILED;

	/* Wrong executable. */
	application_path_and_args[0] = _R("ppppppppppppiiiiiiiiiiiinnnnnnnnnnngggggggggg");
	application_path_and_args[1] = _R("localhost");
	application_path_and_args[2] = RT_NULL;
	if (RT_UNLIKELY(rt_process_create(&process, RT_TRUE,  RT_NULL, RT_NULL, application_path_and_args))) goto end;
	if (RT_UNLIKELY(rt_process_create(&process, RT_FALSE, RT_NULL, RT_NULL, application_path_and_args))) goto end;

	/* Test wrong folder name. */
	application_path_and_args[0] = _R("ping");
	if (RT_UNLIKELY(rt_process_create(&process, RT_TRUE,  _R("Wrong, wrong directory"), RT_NULL, application_path_and_args))) goto end;
	if (RT_UNLIKELY(rt_process_create(&process, RT_FALSE, _R("Wrong, wrong directory"), RT_NULL, application_path_and_args))) goto end;

	/* Test bad argument. */
	application_path_and_args[1] = _R("--invalid-arg");
	if (RT_UNLIKELY(!rt_process_spawn_sync(RT_NULL, RT_NULL, application_path_and_args, &exit_code))) goto end;
	if (RT_UNLIKELY(!exit_code)) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_process(void)
{
	rt_char executable_path[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_file_path_get_executable_path(executable_path, RT_FILE_PATH_SIZE, &buffer_size))) goto end;

	if (RT_UNLIKELY(!zz_test_process_redirect_std_in_to_pipe(executable_path))) goto end;
	if (RT_UNLIKELY(!zz_test_process_redirect_std_out_to_file(executable_path))) goto end;
	if (RT_UNLIKELY(!zz_test_process_redirect_std_err_to_file(executable_path))) goto end;
	if (RT_UNLIKELY(!zz_test_process_env(executable_path))) goto end;
	if (RT_UNLIKELY(!zz_test_process_failed())) goto end;

	ret = RT_OK;
end:
	return ret;
}

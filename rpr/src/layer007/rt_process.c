#include "layer007/rt_process.h"

#include "layer001/rt_memory.h"
#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer003/rt_char.h"
#include "layer003/rt_pipe.h"
#include "layer004/rt_static_heap.h"
#include "layer005/rt_console.h"
#include "layer006/rt_error_message.h"

rt_s rt_process_create(struct rt_process *process, rt_b child, const rt_char *current_dir, struct rt_env_vars *env_vars, const rt_char *const *application_path_and_args)
{
	return rt_process_create_with_redirections(process, child, current_dir, env_vars, RT_NULL, RT_NULL, RT_NULL, application_path_and_args);
}

rt_s rt_process_spawn_sync(const rt_char *current_dir, struct rt_env_vars *env_vars, const rt_char *const *application_path_and_args, rt_un32 *exit_code)
{
	struct rt_process process;
	rt_b process_created = RT_FALSE;
	rt_s ret;

	if (!rt_process_create(&process, RT_TRUE, current_dir, env_vars, application_path_and_args))
		goto error;
	process_created = RT_TRUE;

	if (!rt_process_join(&process))
		goto error;
	if (!rt_process_get_exit_code(&process, exit_code))
		goto error;

	ret = RT_OK;
free:
	if (process_created) {
		process_created = RT_FALSE;
		if (!rt_process_free(&process) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_process_spawn_async(const rt_char *current_dir, struct rt_env_vars *env_vars, const rt_char *const *application_path_and_args)
{
	struct rt_process process;
	rt_b process_created = RT_FALSE;
	rt_s ret;

	if (!rt_process_create(&process, RT_FALSE, current_dir, env_vars, application_path_and_args))
		goto error;
	process_created = RT_TRUE;

	ret = RT_OK;
free:
	if (process_created) {
		process_created = RT_FALSE;
		if (!rt_process_free(&process) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

#ifdef RT_DEFINE_WINDOWS

/**
 * Convert given arg so it can be used in command line.<br>
 * See <tt>rt_process_argv_to_command_line</tt>.
 *
 * @param buffer_size In/out parameter as this function appends to the buffer.
 */
static rt_s rt_process_convert_arg_to_command_line(const rt_char *arg, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_b need_double_quotes;
	rt_un anti_slashes_count;
	rt_un i;
	rt_un j;
	rt_s ret;

	/* Need double-quotes? */
	need_double_quotes = RT_FALSE;
	i = 0;
	while (arg[i]) {
		if ((arg[i] == _R(' '))  ||
		    (arg[i] == _R('\t')) ||
		    (arg[i] == _R('"'))) {
			need_double_quotes = RT_TRUE;
			break;
		}
		i++;
	}

	if (need_double_quotes) {
		/* Open double-quotes. */
		if (!rt_char_append_char(_R('"'), buffer, buffer_capacity, buffer_size))
			goto error;

		i = 0;
		anti_slashes_count = 0;
		while (arg[i]) {
			if (arg[i] == _R('\\')) {
				anti_slashes_count++;
			} else {
				if (arg[i] == _R('"')) {
					for (j = 0; j < anti_slashes_count; j++) {
						/* Escape anti-slashes just before the double quote. */
						if (!rt_char_append_char(_R('\\'), buffer, buffer_capacity, buffer_size))
							goto error;
					}

					/* Escape double-quote. */
					if (!rt_char_append_char(_R('\\'), buffer, buffer_capacity, buffer_size))
						goto error;
				}
				anti_slashes_count = 0;
			}

			/* Copy character. */
			if (!rt_char_append_char(arg[i], buffer, buffer_capacity, buffer_size))
				goto error;

			i++;
		}

		/* Protect possible slashes before added closing double-quote. */
		for (i = 0; i < anti_slashes_count; i++) {
			if (!rt_char_append_char(_R('\\'), buffer, buffer_capacity, buffer_size))
				goto error;
		}

		/* Close double-quotes. */
		if (!rt_char_append_char(_R('"'), buffer, buffer_capacity, buffer_size))
			goto error;
	} else {
		/* No need to add double-quotes, simply append the argument. */
		if (!rt_char_append(arg, rt_char_get_size(arg), buffer, buffer_capacity, buffer_size))
			goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

/**
 * Compute a command line size big enough (and sometimes bigger) for the given <tt>arg</tt>.
 *
 * <p>
 * If there are ' ' (spaces), '\t' (tabs) or '"' (double-quotes), we need 2 more characters for double-quotes.<br>
 * If double-quotes are needed, add 1 character per anti-slash (Not always required) and per '"' (Always required).
 * </p>
 */
static rt_un rt_process_get_arg_size_in_command_line(const rt_char *arg)
{
	rt_un anti_slashes_count = 0;
	rt_un double_quotes_count = 0;
	rt_un spaces_count = 0;
	rt_un result = 0;

	while (arg[result]) {
		if (arg[result] == _R(' ') || arg[result] == _R('\t')) {
			spaces_count++;
		} else if (arg[result] == _R('"')) {
			double_quotes_count++;
		} else if (arg[result] == _R('\\')) {
			anti_slashes_count++;
		}
		result++;
	}

	if (spaces_count || double_quotes_count) {
		/* We need to surround with double-quotes. */
		result += 2;
		/* Add room for double quotes protections. */
		result += double_quotes_count;
		/* Add room for possibly required anti-slashes protections. */
		result += anti_slashes_count;
	}

	return result;
}

/**
 * Opposite of CommandLineToArgvW.
 *
 * <p>
 * If an argument contains ' ' (space), '\t' (tab) or '"' (double quote) then it must be surrounded with double-quotes (No need to surround arguments with anti-slashes only).<br>
 * If an argument needs double-quotes then:<br>
 * Prepend anti-slash before any double-quote.<br>
 * Prepend anti-slash for every anti-slashes just before a double-quote.<br>
 * </p>
 *
 * <ul>
 *   <li>Use "    to start/end a double quoted part (arg)</li>
 *   <li>Use \"   to insert a literal "</li>
 *   <li>Use \\"  to insert a \ then start or end a double quoted part (arg)</li>
 *   <li>Use \\\" to insert a literal \"</li>
 *   <li>Use \    to insert a literal \ (if it is not followed by a double-quote)</li>
 * </ul>
 *
 * <p>
 * On the other hand, re-implementing CommandLineToArgvW would be pretty hard.<br>
 * See this <a href="https://blogs.msdn.microsoft.com/oldnewthing/20100917-00/?p=12833">blog post</a>.<br>
 * Here are some empirical strange rules:
 * </p>
 *
 * <ul>
 * <li>In a non-quoted argument, double-quotes are ignored (foo"bar -> foobar)</li>
 * <li>In a non-quoted argument, double-quotes can be escaped (foo\"bar -> foo"bar)</li>
 * <li>In a non-quoted argument, several double-quotes are replaced by a single one (foo""""bar -> foo"bar)</li>
 * </ul>
 *
 * @param application_path_and_args The application path and the args that will all be converted into a command line string.
 * @param buffer Will receive the command line if it is big enough.
 * @param buffer_capacity Capacity of the buffer for the command line.
 * @param heap_buffer May receive the address of an heap buffer if the stack buffer is too small (out parameter).
 * @param command_line Resulting command line. Can point on the stack buffer or on the heap buffer (out parameter).
 */
static rt_s rt_process_argv_to_command_line(const rt_char *const *application_path_and_args, rt_char *buffer, rt_un buffer_capacity, void **heap_buffer, rt_char **command_line)
{
	const rt_char *const *in_application_path_and_args = application_path_and_args;
	rt_un command_line_buffer_capacity;
	rt_un command_line_buffer_size;
	rt_un heap_buffer_capacity = 0;
	rt_char *local_command_line;
	rt_b first;
	rt_s ret;

	/* First, compute the required size for the command line buffer. */
	command_line_buffer_capacity = 0;
	first = RT_TRUE;
	while (*in_application_path_and_args) {
		if (first) {
			first = RT_FALSE;
		} else {
			/* Space separator between arguments. */
			command_line_buffer_capacity++;
		}
		/* Add size of the argument. */
		command_line_buffer_capacity += rt_process_get_arg_size_in_command_line(*in_application_path_and_args);

		/* Switch to next argument. */
		in_application_path_and_args++;
	}
	/* Add one for terminating zero. */
	command_line_buffer_capacity++;

	/* Allocate space for the command line if needed. */
	if (!rt_static_heap_alloc_if_needed(buffer, buffer_capacity * sizeof(rt_char), heap_buffer, &heap_buffer_capacity, (void**)command_line, command_line_buffer_capacity * sizeof(rt_char)))
		goto error;

	/* Simplify pointer access. */
	local_command_line = *command_line;

	/* Generate the command line into the just allocated buffer. */
	in_application_path_and_args = application_path_and_args;
	command_line_buffer_size = 0;
	first = RT_TRUE;
	while (*in_application_path_and_args) {
		if (first) {
			first = RT_FALSE;
		} else {
			/* Arguments separator. */
			if (!rt_char_append_char(_R(' '), local_command_line, command_line_buffer_capacity, &command_line_buffer_size))
				goto error;
		}
		/* Convert argument. */
		if (!rt_process_convert_arg_to_command_line(*in_application_path_and_args, local_command_line, command_line_buffer_capacity, &command_line_buffer_size))
			goto error;

		/* Switch to next argument. */
		in_application_path_and_args++;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

#endif

#ifdef RT_DEFINE_LINUX

/**
 * Called either by the main process (if child is RT_TRUE) or by a forked intermediate process.
 *
 * <p>
 * First, calls fork to create a new process.<br>
 * Secondly, use execvp/execvpe in the forked process to replace the executed program by the requested one.
 * </p>
 *
 * <p>
 * A pipe allows the forked process to send back an errno if an error occurs before execvp/execvpe or if execvp/execvpe fails.
 * </p>
 *
 */
static rt_s rt_process_create_actual_linux(struct rt_process *process, const rt_char *current_dir, struct rt_env_vars *env_vars,
					   struct rt_io_device *std_input, struct rt_io_device *std_output, struct rt_io_device *std_error,
					   const rt_char *const *application_path_and_args)
{
	rt_n error_number;
	struct rt_pipe pipe;
	struct rt_io_device *input_io_device;
	struct rt_io_device *output_io_device;
	rt_b input_created = RT_FALSE;
	rt_b output_created = RT_FALSE;
	struct rt_input_stream *input_stream;
	struct rt_output_stream *output_stream;
	pid_t pid;
	rt_un bytes_read;
	rt_char **env_vars_array;
	rt_s ret;

	/* Not inheritable, only used by the forked process before execvp/execvpe. */
	if (!rt_pipe_create(&pipe))
		goto error;
	input_io_device = &pipe.input_io_device;
	output_io_device = &pipe.output_io_device;
	input_created = RT_TRUE;
	output_created = RT_TRUE;

	input_stream = &input_io_device->input_stream;
	output_stream = &output_io_device->output_stream;

	pid = fork();
	if (pid == 0) {
		/* We are now in the child process. */

		/* Close the reading pipe, used by the parent/intermediate. */
		/* The writing pipe will be closed either by execvp/execvpe or by the clean-up code. */
		if (!rt_io_device_free(input_io_device)) {
			rt_error_message_write_last(_R("Failed to close reading pipe: "));
			goto handle_child_error;
		}

		/* Attempt to perform redirections. */
		/* Make files descriptors 0/1/2 point on given file descriptions. */
		/* The copy of the file descriptor does not clone O_CLOEXEC. O_CLOEXEC is false in the copy. */
		/* dup2 returns -1 in case of error and set errno. */
		if (std_input) {
			if (dup2(std_input->file_descriptor, 0) == -1) {
				rt_error_message_write_last(_R("Failed to duplicate stdin for redirection: "));
				goto handle_child_error;
			}
		}
		if (std_output) {
			if (dup2(std_output->file_descriptor, 1) == -1) {
				rt_error_message_write_last(_R("Failed to duplicate stdout for redirection: "));
				goto handle_child_error;
			}
		}
		if (std_error) {
			if (dup2(std_error->file_descriptor, 2) == -1) {
				rt_error_message_write_last(_R("Failed to duplicate stderr for redirection: "));
				goto handle_child_error;
			}
		}

		/* Change current dir if provided. */
		if (current_dir) {
			/* chdir Returns zero in case of success, set errno. */
			if (chdir(current_dir)) {
				rt_console_write_error(_R("Failed to change current directory to \""));
				rt_console_write_error(current_dir);
				rt_console_write_error(_R("\": "));
				rt_error_message_write_last(RT_NULL);
				goto handle_child_error;
			}
		}

		/* Returns only if an error has occurred. The return value is -1, and errno is set to indicate the error. */
		if (env_vars) {
			if (!rt_env_vars_get_array(env_vars, &env_vars_array)) {
				rt_error_message_write_last(_R("Failed to compute environment: "));
				goto handle_child_error;
			}
			execvpe(application_path_and_args[0], (char *const*)application_path_and_args, env_vars_array);
		} else {
			execvp(application_path_and_args[0], (char *const*)application_path_and_args);
		}
		/* We are still there, that means that execvp/execvpe failed. */
		rt_console_write_error(_R("Failed to start \""));
		rt_console_write_error(application_path_and_args[0]);
		rt_console_write_error(_R("\": "));
		rt_error_message_write_last(RT_NULL);

handle_child_error:

		/* We will write errno in the pipe to be read by the parent. */
		error_number = errno;
		if (!output_stream->write(output_stream, (rt_char8*)&error_number, sizeof(error_number))) {
			rt_error_message_write_last(_R("Failed to write to parent pipe: "));
		}

		/* We do not need the writing pipe anymore so we close it. */
		if (!rt_io_device_free(output_io_device)) {
			rt_error_message_write_last(_R("Failed to close writing pipe: "));
		}

		/* Kill forked child process. */
		exit(1);
	} else if (pid > 0) {
		/* We are still in the same parent/intermediate process and the fork is ok. */

		/* We store the child process PID. */
		process->pid = pid;

		/* We close writing pipe to avoid a dead lock (as we will read from the pipe soon). It will be used only by the child. */
		output_created = RT_FALSE;
		if (!rt_io_device_free(output_io_device))
			goto error;

		/* Synchronously read errno or zero bytes (pipe closed, success!) from created child. */
		if (!input_stream->read(input_stream, (rt_char8*)&error_number, sizeof(error_number), &bytes_read))
			goto error;
		if (bytes_read) {
			/* Copy errno read from child pipe into parent/intermediate errno. */
			errno = error_number;
			goto error;
		}
	} else {
		/* On failure, fork returns -1 and set errno. */
		goto error;
	}

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

/**
 * Fork a process that will fork again to avoid zombification.
 *
 * <p>
 * If the intermediate fails to start the child, its exit code is 1 and the errno can be read from the pipe.<br>
 * Otherwise its exit code is 0 and the child PID can be read from the pipe.
 * </p>
 */
static rt_s rt_process_create_linux_using_intermediate(struct rt_process *process, const rt_char *current_dir, struct rt_env_vars *env_vars,
						       struct rt_io_device *std_input, struct rt_io_device *std_output, struct rt_io_device *std_error,
						       const rt_char *const *application_path_and_args)
{
	rt_n child_pid;
	rt_n error_number;
	struct rt_pipe pipe;
	struct rt_io_device *input_io_device;
	struct rt_io_device *output_io_device;
	rt_b input_created = RT_FALSE;
	rt_b output_created = RT_FALSE;
	struct rt_input_stream *input_stream;
	struct rt_output_stream *output_stream;
	pid_t pid;
	rt_un bytes_read;
	struct rt_process intermediate_process;
	rt_un32 intermediate_process_exit_code;
	rt_s ret;

	/* Not inheritable, only used by the forked process. */
	if (!rt_pipe_create(&pipe))
		goto error;
	input_io_device = &pipe.input_io_device;
	output_io_device = &pipe.output_io_device;
	input_created = RT_TRUE;
	output_created = RT_TRUE;

	input_stream = &input_io_device->input_stream;
	output_stream = &output_io_device->output_stream;

	pid = fork();
	if (pid == 0) {
		/* We are in the intermediate process. */

		/* Close the read pipe, used by the parent. */
		if (!rt_io_device_free(input_io_device)) {
			rt_error_message_write_last(_R("Failed to close intermediate reading pipe: "));
			goto handle_child_error;
		}

		/* Fork actual process from the current forked process. */
		if (!rt_process_create_actual_linux(process, current_dir, env_vars, std_input, std_output, std_error, application_path_and_args))
			goto handle_child_error;

		/* Write child PID into the pipe. */
		child_pid = process->pid;
		if (!output_stream->write(output_stream, (rt_char8*)&child_pid, sizeof(child_pid))) {
			rt_error_message_write_last(_R("Failed to write the child PID into the pipe: "));
			goto handle_child_error;
		}

		/* All good, kill forked intermediate process. */
		exit(0);

handle_child_error:

		/* We will write errno in the pipe to be read by the parent. */
		error_number = errno;
		if (!output_stream->write(output_stream, (rt_char8*)&error_number, sizeof(error_number))) {
			rt_error_message_write_last(_R("Failed to write to parent pipe: "));
		}

		/* We do not need the writing pipe anymore so we close it. */
		if (!rt_io_device_free(output_io_device)) {
			rt_error_message_write_last(_R("Failed to close writing pipe: "));
		}

		/* Kill forked intermediate process, notifying that something went wrong. */
		exit(1);

	} else if (pid > 0) {
		/* We are still in the same parent process and the fork is ok. */

		/* Retrieve intermediate process. */
		intermediate_process.pid = pid;

		/* We close writing pipe to avoid a dead lock (as we will read from the pipe soon). It will be used only by the intermediate. */
		output_created = RT_FALSE;
		if (!rt_io_device_free(output_io_device))
			goto error;

		/* Wait for intermediate process and retrieve its exit code. */
		if (!rt_process_join(&intermediate_process))
			goto error;
		if (!rt_process_get_exit_code(&intermediate_process, &intermediate_process_exit_code))
			goto error;

		if (intermediate_process_exit_code) {
			/* Something went wrong at some point in the intermediate process. */

			/* Synchronously read errno or zero bytes (should not happen) from created intermediate. */
			if (!input_stream->read(input_stream, (rt_char8*)&error_number, sizeof(error_number), &bytes_read))
				goto error;
			if (bytes_read) {
				/* Copy errno read from child pipe into parent errno. */
				errno = error_number;
				goto error;
			} else {
				/* We failed to retrieve errno from intermediate. Use generic error. */
				rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
				goto error;
			}

		} else {
			/* The intermediate process has done its job. */

			/* Synchronously read PID or zero bytes (should not happen) from created intermediate. */
			if (!input_stream->read(input_stream, (rt_char8*)&child_pid, sizeof(child_pid), &bytes_read))
				goto error;
			if (bytes_read) {
				/* Great, we have read the child PID from the intermediate pipe. */
				process->pid = child_pid;
			} else {
				/* We failed to retrieve child PID from intermediate. Use generic error. */
				rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
				goto error;
			}
		}
	} else {
		/* On failure, fork returns -1 and set errno. */
		goto error;
	}

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

#endif

rt_s rt_process_create_with_redirections(struct rt_process *process, RT_WINDOWS_UNUSED rt_b child, const rt_char *current_dir, struct rt_env_vars *env_vars,
					 struct rt_io_device *std_input, struct rt_io_device *std_output, struct rt_io_device *std_error,
					 const rt_char *const *application_path_and_args)
{
#ifdef RT_DEFINE_WINDOWS
	/* No need to free these devices because they will be created from true stdin/stdout/stderr. */
	struct rt_io_device std_input_device;
	struct rt_io_device std_output_device;
	struct rt_io_device std_error_device;

	struct rt_io_device *actual_std_input;
	struct rt_io_device *actual_std_output;
	struct rt_io_device *actual_std_error;

	rt_b inheritable;

	rt_b restore_input_non_inheritable = RT_FALSE;
	rt_b restore_output_non_inheritable = RT_FALSE;
	rt_b restore_error_non_inheritable = RT_FALSE;

	rt_char command_line_buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_char *command_line;
	void *heap_buffer = RT_NULL;
	STARTUPINFO startup_info;
	rt_char *env_vars_block;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS

	RT_MEMORY_ZERO(&startup_info, sizeof(startup_info));
	startup_info.cb = sizeof(startup_info);

	/* Manage redirections if needed. */
	if (std_input || std_output || std_error) {
		startup_info.dwFlags |= STARTF_USESTDHANDLES;

		/* Std input. */
		if (std_input) {
			actual_std_input = std_input;
		} else {
			 if (!rt_io_device_create_from_std_input(&std_input_device))
				 goto error;
			 actual_std_input = &std_input_device;
		}
		startup_info.hStdInput = actual_std_input->handle;

		/* Std output. */
		if (std_output) {
			actual_std_output = std_output;
		} else {
			if (!rt_io_device_create_from_std_output(&std_output_device))
				goto error;
			actual_std_output = &std_output_device;
		}
		startup_info.hStdOutput = actual_std_output->handle;

		/* Std error. */
		if (std_error) {
			actual_std_error = std_error;
		} else {
			if (!rt_io_device_create_from_std_error(&std_error_device))
				goto error;
			actual_std_error = &std_error_device;
		}
		startup_info.hStdError = actual_std_error->handle;

		/* Temporarily update input inheritability if needed. */
		if (!rt_io_device_is_inheritable(actual_std_input, &inheritable))
			goto error;
		if (!inheritable) {
			if (!rt_io_device_set_inheritable(actual_std_input, RT_TRUE))
				goto error;
			restore_input_non_inheritable = RT_TRUE;
		}

		/* Temporarily update output inheritability if needed. */
		if (!rt_io_device_is_inheritable(actual_std_output, &inheritable))
			goto error;
		if (!inheritable) {
			if (!rt_io_device_set_inheritable(actual_std_output, RT_TRUE))
				goto error;
			restore_output_non_inheritable = RT_TRUE;
		}

		/* Temporarily update error inheritability if needed. */
		if (!rt_io_device_is_inheritable(actual_std_error, &inheritable))
			goto error;
		if (!inheritable) {
			if (!rt_io_device_set_inheritable(actual_std_error, RT_TRUE))
				goto error;
			restore_error_non_inheritable = RT_TRUE;
		}
	}

	if (!rt_process_argv_to_command_line(application_path_and_args, command_line_buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &heap_buffer, &command_line))
		goto error;

	if (env_vars) {
		if (!rt_env_vars_get_block(env_vars, &env_vars_block))
			goto error;
	} else {
		env_vars_block = RT_NULL;
	}

	if (!CreateProcess(RT_NULL,		       /* lpApplicationName.								*/
			   command_line,
			   RT_NULL,		       /* LPSECURITY_ATTRIBUTES lpProcessAttributes.					*/
			   RT_NULL,		       /* LPSECURITY_ATTRIBUTES lpThreadAttributes.					*/
			   TRUE,		       /* bInheritHandles, we align inheritance with Linux.				*/
			   CREATE_UNICODE_ENVIRONMENT, /* dwCreationFlags.								*/
			   env_vars_block,	       /* lpEnvironment.								*/
			   current_dir,		       /* If NULL, the new process will have the same current directory as this one.	*/
			   &startup_info,
			   (PROCESS_INFORMATION*)process
			  ))
		goto error;

	ret = RT_OK;
free:
	if (heap_buffer) {
		if (!rt_static_heap_free(&heap_buffer) && ret)
			goto error;
	}
	if (restore_error_non_inheritable) {
		restore_error_non_inheritable = RT_FALSE;
		if (!rt_io_device_set_inheritable(actual_std_error, RT_FALSE) && ret)
			goto error;
	}
	if (restore_output_non_inheritable) {
		restore_output_non_inheritable = RT_FALSE;
		if (!rt_io_device_set_inheritable(actual_std_output, RT_FALSE) && ret)
			goto error;
	}
	if (restore_input_non_inheritable) {
		restore_input_non_inheritable = RT_FALSE;
		if (!rt_io_device_set_inheritable(actual_std_input, RT_FALSE) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;

#else

	if (child) {
		if (!rt_process_create_actual_linux(process, current_dir, env_vars, std_input, std_output, std_error, application_path_and_args))
			goto error;
	} else {
		if (!rt_process_create_linux_using_intermediate(process, current_dir, env_vars, std_input, std_output, std_error, application_path_and_args))
			goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;

#endif
}

rt_s rt_process_join(struct rt_process *process)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD returned_value;
#else
	int status;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	returned_value = WaitForSingleObject(process->process_handle, INFINITE);
	if (returned_value != WAIT_OBJECT_0) {
		/* If returned value is WAIT_FAILED, then last error is set. */
		if (returned_value != WAIT_FAILED) {
			/* Set an arbitrary last error for WAIT_TIMEOUT/WAIT_ABANDONED cases. */
			rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		}
		goto error;
	}

#else

	/* Returns -1 and set errno in case of error. */
	if (waitpid((pid_t)process->pid, &status, 0) == -1)
		goto error;

	/* Exit code can be read only if WIFEXITED is true. */
	if (WIFEXITED(status)) {
		/* Extract status from exit code. */
		process->exit_code = WEXITSTATUS(status);
	} else {
		/* Process might have been killed, use a generic exit code. */
		process->exit_code = 1;
	}
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_process_get_exit_code(struct rt_process *process, rt_un32 *exit_code)
{
#ifdef RT_DEFINE_WINDOWS
	rt_s ret;

	if (!GetExitCodeProcess(process->process_handle, exit_code))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;

#else

	*exit_code = process->exit_code;
	return RT_OK;
#endif
}

rt_s rt_process_free(RT_LINUX_UNUSED struct rt_process *process)
{
	rt_s ret;

	ret = RT_OK;

#ifdef RT_DEFINE_WINDOWS
	if (!CloseHandle(process->thread_handle))  ret = RT_FAILED;
	if (!CloseHandle(process->process_handle)) ret = RT_FAILED;
#endif

	return ret;
}

#ifndef RT_PROCESS_H
#define RT_PROCESS_H

#include "layer000/rt_types.h"
#include "layer002/rt_io_device.h"
#include "layer005/rt_env_vars.h"

/**
 * @file
 * Processes functions.
 *
 * <p>
 * After a successful call to <tt>rt_process_create</tt>, a call to <tt>rt_process_free</tt> is mandatory.<br>
 * And if <tt>child</tt> argument is <tt>RT_TRUE</tt>, you also have to call <tt>rt_process_join</tt>.
 * </p>
 *
 * <p>
 * Under Linux, there is a phenomenon called process zombification that must be avoided.<br>
 * It occurs when a parent process starts a child process but does not use <tt>waitpid</tt> to read its exit code.<br>
 * If the parent process starts keeps on running and starts numerous processes then there can be a lot of zombies.<br>
 * To workaround this issue, this library is able to spawn an intermediate process that will be the true parent of the child.<br>
 * The current process will wait for the parent end so that the parent will not become a zombie.<br>
 * And the child won't become a zombie because if the parent dies then the child become an orphan which will be adopted by init.<br>
 * And init will make sure to wait for the orphan.
 * </p>
 */

struct rt_process {
#ifdef RT_DEFINE_WINDOWS
	/* rt_process first 4 fields must be exactly the same fields as PROCESS_INFORMATION because this structure address is passed to CreateProcess function. */
	rt_h process_handle; /* HANDLE.	*/
	rt_h thread_handle;  /* HANDLE.	*/
	rt_un32 process_id;  /* DWORD.	*/
	rt_un32 thread_id;   /* DWORD.	*/
#else
	rt_n32 pid;
	rt_un32 exit_code;
#endif
};

/**
 * Start a process.
 *
 * <p>
 * If successful, ensure to call <tt>rt_process_free</tt> (And optionally <tt>rt_process_join</tt> to avoid zombies if <tt>child</tt> is <tt>RT_TRUE</tt>).
 * </p>
 *
 * <p>
 * Watch out for handles/file descriptors leaks into the child process.<br>
 * Under Linux, any file descriptor without FD_CLOEXEC/O_CLOEXEC will "leak".<br>
 * Under Windows, any handle with HANDLE_FLAG_INHERIT/bInheritHandle of SECURITY_ATTRIBUTES will "leak".<br>
 * By default, this library ensure that no resources are leaked.<br>
 * You can use <tt>rt_io_device_set_inheritable</tt> to provide a resource to a child.
 * </p>
 *
 * @param child Set to RT_TRUE if and only if you will call rt_process_join (otherwise the process will become a zombie). No effect under Windows.
 * @param current_dir Current directory of the started process. Same as current process if RT_NULL.
 * @param env_vars Can be RT_NULL.
 */
RT_API rt_s rt_process_create(struct rt_process *process, rt_b child, const rt_char *current_dir, struct rt_env_vars *env_vars, const rt_char *const *application_path_and_args);

/**
 * Spawn a child process and wait for the end of it.
 *
 * <p>
 * Under Linux, <tt>waitpid</tt> is called to wait for the child process end.
 * </p>
 */
RT_API rt_s rt_process_spawn_sync(const rt_char *current_dir, struct rt_env_vars *env_vars, const rt_char *const *application_path_and_args, rt_un32 *exit_code);

/**
 * Spawn a non-child process.
 *
 * <p>
 * Under Linux, the parent will be a temporary intermediate process.
 * </p>
 */
RT_API rt_s rt_process_spawn_async(const rt_char *current_dir, struct rt_env_vars *env_vars, const rt_char *const *application_path_and_args);

/**
 * Start a process with redirections.
 *
 * </p>
 * There is no need for you to provide inheritable file descriptors/handles as redirections.<br>
 * Under Linux, dup2 is used in the child after the fork to deal with redirection.<br>
 * dup2 "assign" given file descriptions into 0/1/2 file descriptors.<br>
 * dup2 does not copy the FD_CLOEXEC flag into the duplicated file descriptor, making it inheritable when execvp/execvpe is called after the fork.<br>
 * Under Windows, <tt>rt_io_device_set_inheritable</tt> is used to temporarily configure the handle as inheritable if it is not already.<br>
 * Be careful as this can cause a race conditions with other calls to <tt>CreateProcess</tt> from other threads.<br>
 * Be sure to use a critical section or another synchronization technique to avoid handles leaks if needed.
 * </p>
 *
 * @param std_input Can be RT_NULL.
 * @param std_output Can be RT_NULL.
 * @param std_error Can be RT_NULL.
 */
RT_API rt_s rt_process_create_with_redirections(struct rt_process *process, rt_b child, const rt_char *current_dir, struct rt_env_vars *env_vars,
					 struct rt_io_device *std_input, struct rt_io_device *std_output, struct rt_io_device *std_error,
					 const rt_char *const *application_path_and_args);

/**
 * Can be used only if <tt>child</tt> was <tt>RT_TRUE</tt> while creating the process.
 *
 * <p>
 * Under Linux, must be called to read the exit code and as a result kill a zombified process.<br>
 * The exit code is saved into the <tt>rt_process</tt> structure so that it can be read with <tt>rt_process_get_exit_code</tt> later on.
 * </p>
 */
RT_API rt_s rt_process_join(struct rt_process *process);

/**
 * Return the exit code number of the child process.
 *
 * <p>
 * Make sure that the child process is terminated by using <tt>rt_process_join</tt> before calling this function.
 * </p>
 *
 * <p>
 * Under Linux, exit code is 8 unsigned bits, 0 -> 255.<br>
 * Under Windows, exit code is a DWORD (32 unsigned bits) although the command line interpreter treats them as signed.<br>
 * </p>
 */
RT_API rt_s rt_process_get_exit_code(struct rt_process *process, rt_un32 *exit_code);

/**
 * Must always be called after a successful call to <tt>rt_process_create</tt>.
 *
 * <p>
 * Under Windows, close the process handles.<br>
 * Does nothing under Linux.
 * </p>
 */
RT_API rt_s rt_process_free(struct rt_process *process);

#endif /* RT_PROCESS_H */

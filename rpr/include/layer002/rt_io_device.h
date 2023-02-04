#ifndef RT_IO_DEVICE_H
#define RT_IO_DEVICE_H

#include "layer000/rt_types.h"
#include "layer001/rt_input_stream.h"
#include "layer001/rt_output_stream.h"

/**
 * @file
 * Device used for I/O: a file, a socket, a pipe...
 *
 * <p>
 * A device may support input only, output only or may support both input and output.
 * <p/>
 */

struct rt_io_device {
	struct rt_input_stream input_stream;
	struct rt_output_stream output_stream;
#ifdef RT_DEFINE_WINDOWS
	rt_h handle;
#else
	rt_n32 file_descriptor;
#endif

};

/**
 * In most cases, you should not need these functions.<br>
 * Create the correct I/O device instead using <tt>rt_file_create</tt>, <tt>rt_pipe_create</tt> and so on.
 */
#ifdef RT_DEFINE_WINDOWS
void rt_io_device_create_from_handle(struct rt_io_device *io_device, rt_h handle);
#else
void rt_io_device_create_from_file_descriptor(struct rt_io_device *io_device, rt_n32 file_descriptor);
#endif

rt_s rt_io_device_create_from_std_input(struct rt_io_device *io_device);
rt_s rt_io_device_create_from_std_output(struct rt_io_device *io_device);
rt_s rt_io_device_create_from_std_error(struct rt_io_device *io_device);

/**
 * In most cases, you should not need this function.<br>
 * Use <tt>input_stream->read</tt> instead.
 */
rt_s rt_io_device_read(struct rt_input_stream *input_stream, rt_char8 *buffer, rt_un bytes_to_read, rt_un *bytes_read);

/**
 * In most cases, you should not need this function.<br>
 * Use <tt>output_stream->write</tt> instead.
 */
rt_s rt_io_device_write(struct rt_output_stream *output_stream, const rt_char8 *data, rt_un bytes_to_write);

/**
 * Does nothing.
 */
rt_s rt_io_device_flush(struct rt_output_stream *output_stream);

/**
 * <p>
 * Under Windows, an inheritable handle is valid in a child process created with CreateProcess and bInheritHandles TRUE.
 * </p>
 *
 * <p>
 * Under Linux, a non-inheritable file descriptor is closed when execl/execlp/execle/execv/execvp/execvpe is used.<br>
 * It is not closed by a fork.
 * </p>
 */
rt_s rt_io_device_is_inheritable(struct rt_io_device *io_device, rt_b *inheritable);

/**
 * Update inheritability of given I/O Device if needed.
 *
 * <p>
 * Beware that there can be a race condition with this function.<br>
 * If another thread is performing fork/exec or CreateProcess, then there can be a file descriptor/handle leak.<br>
 * See O_CLOEXEC flag description in open manual.
 * </p>
 *
 * <p>
 * Under Windows, an inheritable handle can be used by a process created with CreateProcess and bInheritHandles TRUE.
 * </p>
 *
 * <p>
 * Under Linux, a non-inheritable file descriptor is closed when execl/execlp/execle/execv/execvp/execvpe is used.<br>
 * It is not closed by a fork.
 * </p>
 */
rt_s rt_io_device_set_inheritable(struct rt_io_device *io_device, rt_b inheritable);

/**
 * Should be called only with generic devices like files and pipes, not sockets.<br>
 * Should not be called with std Input/Output/Error.
 */
rt_s rt_io_device_free(struct rt_io_device *io_device);

#endif /* RT_IO_DEVICE_H */

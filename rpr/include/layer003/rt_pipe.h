#ifndef RT_PIPE_H
#define RT_PIPE_H

#include "layer000/rt_types.h"
#include "layer002/rt_io_device.h"

/**
 * @file
 * A pipe is a memory area in which you can write and from which you can read.<br>
 * For example it can be used for inter-process communication:<br>
 * A process can write to the pipe and another process can read from the pipe.<br>
 * A process can share a pipe with another process through inheritability.
 */

struct rt_pipe {
	struct rt_io_device input_io_device;
	struct rt_io_device output_io_device;
};

/**
 * Do not forget to close input and output I/O devices when they are no more needed.<br>
 * You can either use <tt>rt_pipe_free</tt> or 2 two calls of <tt>rt_io_device_free</tt>.
 */
rt_s rt_pipe_create(struct rt_pipe *pipe);

/**
 * Free both input and output I/O devices of the pipe.<br>
 * It is not mandatory to call this function.<br>
 * Input and output I/O devices can be closes separately.
 */
rt_s rt_pipe_free(struct rt_pipe *pipe);

#endif /* RT_PIPE_H */

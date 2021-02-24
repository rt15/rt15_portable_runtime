#ifndef RT_ERROR_H
#define RT_ERROR_H

#include "layer000/rt_types.h"

/**
 * @file
 * Manage operating system error codes in an unified way for Windows and Linux.
 *
 * <p>
 * The purpose is to be able to set last error code whatever the underlying OS is.<br>
 * As a result this header could be used by any C file.
 * </p>
 */

/* The operation completed successfully. */
#define RT_ERROR_SUCCESS 0
/* The data area passed to a system call is too small. */
#define RT_ERROR_INSUFFICIENT_BUFFER 1
/* One or more arguments are not correct. */
#define RT_ERROR_BAD_ARGUMENTS 2
/* Not enough memory or memory issue, but not buffer related. */
#define RT_ERROR_NOT_ENOUGH_MEMORY 3
/* The "thing" already exists. */
#define RT_ERROR_ALREADY_EXISTS 4
/* A function has failed. Avoid this generic error code if possible. */
#define RT_ERROR_FUNCTION_FAILED 5
/* A numerical type cannot contain given value. */
#define RT_ERROR_ARITHMETIC_OVERFLOW 6
/* The function call would have blocked if the descriptor was a blocking one. See also RT_ERROR_SOCKET_WOULD_BLOCK. */
#define RT_ERROR_WOULD_BLOCK 7
/* The function call would have blocked if the socket was a blocking one. See also RT_ERROR_WOULD_BLOCK. */
#define RT_ERROR_SOCKET_WOULD_BLOCK 8

/**
 * Can be used to set the operating system last error code.
 *
 * The given <tt>error</tt> will be mapped to an underlying operating system error code.
 *
 * @param error The rpr error code (RT_ERROR_XXXXX). If not known, RT_ERROR_BAD_ARGUMENTS is used.
 */
void rt_error_set_last(rt_un error);

/**
 * Returns <tt>RT_TRUE</tt> if the last error is a "would block" error.
 *
 * <p>
 * Can be used after an error on a call with a non-blocking descriptor.<br>
 * When we call a function that would block if the given descriptor was blocking, a normal "would block" error is triggered.<br>
 * This error should not be considered as a failure.
 * </p>
 *
 * <p>
 * On Linux, <tt>connect</tt> uses <tt>EINPROGRESS</tt>.<br>
 * <tt>recv</tt> and <tt>write</tt> uses <tt>EAGAIN</tt> or <tt>EWOULDBLOCK</tt>.<br>
 * The library normalizes everything to <tt>EWOULDBLOCK</tt>.<br>
 * This function returns RT_TRUE only if errno is <tt>EWOULDBLOCK</tt>.
 * </p>
 *
 * <p>
 * On Windows, socket functions like <tt>connect</tt> uses <tt>WSAEWOULDBLOCK</tt>.<br>
 * <tt>WriteFile</tt> uses <tt>ERROR_IO_PENDING</tt>.<br>
 * The library keep either <tt>WSAEWOULDBLOCK</tt> or <tt>ERROR_IO_PENDING</tt> (Because the error messages are interesting).<br>
 * This function returns RT_TRUE if <tt>GetLastError</tt> is <tt>WSAEWOULDBLOCK</tt> or <tt>ERROR_IO_PENDING</tt>.
 * </p>
 */
rt_b rt_error_would_block();

#endif /* RT_ERROR_H */

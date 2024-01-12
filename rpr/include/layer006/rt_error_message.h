#ifndef RT_ERROR_MESSAGE_H
#define RT_ERROR_MESSAGE_H

#include "layer000/rt_types.h"

/**
 * @file
 * Manage operating system error messages in an unified way for Windows and Linux.
 */

/**
 * Append the last error message.
 *
 * <p>
 * Both Windows and Linux provide a function to get last error code and a function to convert it to a string.
 * </p>
 *
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 * @return RT_FAILED in case of error.
 */
RT_API rt_s rt_error_message_append_last(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * Write last error message into console standard error stream.
 *
 * <p>
 * The optional <tt>prefix</tt> is directly concatenated before the operating system error.<br>
 * Prefix sample: <tt>_R("Failed to open data.txt: ")</tt><br>
 * Can be RT_NULL.
 * </p>
 */
RT_API rt_s rt_error_message_write_last(const rt_char *prefix);

#endif /* RT_ERROR_MESSAGE_H */

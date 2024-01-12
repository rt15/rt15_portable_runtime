
#ifndef RT_LAST_ERROR_MESSAGE_H
#define RT_LAST_ERROR_MESSAGE_H

#include "layer000/rt_types.h"

/**
 * @file
 * Allow to store/retrieve error messages from a thread local storage.
 * 
 * <p>
 * It is not used by rpr internally but can be used to manage error messages in the API consumers.
 * </p>
 * 
 * <p>
 * Beware that the functions are using static variables so the messages are not shared accross multiple instances of the library.
 * </p>
 */

/**
 * Put <tt>message</tt> as last error message of this thread.
 */
RT_API rt_s rt_last_error_message_set(const rt_char *message);

/**
 * Update the last error message of this thread with a message from the last operating system error. 
 */
RT_API rt_s rt_last_error_message_set_with_last_error();

/**
 * Append the last error message of this thread in <tt>buffer</tt>. 
 *
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
RT_API rt_s rt_last_error_message_append(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * Write the last error message of this thread into console standard error stream.
 *
 * <p>
 * The optional <tt>prefix</tt> is directly concatenated before last error message.<br>
 * Prefix sample: <tt>_R("Failed to open data.txt: ")</tt><br>
 * Can be RT_NULL.
 * </p>
 */
RT_API rt_s rt_last_error_message_write(const rt_char *prefix);

/**
 * Clean the memory buffer for this thread.
 * 
 * <p>
 * Each thread has its own buffer.<br>
 * So if you do not use this function, there will be a memory leak at the end of each thread.
 * </p>
 */
RT_API rt_s rt_last_error_message_cleanup_thread_buffer();

/**
 * After cleaning all the thread buffers, you can call this function to free the local storage.
 * 
 * <p>
 * It can be called only once.
 * </p>
 */
RT_API rt_s rt_last_error_message_cleanup();

#endif /* RT_LAST_ERROR_MESSAGE_H */

#ifndef RT_CONSOLE_H
#define RT_CONSOLE_H

#include "layer000/rt_types.h"

rt_s rt_console_write_string(const rt_char *str);
rt_s rt_console_write_string_with_size(const rt_char *str, rt_un size);

rt_s rt_console_write_error(const rt_char *error);
rt_s rt_console_write_error_with_size(const rt_char *error, rt_un size);

rt_s rt_console_write(const rt_char *message, rt_b error);
rt_s rt_console_write_with_size(const rt_char *message, rt_un size, rt_b error);

/**
 * Read a line from the console (the standard input).
 *
 * <p>
 * The implementation of this function might be quite slow but it should not be an issue as the user should be slower.
 * </p>
 *
 * @param buffer_capacity Must be at least 2.
 * @param buffer_size Out parameter.
 */
rt_s rt_console_read_line(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

#endif /* RT_CONSOLE_H */

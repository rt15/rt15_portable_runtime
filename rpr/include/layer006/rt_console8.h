#ifndef RT_CONSOLE8_H
#define RT_CONSOLE8_H

#include "layer000/rt_types.h"
#include "layer004/rt_encoding.h"

RT_API rt_s rt_console8_write_str(const rt_char8 *str, enum rt_encoding encoding);
RT_API rt_s rt_console8_write_str_with_size(const rt_char8 *str, enum rt_encoding encoding, rt_un size);

RT_API rt_s rt_console8_write_error(const rt_char8 *error, enum rt_encoding encoding);
RT_API rt_s rt_console8_write_error_with_size(const rt_char8 *error, enum rt_encoding encoding, rt_un size);

RT_API rt_s rt_console8_write(const rt_char8 *message, enum rt_encoding encoding, rt_b error);
RT_API rt_s rt_console8_write_with_size(const rt_char8 *message, enum rt_encoding encoding, rt_un size, rt_b error);

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
RT_API rt_s rt_console8_read_line(enum rt_encoding encoding, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size);

#endif /* RT_CONSOLE8_H */

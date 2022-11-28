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

/**
 * Wait for the user to press a key.
 *
 * <p>
 * Beware that if the user press a key like 'é' and the console is in UTF-8, then two characters are actually entered: 0xC3 and 0xA9.<br>
 * The rt_console_read_char will only return 0xC3 (195) and the other character will be discarded.<br>
 * As a result this function is interesting only to pause the execution and to retrieve ASCII characters.
 * </p>
 *
 * @return The "first" entered character.
 */
rt_s rt_console_read_char(rt_char *character);

#endif /* RT_CONSOLE_H */

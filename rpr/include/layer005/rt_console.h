#ifndef RT_CONSOLE_H
#define RT_CONSOLE_H

#include "layer000/rt_types.h"


enum rt_console_color {
	RT_CONSOLE_COLOR_BLACK = 0,
	RT_CONSOLE_COLOR_BLUE = 0x1,
	RT_CONSOLE_COLOR_GREEN = 0x2,
	RT_CONSOLE_COLOR_CYAN = 0x1 | 0x2, /* Dark cyan. You might prefer BRIGHT_CYAN. */
	RT_CONSOLE_COLOR_RED = 0x4,
	RT_CONSOLE_COLOR_MAGENTA = 0x1 | 0x4,
	RT_CONSOLE_COLOR_YELLOW = 0x2 | 0x4, /* More like orange. Prefer BRIGHT_YELLOW for yellow. */
	RT_CONSOLE_COLOR_WHITE = 0x1 | 0x2 | 0x4, /* Very light gray, default color under Windows. */
	RT_CONSOLE_COLOR_BRIGHT_BLACK = 0x8, /* Gray. */
	RT_CONSOLE_COLOR_BRIGHT_BLUE = 0x1 | 0x8,
	RT_CONSOLE_COLOR_BRIGHT_GREEN = 0x2 | 0x8, /* Lemon green. */
	RT_CONSOLE_COLOR_BRIGHT_CYAN = 0x1 | 0x2 | 0x8,
	RT_CONSOLE_COLOR_BRIGHT_RED = 0x4 | 0x8, /* Almost orange. */
	RT_CONSOLE_COLOR_BRIGHT_MAGENTA = 0x1 | 0x4 | 0x8, /* Almost pink. */
	RT_CONSOLE_COLOR_BRIGHT_YELLOW = 0x2 | 0x4 | 0x8, /* Yellow. */
	RT_CONSOLE_COLOR_BRIGHT_WHITE = 0x1 | 0x2 | 0x4 | 0x8 /* True white. */
};

RT_API rt_s rt_console_write_str(const rt_char *str);
RT_API rt_s rt_console_write_str_with_size(const rt_char *str, rt_un size);

RT_API rt_s rt_console_write_error(const rt_char *error);
RT_API rt_s rt_console_write_error_with_size(const rt_char *error, rt_un size);

RT_API rt_s rt_console_write(const rt_char *message, rt_b error);
RT_API rt_s rt_console_write_with_size(const rt_char *message, rt_un size, rt_b error);

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
RT_API rt_s rt_console_read_line(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

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
RT_API rt_s rt_console_read_char(rt_char *character);

/**
 * Clear the console screen.
 */
RT_API rt_s rt_console_clear(void);

/**
 * Set console foreground color.
 */
RT_API rt_s rt_console_set_color(enum rt_console_color console_color);

/**
 * Reset console foreground color to the default one.
 */
RT_API rt_s rt_console_reset_color(void);

#endif /* RT_CONSOLE_H */

#ifndef RT_CONSOLE_H
#define RT_CONSOLE_H

#include "layer000/rt_types.h"

rt_s rt_console_write_string(const rt_char *string);
rt_s rt_console_write_string_with_size(const rt_char *string, rt_un size);

rt_s rt_console_write_error(const rt_char *error);
rt_s rt_console_write_error_with_size(const rt_char *error, rt_un size);

rt_s rt_console_write(const rt_char *message, rt_b error);
rt_s rt_console_write_with_size(const rt_char *message, rt_un size, rt_b error);

#endif /* RT_CONSOLE_H */

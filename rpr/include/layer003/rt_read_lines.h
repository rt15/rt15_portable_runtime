#ifndef RT_READ_LINES_H
#define RT_READ_LINES_H

#include "layer000/rt_types.h"
#include "layer001/rt_eol.h"
#include "layer001/rt_input_stream.h"

/**
 * @file
 * Allow to easily read lines from an input stream.
 */

typedef rt_s (*rt_read_lines_callback_t)(const rt_char8 *line, rt_un line_size, enum rt_eol eol, void *context);

RT_API rt_s rt_read_lines(struct rt_input_stream *input_stream, rt_char8 *buffer, rt_un buffer_capacity, rt_read_lines_callback_t callback, void *context);

#endif /* RT_READ_LINES_H */

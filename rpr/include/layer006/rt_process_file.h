#ifndef RT_PROCESS_FILE_H
#define RT_PROCESS_FILE_H

#include "layer000/rt_types.h"

#include "layer001/rt_output_stream.h"
#include "layer003/rt_read_lines.h"

/**
 * @file
 * Allow to perform modifications of the lines of a file, or to generate a new file from the lines of a file.
 */

struct rt_process_file_context {
	struct rt_output_stream *output_stream;
	void *context;
};

typedef rt_s (*rt_process_file_callback_t)(const rt_char8 *line, rt_un line_size, enum rt_eol eol, struct rt_process_file_context *process_file_context);

RT_API rt_s rt_process_file(const rt_char *file_path, rt_process_file_callback_t callback, void *context);

RT_API rt_s rt_process_file_with_output_file(const rt_char *input_file_path, const rt_char *output_file_path, rt_process_file_callback_t callback, void *context);

RT_API rt_s rt_process_file_write_eol(enum rt_eol eol, struct rt_output_stream *output_stream);

#endif /* RT_PROCESS_FILE_H */

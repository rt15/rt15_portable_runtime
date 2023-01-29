#include "layer003/rt_read_lines.h"

#include "layer001/rt_memory.h"
#include "layer002/rt_error.h"

rt_s rt_read_lines(struct rt_input_stream *input_stream, rt_char8 *buffer, rt_un buffer_capacity, rt_read_lines_callback_t callback, void *context)
{
	rt_un bytes_parsed = 0;
	rt_un bytes_read;
	rt_un buffer_size;
	rt_un line_beginning;
	enum rt_eol eol;
	rt_un i;
	rt_s ret;

	while (RT_TRUE) {
		if (!input_stream->read(input_stream, &buffer[bytes_parsed], buffer_capacity - bytes_parsed, &bytes_read))
			goto error;

		/* End of stream? */
		if (!bytes_read) {
			/* If any, send the line that was at the beginning of the buffer. */
			if (bytes_parsed) {
				if (!callback(buffer, bytes_parsed, RT_EOL_NONE, context))
					goto error;
			}
			break;
		}

		/* Considering the beginning of the buffer as read. */
		buffer_size = bytes_parsed + bytes_read;

continue_reading:
		line_beginning = 0;
		for (i = bytes_parsed; i < buffer_size; i++) {
			if (buffer[i] == '\n' || buffer[i] == '\r') {
				if (buffer[i] == '\r') {
					if (i + 1 >= buffer_size) {
						/* The next character might be LF, but we cannot find out yet because it is not in the buffer. */

						/* Copy the current line at the beginning of the buffer. */
						bytes_parsed = buffer_size - line_beginning;
						/* bytes_parsed is at least 1 because we just found a CR character. */
						RT_MEMORY_COPY(&buffer[line_beginning], buffer, bytes_parsed);

						/* Attempt to read the potential LF from the real stream. */
						if (!input_stream->read(input_stream, &buffer[bytes_parsed], buffer_capacity - bytes_parsed, &bytes_read))
							goto error;

						/* Nothing more to read, so we assume no LF. */
						if (!bytes_read) {
							line_beginning = 0;
							buffer_size = bytes_parsed;
							i = bytes_parsed - 1;
							eol = RT_EOL_CR;
						} else {
							buffer_size = bytes_parsed + bytes_read;
							/* We will re-parse the CR. */
							bytes_parsed--;
							/* We just read more characters so it is not an infinite loop. */
							goto continue_reading;
						}
					}else if (i + 1 < buffer_size && buffer[i + 1] == '\n') {
						eol = RT_EOL_CRLF;
					} else {
						eol = RT_EOL_CR;
					}
				} else {
					eol = RT_EOL_LF;
				}

				if (!callback(&buffer[line_beginning], i - line_beginning, eol, context))
					goto error;
				if (eol == RT_EOL_CRLF) {
					/* Skip he LF. */
					i++;
				}

				line_beginning = i + 1;
			}
		}

		bytes_parsed = buffer_size - line_beginning;
		if (bytes_parsed == buffer_capacity) {
			/* The buffer is full yet no end of line. The buffer is too small. */
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}

		/* Copy the beginning of the current line at the beginning of the buffer. */
		if (bytes_parsed)
			RT_MEMORY_COPY(&buffer[line_beginning], buffer, bytes_parsed);
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

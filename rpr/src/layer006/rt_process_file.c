#include "layer006/rt_process_file.h"

#include "layer002/rt_error.h"
#include "layer003/rt_char.h"
#include "layer003/rt_char8.h"
#include "layer003/rt_file.h"
#include "layer003/rt_read_lines.h"
#include "layer004/rt_file_path.h"
#include "layer005/rt_file_system.h"

struct rt_process_file_internal_context {
	struct rt_process_file_context process_file_context;
	rt_process_file_callback_t callback;
};

static rt_s rt_process_file_with_streams_callback(const rt_char8 *line, rt_un line_size, enum rt_eol eol, void *context)
{
	struct rt_process_file_internal_context *process_file_internal_context = (struct rt_process_file_internal_context*)context;
	struct rt_process_file_context *process_file_context = &process_file_internal_context->process_file_context;

	return process_file_internal_context->callback(line, line_size, eol, process_file_context);
}

static rt_s rt_process_file_with_streams(struct rt_input_stream *input_stream, struct rt_output_stream *output_stream, rt_process_file_callback_t callback, void *context)
{
	rt_char8 buffer[RT_CHAR8_BIG_STRING_SIZE];
	struct rt_process_file_internal_context internal_context;
	rt_s ret = RT_FAILED;

	internal_context.process_file_context.output_stream = output_stream;
	internal_context.process_file_context.context = context;
	internal_context.callback = callback;

	if (RT_UNLIKELY(!rt_read_lines(input_stream, buffer, RT_CHAR8_BIG_STRING_SIZE, &rt_process_file_with_streams_callback, &internal_context)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_process_file_with_output_file(const rt_char *input_file_path, const rt_char *output_file_path, rt_process_file_callback_t callback, void *context)
{
	struct rt_file input_file;
	rt_b input_file_created = RT_FALSE;
	struct rt_file output_file;
	rt_b output_file_created = RT_FALSE;
	struct rt_input_stream *input_stream;
	struct rt_output_stream *output_stream;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_file_create(&input_file, input_file_path, RT_FILE_MODE_READ)))
		goto end;
	input_file_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_file_create(&output_file, output_file_path, RT_FILE_MODE_TRUNCATE)))
		goto end;
	output_file_created = RT_TRUE;

	input_stream = &input_file.io_device.input_stream;
	output_stream = &output_file.io_device.output_stream;

	if (RT_UNLIKELY(!rt_process_file_with_streams(input_stream, output_stream, callback, context)))
		goto end;

	ret = RT_OK;
end:
	if (output_file_created) {
		if (RT_UNLIKELY(!rt_io_device_free(&output_file.io_device)))
			ret = RT_FAILED;
	}
	if (input_file_created) {
		if (RT_UNLIKELY(!rt_io_device_free(&input_file.io_device)))
			ret = RT_FAILED;
	}

	return ret;
}

rt_s rt_process_file(const rt_char *file_path, rt_process_file_callback_t callback, void *context)
{
	rt_char tmp_file_path[RT_FILE_PATH_SIZE];
	rt_un tmp_file_path_size = rt_char_get_size(file_path);
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_char_copy(file_path, tmp_file_path_size, tmp_file_path, RT_FILE_PATH_SIZE))) goto end;
	if (RT_UNLIKELY(!rt_char_append(_R(".tmp"), 4, tmp_file_path, RT_FILE_PATH_SIZE, &tmp_file_path_size))) goto end;

	if (RT_UNLIKELY(!rt_process_file_with_output_file(file_path, tmp_file_path, callback, context))) goto end;

	if (RT_UNLIKELY(!rt_file_system_delete_file(file_path))) goto end;
	if (RT_UNLIKELY(!rt_file_system_move_file(tmp_file_path, file_path))) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_process_file_write_eol(enum rt_eol eol, struct rt_output_stream *output_stream)
{
	rt_char8 *end_of_line;
	rt_un end_of_line_size;
	rt_s ret = RT_FAILED;

	switch (eol) {
	case RT_EOL_NONE:
		end_of_line = RT_NULL;
		end_of_line_size = 0;
		break;
	case RT_EOL_LF:
		end_of_line = "\n";
		end_of_line_size = 1;
		break;
	case RT_EOL_CRLF:
		end_of_line = "\r\n";
		end_of_line_size = 2;
		break;
	case RT_EOL_CR:
		end_of_line = "\r";
		end_of_line_size = 1;
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	if (end_of_line) {
		if (RT_UNLIKELY(!output_stream->write(output_stream, end_of_line, end_of_line_size)))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

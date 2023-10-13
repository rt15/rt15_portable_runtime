#include <rpr.h>

#include "zz_utils.h"

static rt_char8 *zz_line = "abcdefghijklmnopqrstuvwxyz\n";

static rt_s zz_test_buffered_input_stream_prepare_file(const rt_char* file_path)
{
	struct rt_file file;
	rt_b file_created = RT_FALSE;
	struct rt_output_stream *output_stream;
	rt_un zz_line_size = rt_char8_get_size(zz_line);
	rt_un i;
	rt_s ret;

	if (RT_UNLIKELY(!rt_file_create(&file, file_path, RT_FILE_MODE_TRUNCATE))) goto error;
	file_created = RT_TRUE;

	output_stream = &file.io_device.output_stream;

	for (i = 0; i < 999; i++) {
		if (RT_UNLIKELY(!output_stream->write(output_stream, zz_line, zz_line_size)))
			goto error;
	}
	if (RT_UNLIKELY(!output_stream->write(output_stream, zz_line, 13)))
		goto error;

	ret = RT_OK;
free:
	if (file_created) {
		file_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_io_device_free(&file.io_device) && ret))
			goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_buffered_input_stream_do(const rt_char* file_path)
{
	struct rt_file file;
	rt_b file_created = RT_FALSE;
	struct rt_buffered_input_stream buffered_input_stream;
	rt_char8 stream_buffer[80];
	struct rt_input_stream *input_stream;
	rt_char8 buffer[40];
	rt_un bytes_read;
	rt_un zz_line_size = rt_char8_get_size(zz_line);
	rt_un i;
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_buffered_input_stream_prepare_file(file_path))) goto error;

	if (RT_UNLIKELY(!rt_file_create(&file, file_path, RT_FILE_MODE_READ))) goto error;
	file_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_buffered_input_stream_create(&buffered_input_stream, &file.io_device.input_stream, stream_buffer, 80))) goto error;
	input_stream = &buffered_input_stream.input_stream;

	for (i = 0; i < 999; i++) {
		RT_MEMORY_SET(buffer, '1', 40);
		if (RT_UNLIKELY(!input_stream->read(input_stream, buffer, 27, &bytes_read)))
			goto error;
		if (RT_UNLIKELY(bytes_read != 27))
			goto error;
		if (RT_UNLIKELY(!rt_char8_equals(buffer, bytes_read, zz_line, zz_line_size)))
			goto error;
	}

	/* Read the last cut off line. */
	RT_MEMORY_SET(buffer, '1', 40);
	if (RT_UNLIKELY(!input_stream->read(input_stream, buffer, 15, &bytes_read)))
		goto error;
	if (RT_UNLIKELY(bytes_read != 13))
		goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, bytes_read, zz_line, 13)))
		goto error;

	ret = RT_OK;
free:
	if (file_created) {
		file_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_io_device_free(&file.io_device) && ret))
			goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_buffered_input_stream()
{
	rt_char file_path[RT_FILE_PATH_SIZE];
	rt_un file_path_size;
	rt_s ret;

	if (RT_UNLIKELY(!zz_get_tmp_dir(file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("buffered_stream.txt"), 19, file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;

	if (RT_UNLIKELY(!zz_test_buffered_input_stream_do(file_path))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

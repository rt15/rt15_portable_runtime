#include <rpr.h>

#include "zz_utils.h"

static rt_s zz_generate_char8_replace(rt_char8 *buffer, rt_un *buffer_size)
{
	rt_char8 temp_buffer[RT_CHAR8_HALF_BIG_STRING_SIZE];
	rt_un temp_buffer_size;
	rt_s ret;

	temp_buffer_size = 0;
	if (RT_UNLIKELY(!rt_char8_replace(buffer, *buffer_size, "rt_char", 7, "rt_char8", 8, temp_buffer, RT_CHAR8_HALF_BIG_STRING_SIZE, &temp_buffer_size)))
		goto error;

	*buffer_size = 0;
	if (RT_UNLIKELY(!rt_char8_replace(temp_buffer, temp_buffer_size, "rt_uchar ", 9, "rt_uchar8 ", 10, buffer, RT_CHAR8_HALF_BIG_STRING_SIZE, buffer_size)))
		goto error;

	temp_buffer_size = 0;
	if (RT_UNLIKELY(!rt_char8_replace(buffer, *buffer_size, "rt_uchar*", 9, "rt_uchar8*", 10, temp_buffer, RT_CHAR8_HALF_BIG_STRING_SIZE, &temp_buffer_size)))
		goto error;

	*buffer_size = 0;
	if (RT_UNLIKELY(!rt_char8_replace(temp_buffer, temp_buffer_size, "RT_CHAR", 7, "RT_CHAR8", 8, buffer, RT_CHAR8_HALF_BIG_STRING_SIZE, buffer_size)))
		goto error;

	temp_buffer_size = 0;
	if (RT_UNLIKELY(!rt_char8_replace(buffer, *buffer_size, "test_char", 9, "test_char8", 10, temp_buffer, RT_CHAR8_HALF_BIG_STRING_SIZE, &temp_buffer_size)))
		goto error;

	*buffer_size = 0;
	if (RT_UNLIKELY(!rt_char8_replace(temp_buffer, temp_buffer_size, "TEST_CHAR", 9, "TEST_CHAR8", 10, buffer, RT_CHAR8_HALF_BIG_STRING_SIZE, buffer_size)))
		goto error;

	temp_buffer_size = 0;
	if (RT_UNLIKELY(!rt_char8_replace(buffer, *buffer_size, "ZZ_TEST_CHAR8_ACCENTED_CHARACTERS_CASE 1", 40, "ZZ_TEST_CHAR8_ACCENTED_CHARACTERS_CASE 0", 40, temp_buffer, RT_CHAR8_HALF_BIG_STRING_SIZE, &temp_buffer_size)))
		goto error;

	*buffer_size = 0;
	if (RT_UNLIKELY(!rt_char8_replace(temp_buffer, temp_buffer_size, "RT_MEMORY_SET_CHAR", 18, "RT_MEMORY_SET", 13, buffer, RT_CHAR8_HALF_BIG_STRING_SIZE, buffer_size)))
		goto error;

	temp_buffer_size = 0;
	if (RT_UNLIKELY(!rt_char8_replace(buffer, *buffer_size, "RT_CHAR8_BIG_STRING_SIZE 1920", 29, "RT_CHAR8_BIG_STRING_SIZE 3840", 29, temp_buffer, RT_CHAR8_HALF_BIG_STRING_SIZE, &temp_buffer_size)))
		goto error;

	*buffer_size = 0;
	if (RT_UNLIKELY(!rt_char8_replace(temp_buffer, temp_buffer_size, "RT_CHAR8_HALF_BIG_STRING_SIZE 960", 33, "RT_CHAR8_HALF_BIG_STRING_SIZE 1920", 34, buffer, RT_CHAR8_HALF_BIG_STRING_SIZE, buffer_size)))
		goto error;

	temp_buffer_size = 0;
	if (RT_UNLIKELY(!rt_char8_replace(buffer, *buffer_size, "RT_CHAR8_THIRD_BIG_STRING_SIZE 640", 34, "RT_CHAR8_THIRD_BIG_STRING_SIZE 1280", 35, temp_buffer, RT_CHAR8_HALF_BIG_STRING_SIZE, &temp_buffer_size)))
		goto error;

	*buffer_size = 0;
	if (RT_UNLIKELY(!rt_char8_replace(temp_buffer, temp_buffer_size, "RT_CHAR8_QUARTER_BIG_STRING_SIZE 480", 36, "RT_CHAR8_QUARTER_BIG_STRING_SIZE 960", 36, buffer, RT_CHAR8_HALF_BIG_STRING_SIZE, buffer_size)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_generate_char8_remove_macros(rt_char8 *buffer, rt_un *buffer_size)
{
	rt_un index;
	rt_char quote;
	rt_un end_index;
	rt_s ret;

	while (RT_TRUE) {
		index = rt_char8_search(buffer, "_R(");
		if (index == RT_TYPE_MAX_UN)
			break;

		quote = buffer[index + 3];
		end_index = index + 4;
		while (RT_TRUE) {
			if (buffer[end_index] == quote) {
				end_index++;
				break;
			}
			end_index++;
		}


		if (RT_UNLIKELY(!rt_char8_copy(&buffer[index + 3], *buffer_size - (index + 3), &buffer[index], RT_CHAR8_HALF_BIG_STRING_SIZE - index)))
			goto error;
		*buffer_size -= 3;
		end_index -= 3;

		if (RT_UNLIKELY(!rt_char8_copy(&buffer[end_index + 1], *buffer_size - (end_index + 1), &buffer[end_index], RT_CHAR8_HALF_BIG_STRING_SIZE - end_index)))
			goto error;
		*buffer_size -= 1;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_generate_char8_callback(const rt_char8 *line, rt_un line_size, enum rt_eol eol, void *context)
{
	struct rt_output_stream *output_stream;
	rt_char8 buffer[RT_CHAR8_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret;

	output_stream = (struct rt_output_stream*)context;

	buffer_size = line_size;

	if (RT_UNLIKELY(!rt_char8_copy(line, line_size, buffer, RT_CHAR8_HALF_BIG_STRING_SIZE))) goto error;
	if (RT_UNLIKELY(!zz_generate_char8_replace(buffer, &buffer_size))) goto error;
	if (RT_UNLIKELY(!zz_generate_char8_remove_macros(buffer, &buffer_size))) goto error;
	if (RT_UNLIKELY(!output_stream->write(output_stream, buffer, buffer_size))) goto error;

	if (eol != RT_EOL_NONE) {
		if (RT_UNLIKELY(!output_stream->write(output_stream, "\n", 1)))
			goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_generate_char8_process(const rt_char *file_path, const rt_char *file_path8)
{
	struct rt_file file;
	rt_b file_created = RT_FALSE;
	struct rt_file file8;
	rt_b file8_created = RT_FALSE;
	struct rt_input_stream *input_stream;
	struct rt_output_stream *output_stream;
	rt_char8 buffer[RT_CHAR8_HALF_BIG_STRING_SIZE];
	rt_s ret;

	if (RT_UNLIKELY(!rt_file_create(&file, file_path, RT_FILE_MODE_READ)))
		goto error;
	file_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_file_create(&file8, file_path8, RT_FILE_MODE_TRUNCATE)))
		goto error;
	file_created = RT_TRUE;

	input_stream = &file.io_device.input_stream;
	output_stream = &file8.io_device.output_stream;

	if (RT_UNLIKELY(!rt_read_lines(input_stream, buffer, RT_CHAR8_HALF_BIG_STRING_SIZE, &zz_generate_char8_callback, output_stream)))
		goto error;

	ret = RT_OK;
free:
	if (file8_created) {
		file8_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_io_device_free(&file8.io_device) && ret))
			goto error;
	}
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

static rt_s zz_generate_char8_do(const rt_char *root_dir, rt_un root_dir_size, const rt_char *sub_file_path, const rt_char *sub_file_path8)
{
	rt_char file_path[RT_FILE_PATH_SIZE];
	rt_un file_path_size;
	rt_char file_path8[RT_FILE_PATH_SIZE];
	rt_un file_path8_size;
	rt_s ret;

	if (RT_UNLIKELY(!rt_char_copy(root_dir, root_dir_size, file_path, RT_FILE_PATH_SIZE))) goto error;
	file_path_size = root_dir_size;
	if (RT_UNLIKELY(!rt_file_path_append_separator(file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(sub_file_path, rt_char_get_size(sub_file_path), file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;

	if (RT_UNLIKELY(!rt_char_copy(root_dir, root_dir_size, file_path8, RT_FILE_PATH_SIZE))) goto error;
	file_path8_size = root_dir_size;
	if (RT_UNLIKELY(!rt_file_path_append_separator(file_path8, RT_FILE_PATH_SIZE, &file_path8_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(sub_file_path8, rt_char_get_size(sub_file_path8), file_path8, RT_FILE_PATH_SIZE, &file_path8_size))) goto error;

	if (RT_UNLIKELY(!zz_generate_char8_process(file_path, file_path8))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_generate_char8(void)
{
	rt_char root_dir[RT_FILE_PATH_SIZE];
	rt_un root_dir_size;
	rt_s ret;

	if (RT_UNLIKELY(!zz_get_test_resources_dir(root_dir, RT_FILE_PATH_SIZE, &root_dir_size))) goto error;

	/* tests */
	if (RT_UNLIKELY(!rt_file_path_get_parent(root_dir, RT_FILE_PATH_SIZE, &root_dir_size))) goto error;
	/* samples */
	if (RT_UNLIKELY(!rt_file_path_get_parent(root_dir, RT_FILE_PATH_SIZE, &root_dir_size))) goto error;
	/* rt15_portable_runtime */
	if (RT_UNLIKELY(!rt_file_path_get_parent(root_dir, RT_FILE_PATH_SIZE, &root_dir_size))) goto error;

	if (RT_UNLIKELY(!zz_generate_char8_do(root_dir, root_dir_size, _R("samples/tests/src/auto/zz_test_char.c"), _R("samples/tests/src/auto/zz_test_char8.c")))) goto error;
	if (RT_UNLIKELY(!zz_generate_char8_do(root_dir, root_dir_size, _R("rpr/src/layer003/rt_char.c"), _R("rpr/src/layer003/rt_char8.c")))) goto error;
	if (RT_UNLIKELY(!zz_generate_char8_do(root_dir, root_dir_size, _R("rpr/include/layer003/rt_char.h"), _R("rpr/include/layer003/rt_char8.h")))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

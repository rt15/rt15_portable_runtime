#include "zz_utils.h"

static rt_s zz_get_dir(const rt_char *dir_name, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	enum rt_file_path_type type;
	rt_s ret;

	if (RT_UNLIKELY(!rt_file_path_get_executable_path(buffer, buffer_capacity, buffer_size))) goto error;

	/* Get bin directory. */
	if (RT_UNLIKELY(!rt_file_path_get_parent(buffer, buffer_capacity, buffer_size))) goto error;

	/* Get tests directory. */
	if (RT_UNLIKELY(!rt_file_path_get_parent(buffer, buffer_capacity, buffer_size))) goto error;

	/* Get requested directory. */
	if (RT_UNLIKELY(!rt_file_path_append_separator(buffer, buffer_capacity, buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(dir_name, rt_char_get_size(dir_name), buffer, buffer_capacity, buffer_size))) goto error;

	/* Checking the directory. */
	if (RT_UNLIKELY(!rt_file_path_get_type(buffer, &type))) goto error;
	if (type != RT_FILE_PATH_TYPE_DIR) {
		if (RT_LIKELY(type == RT_FILE_PATH_TYPE_NONE && rt_char_equals(dir_name, rt_char_get_size(dir_name), _R("tmp"), 3))) {
			/* Create the tmp directory if it does not exist yet. */
			if (RT_UNLIKELY(!rt_file_system_create_dir(buffer)))
				goto error;
		} else {
			goto error;
		}
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_get_tmp_dir(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	return zz_get_dir(_R("tmp"), buffer, buffer_capacity, buffer_size);
}

rt_s zz_get_test_resources_dir(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	return zz_get_dir(_R("test_resources"), buffer, buffer_capacity, buffer_size);
}

rt_s zz_create_file(const rt_char *file_path, const rt_char8 *str)
{
	struct rt_file file;
	rt_b file_created = RT_FALSE;
	rt_s ret;

	if (RT_UNLIKELY(!rt_file_create(&file, file_path, RT_FILE_MODE_TRUNCATE))) goto error;
	file_created = RT_TRUE;
	if (RT_UNLIKELY(!rt_io_device_write(&file.io_device.output_stream, str, rt_char8_get_size(str)))) goto error;

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

rt_s zz_check_file_content(const rt_char *file_path, const rt_char8 *expected)
{
	enum rt_file_path_type type;
	rt_char8 buffer[RT_CHAR8_BIG_STRING_SIZE];
	rt_char8 *output;
	rt_un output_size;
	rt_s ret;

	if (RT_UNLIKELY(!rt_file_path_get_type(file_path, &type))) goto error;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_FILE)) goto error;

	if (RT_UNLIKELY(!rt_small_file_read(file_path, buffer, RT_CHAR8_BIG_STRING_SIZE, RT_NULL, RT_NULL, &output, &output_size, RT_NULL))) goto error;

	if (RT_UNLIKELY(!rt_char8_equals(output, output_size, expected, rt_char8_get_size(expected)))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_check_same_files_content_do(struct rt_input_stream *input_stream1, struct rt_input_stream *input_stream2)
{
	rt_char8 buffer1[RT_CHAR8_HALF_BIG_STRING_SIZE];
	rt_un bytes_read1;
	rt_char8 buffer2[RT_CHAR8_HALF_BIG_STRING_SIZE];
	rt_un bytes_read2;
	rt_s ret;

	while (RT_TRUE) {

		if (RT_UNLIKELY(!input_stream1->read(input_stream1, buffer1, RT_CHAR8_HALF_BIG_STRING_SIZE, &bytes_read1))) goto error;
		if (RT_UNLIKELY(!input_stream2->read(input_stream2, buffer2, RT_CHAR8_HALF_BIG_STRING_SIZE, &bytes_read2))) goto error;

		if (RT_UNLIKELY(!rt_char8_equals(buffer1, bytes_read1, buffer2, bytes_read2))) goto error;

		if (!bytes_read1)
			break;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_check_same_files_content(const rt_char *file1_path, const rt_char *file2_path)
{
	struct rt_file file1;
	rt_b file1_created = RT_FALSE;
	struct rt_input_stream *input_stream1;
	struct rt_file file2;
	rt_b file2_created = RT_FALSE;
	struct rt_input_stream *input_stream2;
	rt_s ret;

	if (RT_UNLIKELY(!rt_file_create(&file1, file1_path, RT_FILE_MODE_READ))) goto error;
	file1_created = RT_TRUE;

	input_stream1 = &file1.io_device.input_stream;

	if (RT_UNLIKELY(!rt_file_create(&file2, file2_path, RT_FILE_MODE_READ))) goto error;
	file2_created = RT_TRUE;

	input_stream2 = &file2.io_device.input_stream;

	if (RT_UNLIKELY(!zz_check_same_files_content_do(input_stream1, input_stream2))) goto error;

	ret = RT_OK;
free:
	if (file2_created) {
		file2_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_io_device_free(&file2.io_device) && ret))
			goto error;
	}
	if (file1_created) {
		file1_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_io_device_free(&file1.io_device) && ret))
			goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_b zz_char_equals_with_nulls(const rt_char *str1, const rt_char *str2)
{
	rt_b ret;

	if (!str1) {
		ret = (!str2);
	} else {
		if (!str2) {
			ret = RT_FALSE;
		} else {
			while (!(ret = *(rt_uchar*)str1 - *(rt_uchar*)str2) && *str2)
			{
				str1++;
				str2++;
			}
			ret = !ret;
		}
	}

	return ret;
}

rt_b zz_char8_equals_with_nulls(const rt_char8 *str1, const rt_char8 *str2)
{
	rt_b ret;

	if (!str1) {
		ret = (!str2);
	} else {
		if (!str2) {
			ret = RT_FALSE;
		} else {
			while (!(ret = *(rt_uchar8*)str1 - *(rt_uchar8*)str2) && *str2)
			{
				str1++;
				str2++;
			}
			ret = !ret;
		}
	}

	return ret;
}

rt_s zz_read_text_file(const rt_char *file_path, enum rt_encoding encoding, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_char8 encoded_buffer[RT_CHAR8_BIG_STRING_SIZE];
	rt_char8 *encoded_output;
	rt_un encoded_output_size;
	rt_char *output;
	rt_s ret;

	if (RT_UNLIKELY(!rt_small_file_read(file_path, encoded_buffer, RT_CHAR8_BIG_STRING_SIZE, RT_NULL, RT_NULL, &encoded_output, &encoded_output_size, RT_NULL))) goto error;
	if (RT_UNLIKELY(!rt_encoding_decode(encoded_buffer, encoded_output_size, encoding, buffer, buffer_capacity, RT_NULL, RT_NULL, &output, buffer_size, RT_NULL))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_comparison_callback(const void *item1, const void *item2, void *context, rt_n *comparison_result)
{
	rt_un item1_value = *(rt_un*)item1;
	rt_un item2_value = *(rt_un*)item2;
	rt_un context_value = *(rt_un*)context;
	rt_s ret;

	if (RT_UNLIKELY(context_value != 42))
		goto error;

	*comparison_result = item1_value - item2_value;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

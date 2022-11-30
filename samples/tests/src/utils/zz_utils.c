#include "zz_utils.h"

static rt_s zz_get_dir(const rt_char *dir_name, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	enum rt_file_path_type type;
	rt_s ret;

	if (!rt_file_path_get_executable_path(buffer, buffer_capacity, buffer_size)) goto error;

	/* Get bin directory. */
	if (!rt_file_path_get_parent(buffer, buffer_capacity, buffer_size)) goto error;

	/* Get tests directory. */
	if (!rt_file_path_get_parent(buffer, buffer_capacity, buffer_size)) goto error;

	/* Get requested directory. */
	if (!rt_file_path_append_separator(buffer, buffer_capacity, buffer_size)) goto error;
	if (!rt_char_append(dir_name, rt_char_get_size(dir_name), buffer, buffer_capacity, buffer_size)) goto error;

	/* Checking the directory. */
	if (!rt_file_path_get_type(buffer, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_DIR) goto error;

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

	if (!rt_file_create(&file, file_path, RT_FILE_MODE_TRUNCATE)) goto error;
	file_created = RT_TRUE;
	if (!rt_io_device_write(&file.io_device.output_stream, str, rt_char8_get_size(str))) goto error;

	ret = RT_OK;
free:
	if (file_created) {
		file_created = RT_FALSE;
		if (!rt_io_device_free(&file.io_device) && ret)
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

	if (!rt_file_path_get_type(file_path, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_FILE) goto error;

	if (!rt_small_file_read(file_path, buffer, RT_CHAR8_BIG_STRING_SIZE, RT_NULL, RT_NULL, &output, &output_size, RT_NULL)) goto error;

	if (!rt_char8_equals(output, output_size, expected, rt_char8_get_size(expected))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_b zz_char_equals_with_nulls(const rt_char *string1, const rt_char *string2)
{
	rt_b ret;

	if (!string1) {
		ret = (!string2);
	} else {
		if (!string2) {
			ret = RT_FALSE;
		} else {
			while (!(ret = *(rt_uchar*)string1 - *(rt_uchar*)string2) && *string2)
			{
				string1++;
				string2++;
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

	if (!rt_small_file_read(file_path, encoded_buffer, RT_CHAR8_BIG_STRING_SIZE, RT_NULL, RT_NULL, &encoded_output, &encoded_output_size, RT_NULL)) goto error;
	if (!rt_encoding_decode(encoded_buffer, encoded_output_size, encoding, buffer, buffer_capacity, RT_NULL, RT_NULL, &output, buffer_size, RT_NULL)) goto error;

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

	if (context_value != 42)
		goto error;

	*comparison_result = item1_value - item2_value;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

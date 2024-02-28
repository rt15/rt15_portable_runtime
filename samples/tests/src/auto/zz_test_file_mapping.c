#include <rpr.h>

#include "zz_utils.h"

#define ZZ_TEST_FILE_MAPPING_FILE_CONTENT "firt_line\nsecond_line\noneCR\rtwoCR\r\rtwoLF\n\nCRLF\r\nLFCR\n\rlast_line"

static rt_s zz_test_file_mapping_read(const rt_char *file_path)
{
	struct rt_file_mapping file_mapping;
	rt_b file_mapping_created = RT_FALSE;
	struct rt_file_mapping_view file_mapping_view;
	rt_b file_mapping_view_created = RT_FALSE;
	rt_s ret;

	if (RT_UNLIKELY(!rt_file_mapping_create(&file_mapping, file_path, RT_FILE_MAPPING_RIGHT_READ, 0)))
		goto error;
	file_mapping_created = RT_TRUE;

	if (RT_UNLIKELY(file_mapping.file_size != 63))
		goto error;

	if (RT_UNLIKELY(!rt_file_mapping_view_create(&file_mapping_view, &file_mapping, RT_FILE_MAPPING_RIGHT_READ, 0, 0)))
		goto error;
	file_mapping_view_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_char8_equals(file_mapping_view.area, 63, ZZ_TEST_FILE_MAPPING_FILE_CONTENT, 63)))
		goto error;

	ret = RT_OK;
free:
	if (file_mapping_view_created) {
		file_mapping_view_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_file_mapping_view_free(&file_mapping_view) && ret))
			goto error;
	}

	if (file_mapping_created) {
		file_mapping_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_file_mapping_free(&file_mapping) && ret))
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_file_mapping_write_with_size(const rt_char *file_path, rt_un size)
{
	struct rt_file_mapping file_mapping;
	rt_b file_mapping_created = RT_FALSE;
	struct rt_file_mapping_view file_mapping_view;
	rt_b file_mapping_view_created = RT_FALSE;
	rt_char8 *text;
	rt_un i;
	rt_s ret;

	if (RT_UNLIKELY(!rt_file_mapping_create(&file_mapping, file_path, RT_FILE_MAPPING_RIGHT_READ_WRITE, size * 2)))
		goto error;
	file_mapping_created = RT_TRUE;

	if (RT_UNLIKELY(file_mapping.file_size != 63))
		goto error;

	if (RT_UNLIKELY(!rt_file_mapping_view_create(&file_mapping_view, &file_mapping, RT_FILE_MAPPING_RIGHT_READ_WRITE, 0, size)))
		goto error;
	file_mapping_view_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_char8_equals(file_mapping_view.area, 63, ZZ_TEST_FILE_MAPPING_FILE_CONTENT, 63)))
		goto error;

	if (RT_UNLIKELY(!rt_file_mapping_set_file_size(&file_mapping, size)))
		goto error;

	if (RT_UNLIKELY(file_mapping.file_size != size))
		goto error;

	text = file_mapping_view.area;

	for (i = 0; i < size; i++)
		text[i] = 'x';

	if (RT_UNLIKELY(!rt_file_mapping_view_flush(&file_mapping_view, text, size)))
		goto error;

	if (RT_UNLIKELY(file_mapping.file_size != size))
		goto error;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(file_mapping.initial_file_size != 63))
		goto error;

	if (RT_UNLIKELY(file_mapping.max_size != size * 2))
		goto error;

#endif

	ret = RT_OK;
free:
	if (file_mapping_view_created) {
		file_mapping_view_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_file_mapping_view_free(&file_mapping_view) && ret))
			goto error;
	}

	if (file_mapping_created) {
		file_mapping_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_file_mapping_free(&file_mapping) && ret))
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_file_mapping_check_file(const rt_char *file_path, rt_un size)
{
	rt_char8 buffer[RT_CHAR8_BIG_STRING_SIZE];
	rt_char8 *output;
	rt_un output_size;
	rt_un i;
	rt_s ret;

	if (RT_UNLIKELY(!rt_small_file_read(file_path, buffer, RT_CHAR8_BIG_STRING_SIZE, RT_NULL, RT_NULL, &output, &output_size, RT_NULL)))
		goto error;

	if (RT_UNLIKELY(output_size != size))
		goto error;

	for (i = 0; i < size; i++) {
		if (RT_UNLIKELY(output[i] != 'x'))
			goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_file_mapping_write(const rt_char *lines_file_path, const rt_char *tmp_file_path)
{
	rt_s ret;

	if (RT_UNLIKELY(!rt_file_system_copy_file(lines_file_path, tmp_file_path, RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_file_mapping_write_with_size(tmp_file_path, 80))) goto error;
	if (RT_UNLIKELY(!zz_test_file_mapping_check_file(tmp_file_path, 80))) goto error;

	if (RT_UNLIKELY(!rt_file_system_copy_file(lines_file_path, tmp_file_path, RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_file_mapping_write_with_size(tmp_file_path, 40))) goto error;
	if (RT_UNLIKELY(!zz_test_file_mapping_check_file(tmp_file_path, 40))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_file_mapping(void)
{
	rt_char lines_file_path[RT_FILE_PATH_SIZE];
	rt_un lines_file_path_size;
	rt_char tmp_file_path[RT_FILE_PATH_SIZE];
	rt_un tmp_file_path_size;
	rt_s ret;

	if (RT_UNLIKELY(!zz_get_test_resources_dir(lines_file_path, RT_FILE_PATH_SIZE, &lines_file_path_size))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(lines_file_path, RT_FILE_PATH_SIZE, &lines_file_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("lines.dat"), 9, lines_file_path, RT_FILE_PATH_SIZE, &lines_file_path_size))) goto error;

	if (RT_UNLIKELY(!zz_test_file_mapping_read(lines_file_path))) goto error;

	if (RT_UNLIKELY(!zz_get_tmp_dir(tmp_file_path, RT_FILE_PATH_SIZE, &tmp_file_path_size))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(tmp_file_path, RT_FILE_PATH_SIZE, &tmp_file_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("zz_test_file_mapping.dat"), 24, tmp_file_path, RT_FILE_PATH_SIZE, &tmp_file_path_size))) goto error;

	if (RT_UNLIKELY(!zz_test_file_mapping_write(lines_file_path, tmp_file_path))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

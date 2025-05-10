#include <rpr.h>

#include "zz_utils.h"

rt_s zz_test_small_file_read_do(const rt_char *file_path, struct rt_heap *heap)
{
	rt_char8 buffer[200];
	rt_un data_size = rt_char8_get_size("APPEND1APPEND2");
	rt_char8 *output;
	rt_un output_size;
	void *heap_buffer = RT_NULL;
	rt_un heap_buffer_capacity = 0;
	rt_un i;
	rt_s ret = RT_FAILED;

	for (i = 0; i < 200; i++)
		buffer[i] = 'z';

	/* Buffer just the right size. */
	if (RT_UNLIKELY(!rt_small_file_read(file_path, buffer, data_size + 1, RT_NULL, RT_NULL, &output, &output_size, RT_NULL))) goto end;
	if (RT_UNLIKELY(output_size != rt_char8_get_size(output))) goto end;
	if (RT_UNLIKELY(!rt_char8_equals(output, output_size, "APPEND1APPEND2", data_size))) goto end;

	/* Buffer too small. */
	if (RT_UNLIKELY(rt_small_file_read(file_path, buffer, data_size, RT_NULL, RT_NULL, &output, &output_size, RT_NULL))) goto end;

	/* Buffer too small, heap provided. */
	if (RT_UNLIKELY(!rt_small_file_read(file_path, buffer, data_size, &heap_buffer, &heap_buffer_capacity, &output, &output_size, heap))) goto end;
	if (RT_UNLIKELY(!heap_buffer)) goto end;
	if (RT_UNLIKELY(output != heap_buffer)) goto end;
	if (RT_UNLIKELY(output_size != rt_char8_get_size(output))) goto end;
	if (RT_UNLIKELY(!rt_char8_equals(output, output_size, "APPEND1APPEND2", data_size))) goto end;

	ret = RT_OK;
end:
	if (heap_buffer) {
		if (RT_UNLIKELY(!heap->free(heap, &heap_buffer)))
			ret = RT_FAILED;
	}

	return ret;
}

rt_s zz_test_small_file_read(const rt_char *file_path)
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_runtime_heap_create(&runtime_heap)))
		goto end;
	runtime_heap_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_test_small_file_read_do(file_path, &runtime_heap.heap)))
		goto end;

	ret = RT_OK;
end:
	if (runtime_heap_created) {
		if (RT_UNLIKELY(!runtime_heap.heap.close(&runtime_heap.heap)))
			ret = RT_FAILED;
	}

	return ret;
}

rt_s zz_test_small_file_write(const rt_char *file_path, enum rt_small_file_mode mode, const rt_char8 *data)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_small_file_write(file_path, mode, data, rt_char8_get_size(data))))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_small_file_do(const rt_char *file_path)
{
	rt_s ret = RT_FAILED;

	/* NEW, without existing file. */
	if (RT_UNLIKELY(!rt_file_system_delete_file_if_exists(file_path))) goto end;
	if (RT_UNLIKELY(!zz_test_small_file_write(file_path, RT_SMALL_FILE_MODE_NEW, "NEW1"))) goto end;
	if (RT_UNLIKELY(!zz_check_file_content(file_path, "NEW1"))) goto end;

	/* NEW, with existing file. */
	if (RT_UNLIKELY(zz_test_small_file_write(file_path, RT_SMALL_FILE_MODE_NEW, "NEW2"))) goto end;
	if (RT_UNLIKELY(!zz_check_file_content(file_path, "NEW1"))) goto end;

	/* TRUNCATE, without existing file. */
	if (RT_UNLIKELY(!rt_file_system_delete_file_if_exists(file_path))) goto end;
	if (RT_UNLIKELY(!zz_test_small_file_write(file_path, RT_SMALL_FILE_MODE_TRUNCATE, "TRUNCATE1"))) goto end;
	if (RT_UNLIKELY(!zz_check_file_content(file_path, "TRUNCATE1"))) goto end;

	/* TRUNCATE, with existing file. */
	if (RT_UNLIKELY(!zz_test_small_file_write(file_path, RT_SMALL_FILE_MODE_TRUNCATE, "TRUNCATE2"))) goto end;
	if (RT_UNLIKELY(!zz_check_file_content(file_path, "TRUNCATE2"))) goto end;

	/* APPEND, without existing file. */
	if (RT_UNLIKELY(!rt_file_system_delete_file_if_exists(file_path))) goto end;
	if (RT_UNLIKELY(!zz_test_small_file_write(file_path, RT_SMALL_FILE_MODE_APPEND, "APPEND1"))) goto end;
	if (RT_UNLIKELY(!zz_check_file_content(file_path, "APPEND1"))) goto end;

	/* APPEND, with existing file. */
	if (RT_UNLIKELY(!zz_test_small_file_write(file_path, RT_SMALL_FILE_MODE_APPEND, "APPEND2"))) goto end;
	if (RT_UNLIKELY(!zz_check_file_content(file_path, "APPEND1APPEND2"))) goto end;

	if (RT_UNLIKELY(!zz_test_small_file_read(file_path))) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_small_file(void)
{
	rt_char file_path[RT_FILE_PATH_SIZE];
	rt_un file_path_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_get_tmp_dir(file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto end;
	if (RT_UNLIKELY(!rt_file_path_append_separator(file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append(_R("small.txt"), 9, file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto end;

	if (RT_UNLIKELY(!zz_test_small_file_do(file_path))) goto end;

	ret = RT_OK;
end:
	return ret;
}

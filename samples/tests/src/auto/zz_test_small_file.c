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
	rt_s ret;

	for (i = 0; i < 200; i++)
		buffer[i] = 'z';

	/* Buffer just the right size. */
	if (!rt_small_file_read(file_path, buffer, data_size + 1, RT_NULL, RT_NULL, &output, &output_size, RT_NULL)) goto error;
	if (output_size != rt_char8_get_size(output)) goto error;
	if (!rt_char8_equals(output, output_size, "APPEND1APPEND2", data_size)) goto error;

	/* Buffer too small. */
	if (rt_small_file_read(file_path, buffer, data_size, RT_NULL, RT_NULL, &output, &output_size, RT_NULL)) goto error;

	/* Buffer too small, heap provided. */
	if (!rt_small_file_read(file_path, buffer, data_size, &heap_buffer, &heap_buffer_capacity, &output, &output_size, heap)) goto error;
	if (!heap_buffer) goto error;
	if (output != heap_buffer) goto error;
	if (output_size != rt_char8_get_size(output)) goto error;
	if (!rt_char8_equals(output, output_size, "APPEND1APPEND2", data_size)) goto error;

	ret = RT_OK;
free:
	if (heap_buffer) {
		if (!heap->free(heap, &heap_buffer) && ret)
			goto error;
	}

	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_small_file_read(const rt_char *file_path)
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	rt_s ret;

	if (!rt_runtime_heap_create(&runtime_heap))
		goto error;
	runtime_heap_created = RT_TRUE;

	if (!zz_test_small_file_read_do(file_path, &runtime_heap.heap))
		goto error;

	ret = RT_OK;
free:
	if (runtime_heap_created) {
		runtime_heap_created = RT_FALSE;
		if (!runtime_heap.heap.close(&runtime_heap.heap) && ret)
			goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_small_file_write(const rt_char *file_path, enum rt_small_file_mode mode, const rt_char8 *data)
{
	rt_s ret;

	if (!rt_small_file_write(file_path, mode, data, rt_char8_get_size(data)))
		goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_small_file_do(const rt_char *file_path)
{
	rt_s ret;

	/* NEW, without existing file. */
	if (!rt_file_system_delete_file_if_exists(file_path)) goto error;
	if (!zz_test_small_file_write(file_path, RT_SMALL_FILE_MODE_NEW, "NEW1")) goto error;
	if (!zz_check_file_content(file_path, "NEW1")) goto error;

	/* NEW, with existing file. */
	if (zz_test_small_file_write(file_path, RT_SMALL_FILE_MODE_NEW, "NEW2")) goto error;
	if (!zz_check_file_content(file_path, "NEW1")) goto error;

	/* TRUNCATE, without existing file. */
	if (!rt_file_system_delete_file_if_exists(file_path)) goto error;
	if (!zz_test_small_file_write(file_path, RT_SMALL_FILE_MODE_TRUNCATE, "TRUNCATE1")) goto error;
	if (!zz_check_file_content(file_path, "TRUNCATE1")) goto error;

	/* TRUNCATE, with existing file. */
	if (!zz_test_small_file_write(file_path, RT_SMALL_FILE_MODE_TRUNCATE, "TRUNCATE2")) goto error;
	if (!zz_check_file_content(file_path, "TRUNCATE2")) goto error;

	/* APPEND, without existing file. */
	if (!rt_file_system_delete_file_if_exists(file_path)) goto error;
	if (!zz_test_small_file_write(file_path, RT_SMALL_FILE_MODE_APPEND, "APPEND1")) goto error;
	if (!zz_check_file_content(file_path, "APPEND1")) goto error;

	/* APPEND, with existing file. */
	if (!zz_test_small_file_write(file_path, RT_SMALL_FILE_MODE_APPEND, "APPEND2")) goto error;
	if (!zz_check_file_content(file_path, "APPEND1APPEND2")) goto error;

	if (!zz_test_small_file_read(file_path)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_small_file()
{
	rt_char file_path[RT_FILE_PATH_SIZE];
	rt_un file_path_size;
	rt_s ret;

	if (!zz_get_tmp_dir(file_path, RT_FILE_PATH_SIZE, &file_path_size)) goto error;
	if (!rt_file_path_append_separator(file_path, RT_FILE_PATH_SIZE, &file_path_size)) goto error;
	if (!rt_char_append(_R("small.txt"), 9, file_path, RT_FILE_PATH_SIZE, &file_path_size)) goto error;

	if (!zz_test_small_file_do(file_path)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

#include "layer004/rt_small_file.h"

#include "layer002/rt_error.h"
#include "layer002/rt_io_device.h"
#include "layer003/rt_file.h"

rt_s rt_small_file_read(const rt_char *file_path, rt_char8 *buffer, rt_un buffer_capacity, void **heap_buffer, rt_un *heap_buffer_capacity, rt_char8 **output, rt_un *output_size, struct rt_heap *heap)
{
	rt_b file_created = RT_FALSE;
	struct rt_file file;
	rt_un64 file_size;
	struct rt_input_stream *input_stream;
	rt_s ret;

	if (RT_UNLIKELY(!rt_file_create(&file, file_path, RT_FILE_MODE_READ)))
		goto error;
	file_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_file_get_size(&file, &file_size)))
		goto error;

	if (RT_UNLIKELY(!rt_heap_alloc_if_needed(buffer, buffer_capacity, heap_buffer, heap_buffer_capacity, (void**)output, (rt_un)(file_size + 1), heap)))
		goto error;

	input_stream = &file.io_device.input_stream;
	if (RT_UNLIKELY(!input_stream->read(input_stream, *output, (rt_un)file_size, output_size)))
		goto error;

	/* We know the size of the file so we should be able to read it completely. */
	if (RT_UNLIKELY(*output_size != file_size)) {
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto error;
	}

	/* Add terminating zero. */
	(*output)[*output_size] = 0;

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

rt_s rt_small_file_write(const rt_char *file_path, enum rt_small_file_mode mode, const rt_char8 *data, rt_un bytes_to_write)
{
	enum rt_file_mode file_mode;
	rt_b file_created = RT_FALSE;
	struct rt_file file;
	struct rt_output_stream *output_stream;
	rt_s ret;

	switch (mode) {
	case RT_SMALL_FILE_MODE_APPEND:
		file_mode = RT_FILE_MODE_READ_WRITE;
		break;
	case RT_SMALL_FILE_MODE_TRUNCATE:
		file_mode = RT_FILE_MODE_TRUNCATE;
		break;
	case RT_SMALL_FILE_MODE_NEW:
		file_mode = RT_FILE_MODE_NEW;
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	if (RT_UNLIKELY(!rt_file_create(&file, file_path, file_mode)))
		goto error;
	file_created = RT_TRUE;

	if (mode == RT_SMALL_FILE_MODE_APPEND) {
		if (RT_UNLIKELY(!rt_file_set_pointer(&file, 0, RT_FILE_POSITION_END)))
			goto error;
	}

	output_stream = &file.io_device.output_stream;
	if (RT_UNLIKELY(!output_stream->write(output_stream, data, bytes_to_write)))
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

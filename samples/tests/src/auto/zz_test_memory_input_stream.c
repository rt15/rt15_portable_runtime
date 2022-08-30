#include <rpr.h>

rt_s zz_test_memory_input_stream_do(rt_un bytes_to_read, const rt_char8 *expected)
{
	const rt_char8 *stream_content = "foo";
	struct rt_memory_input_stream memory_input_stream;
	rt_char8 buffer[200];
	rt_un bytes_read;
	rt_un expected_size;
	rt_un remaining_size;
	rt_s ret;

	/* Read the requested size. */
	if (!rt_memory_input_stream_create(&memory_input_stream, stream_content, 3)) goto error;
	if (!memory_input_stream.input_stream.read(&memory_input_stream.input_stream, buffer, bytes_to_read, &bytes_read)) goto error;
	expected_size = rt_char8_get_size(expected);
	if (bytes_read != expected_size) goto error;
	if (!rt_char8_equals(buffer, expected_size, expected, expected_size)) goto error;

	/* Read the remaining of the stream. */
	if (!memory_input_stream.input_stream.read(&memory_input_stream.input_stream, buffer, 200, &bytes_read)) goto error;
	remaining_size = 3 - expected_size;
	if (bytes_read != remaining_size) goto error;
	if (!rt_char8_equals(buffer, remaining_size, &stream_content[expected_size], remaining_size)) goto error;

	/* The stream is now empty. */
	if (!memory_input_stream.input_stream.read(&memory_input_stream.input_stream, buffer, 200, &bytes_read)) goto error;
	if (bytes_read) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_memory_input_stream()
{
	rt_s ret;

	if (!zz_test_memory_input_stream_do(0, "")) goto error;
	if (!zz_test_memory_input_stream_do(1, "f")) goto error;
	if (!zz_test_memory_input_stream_do(2, "fo")) goto error;
	if (!zz_test_memory_input_stream_do(3, "foo")) goto error;
	if (!zz_test_memory_input_stream_do(4, "foo")) goto error;
	if (!zz_test_memory_input_stream_do(5, "foo")) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

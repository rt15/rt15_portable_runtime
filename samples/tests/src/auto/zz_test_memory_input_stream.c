#include <rpr.h>

rt_s zz_test_memory_input_stream_do(rt_un bytes_to_read, const rt_char8 *expected)
{
	const rt_char8 *stream_content = "foo";
	struct rt_memory_input_stream memory_input_stream;
	rt_char8 buffer[200];
	rt_un bytes_read;
	rt_un expected_size;
	rt_un remaining_size;
	rt_s ret = RT_FAILED;

	/* Read the requested size. */
	if (RT_UNLIKELY(!rt_memory_input_stream_create(&memory_input_stream, stream_content, 3))) goto end;
	if (RT_UNLIKELY(!memory_input_stream.input_stream.read(&memory_input_stream.input_stream, buffer, bytes_to_read, &bytes_read))) goto end;
	expected_size = rt_char8_get_size(expected);
	if (RT_UNLIKELY(bytes_read != expected_size)) goto end;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, expected_size, expected, expected_size))) goto end;

	/* Read the remaining of the stream. */
	if (RT_UNLIKELY(!memory_input_stream.input_stream.read(&memory_input_stream.input_stream, buffer, 200, &bytes_read))) goto end;
	remaining_size = 3 - expected_size;
	if (RT_UNLIKELY(bytes_read != remaining_size)) goto end;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, remaining_size, &stream_content[expected_size], remaining_size))) goto end;

	/* The stream is now empty. */
	if (RT_UNLIKELY(!memory_input_stream.input_stream.read(&memory_input_stream.input_stream, buffer, 200, &bytes_read))) goto end;
	if (RT_UNLIKELY(bytes_read)) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_memory_input_stream(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_memory_input_stream_do(0, ""))) goto end;
	if (RT_UNLIKELY(!zz_test_memory_input_stream_do(1, "f"))) goto end;
	if (RT_UNLIKELY(!zz_test_memory_input_stream_do(2, "fo"))) goto end;
	if (RT_UNLIKELY(!zz_test_memory_input_stream_do(3, "foo"))) goto end;
	if (RT_UNLIKELY(!zz_test_memory_input_stream_do(4, "foo"))) goto end;
	if (RT_UNLIKELY(!zz_test_memory_input_stream_do(5, "foo"))) goto end;

	ret = RT_OK;
end:
	return ret;
}

#include <rpr.h>

static rt_s zz_test_slurp_do(const rt_char8 *input)
{
	struct rt_memory_input_stream memory_input_stream;
	struct rt_memory_output_stream memory_output_stream;
	rt_b memory_output_stream_created = RT_FALSE;
	rt_char8 memory_output_stream_buffer[RT_CHAR8_QUARTER_BIG_STRING_SIZE];
	rt_char8 slurp_buffer[16];
	rt_char8 *output_stream_data;
	rt_un i;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_memory_input_stream_create(&memory_input_stream, input, 200))) goto end;
	if (RT_UNLIKELY(!rt_memory_output_stream_create(&memory_output_stream, memory_output_stream_buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, RT_NULL, RT_CHAR8_QUARTER_BIG_STRING_SIZE))) goto end;
	memory_output_stream_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_slurp(&memory_input_stream.input_stream, &memory_output_stream.output_stream, slurp_buffer, 16)))
		goto end;

	if (RT_UNLIKELY(memory_output_stream.size != 200))
		goto end;

	output_stream_data = rt_memory_output_stream_get_data(&memory_output_stream);
	for (i = 0; i < 200; i++) {
		if (RT_UNLIKELY(output_stream_data[i] != 'a'))
			goto end;
	}

	ret = RT_OK;
end:
	if (memory_output_stream_created) {
		if (RT_UNLIKELY(!rt_memory_output_stream_free(&memory_output_stream)))
			ret = RT_FAILED;
	}

	return ret;
}

rt_s zz_test_slurp(void)
{
	rt_char8 input[RT_CHAR8_HALF_BIG_STRING_SIZE];
	rt_un i;
	rt_s ret = RT_FAILED;

	for (i = 0; i < RT_CHAR8_HALF_BIG_STRING_SIZE; i++)
		input[i] = 'a';

	if (RT_UNLIKELY(!zz_test_slurp_do(input))) goto end;

	ret = RT_OK;
end:
	return ret;
}

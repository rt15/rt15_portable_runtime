#include <rpr.h>

rt_s zz_test_error_message(void)
{
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_char second_buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un second_buffer_size;
	rt_un message_size;
	rt_s ret;

	/* Intentionally generate an error (RT_ERROR_INSUFFICIENT_BUFFER). */
	if (RT_UNLIKELY(rt_char_copy(_R("Too long string."), 16, buffer, 3)))
		goto error;

	buffer_size = 0;
	if (RT_UNLIKELY(!rt_error_message_append_last(buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto error;
	message_size = rt_char_get_size(buffer);
	if (RT_UNLIKELY(message_size != buffer_size)) goto error;
#ifdef RT_DEFINE_WINDOWS
	/* Error messages end with \r\n under Windows so there must be room for them. The library trim them. */
	message_size += 2;
#endif

	/* Just enough room. */
	second_buffer_size = 0;
	if (RT_UNLIKELY(!rt_error_message_append_last(second_buffer, message_size + 1, &second_buffer_size))) goto error;
	if (RT_UNLIKELY(rt_char_get_size(second_buffer) != second_buffer_size)) goto error;
	if (RT_UNLIKELY(!rt_char_equals(buffer, buffer_size, second_buffer, second_buffer_size))) goto error;

	/* Not enough room. */
	second_buffer_size = 0;
	if (RT_UNLIKELY(rt_error_message_append_last(second_buffer, message_size, &second_buffer_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

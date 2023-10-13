#include <rpr.h>

rt_s zz_read_line()
{
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_char message[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un message_size;
	rt_s ret;

	if (RT_UNLIKELY(!rt_console_read_line(buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(rt_char_get_size(buffer) != buffer_size)) goto error;

	message_size = 1;
	if (RT_UNLIKELY(!rt_char_copy(_R("\""), message_size, message, RT_CHAR_HALF_BIG_STRING_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_char_append(buffer, buffer_size, message, RT_CHAR_HALF_BIG_STRING_SIZE, &message_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("\"\n"), 2, message, RT_CHAR_HALF_BIG_STRING_SIZE, &message_size))) goto error;
	if (RT_UNLIKELY(!rt_console_write_string_with_size(message, message_size))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

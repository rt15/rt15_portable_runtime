#include <rpr.h>

rt_s zz_manual_test_os_version()
{
	rt_un major;
	rt_un minor;
	rt_un patch;
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret;

	if (RT_UNLIKELY(!rt_os_version_get(&major, &minor, &patch))) goto error;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Operating system version = ")))) goto error;

	buffer_size = 0;
	if (RT_UNLIKELY(!rt_char_append_un(major, 10,   buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append_char(_R('.'),  buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append_un(minor, 10,   buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append_char(_R('.'),  buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append_un(patch, 10,   buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto error;

	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

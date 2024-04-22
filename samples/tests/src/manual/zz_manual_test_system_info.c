#include <rpr.h>

rt_s zz_manual_test_system_info(void)
{
	rt_un logical_cpu_count;
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret;

	if (RT_UNLIKELY(!rt_system_info_get_logical_cpu_count(&logical_cpu_count))) goto error;

	buffer_size = 20;
	if (RT_UNLIKELY(!rt_char_copy(_R("Logical CPU count = "), buffer_size, buffer, RT_CHAR_HALF_BIG_STRING_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_char_append_un(logical_cpu_count, 10, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

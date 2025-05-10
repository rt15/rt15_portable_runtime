#include <rpr.h>

rt_s zz_manual_test_get_process_id(void)
{
	rt_un process_id;
	rt_char buffer[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_get_process_id(&process_id))) goto end;

	buffer_size = 13;
	if (RT_UNLIKELY(!rt_char_copy(_R("Process id = "), buffer_size, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE))) goto end;
	if (RT_UNLIKELY(!rt_char_append_un(process_id, 10, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size))) goto end;

	ret = RT_OK;
end:
	return ret;
}

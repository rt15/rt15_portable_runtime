#include <rpr.h>

rt_s zz_manual_test_env_var(void)
{
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_char *variable_name;
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	variable_name = _R("SystemRoot");
#else
	variable_name = _R("HOME");
#endif

	if (RT_UNLIKELY(!rt_env_var_get(variable_name, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;

	if (RT_UNLIKELY(!rt_console_write_str(variable_name))) goto end;
	if (RT_UNLIKELY(!rt_console_write_str(_R(" = \"")))) goto end;
	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_console_write_str(_R("\"\n")))) goto end;

	ret = RT_OK;
end:
	return ret;
}

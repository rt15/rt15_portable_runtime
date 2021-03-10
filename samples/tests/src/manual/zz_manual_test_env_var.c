#include <rpr.h>

rt_s zz_manual_test_env_var()
{
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_char *variable_name;
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	variable_name = _R("SystemRoot");
#else
	variable_name = _R("HOME");
#endif

	if (!rt_env_var_get(variable_name, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)) goto error;

	if (!rt_console_write_string(variable_name)) goto error;
	if (!rt_console_write_string(_R(" = \""))) goto error;
	if (!rt_console_write_string_with_size(buffer, buffer_size)) goto error;
	if (!rt_console_write_string(_R("\"\n"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

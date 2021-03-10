#include <rpr.h>

rt_s zz_test_env_var()
{
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_char *variable_name;
	rt_s ret;

	/* Asking for the value of an unknown variable should result in error. */
	if (rt_env_var_get(_R("NonExistingName"), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)) goto error;

#ifdef RT_DEFINE_WINDOWS
	variable_name = _R("SystemRoot");
#else
	variable_name = _R("HOME");
#endif

	/* Check existing variable. */
	if (!rt_env_var_get(variable_name, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)) goto error;
	if (!buffer_size) goto error;
	if (rt_char_get_size(buffer) != buffer_size) goto error;

	/* Add a new variable. */
	if (!rt_env_var_set(_R("RT_FOO"), _R("BAR"))) goto error;

	/* Check the new variable. */
	if (!rt_env_var_get(_R("RT_FOO"), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)) goto error;
	if (buffer_size != 3) goto error;
	if (rt_char_get_size(buffer) != 3) goto error;
	if (!rt_char_equals(buffer, 3, _R("BAR"), 3)) goto error;

	/* Delete the new variable. */
	if (!rt_env_var_delete(_R("RT_FOO"))) goto error;

	/* Check that the variable is not available anymore. */
	if (rt_env_var_get(_R("RT_FOO"), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

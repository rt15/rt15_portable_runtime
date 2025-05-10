#include <rpr.h>

rt_s zz_test_env_var(void)
{
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_char *variable_name;
	rt_s ret = RT_FAILED;

	/* Asking for the value of an unknown variable should result in error. */
	if (RT_UNLIKELY(rt_env_var_get(_R("NonExistingName"), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;

#ifdef RT_DEFINE_WINDOWS
	variable_name = _R("SystemRoot");
#else
	variable_name = _R("HOME");
#endif

	/* Check existing variable. */
	if (RT_UNLIKELY(!rt_env_var_get(variable_name, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!buffer_size)) goto end;
	if (RT_UNLIKELY(rt_char_get_size(buffer) != buffer_size)) goto end;

	/* Add a new variable. */
	if (RT_UNLIKELY(!rt_env_var_set(_R("RT_FOO"), _R("BAR")))) goto end;

	/* Check the new variable. */
	if (RT_UNLIKELY(!rt_env_var_get(_R("RT_FOO"), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(buffer_size != 3)) goto end;
	if (RT_UNLIKELY(rt_char_get_size(buffer) != 3)) goto end;
	if (RT_UNLIKELY(!rt_char_equals(buffer, 3, _R("BAR"), 3))) goto end;

	/* Edit the variable. */
	if (RT_UNLIKELY(!rt_env_var_set(_R("RT_FOO"), _R("TEAM")))) goto end;
	if (RT_UNLIKELY(!rt_env_var_get(_R("RT_FOO"), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(buffer_size != 4)) goto end;
	if (RT_UNLIKELY(rt_char_get_size(buffer) != 4)) goto end;
	if (RT_UNLIKELY(!rt_char_equals(buffer, 4, _R("TEAM"), 4))) goto end;

	/* Delete the new variable. */
	if (RT_UNLIKELY(!rt_env_var_delete(_R("RT_FOO")))) goto end;

	/* Delete an unknown variable (should be successful even if it does not exist). */
	if (RT_UNLIKELY(!rt_env_var_delete(_R("RT_BAR")))) goto end;

	/* Check that the variable is not available anymore. */
	if (RT_UNLIKELY(rt_env_var_get(_R("RT_FOO"), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;

	ret = RT_OK;
end:
	return ret;
}

#include <rpr.h>

rt_s zz_display_env_vars(void)
{
	struct rt_env_vars env_vars;
	rt_b env_vars_created = RT_FALSE;
	rt_char **env_vars_array;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_env_vars_create(&env_vars)))
		goto end;
	env_vars_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_env_vars_get_array(&env_vars, &env_vars_array)))
		goto end;

	while (*env_vars_array) {
		if (RT_UNLIKELY(!rt_console_write_str(*env_vars_array))) goto end;
		if (RT_UNLIKELY(!rt_console_write_str_with_size(_R("\n"), 1))) goto end;
		env_vars_array++;
	}

	ret = RT_OK;
end:
	if (env_vars_created) {
		if (RT_UNLIKELY(!rt_env_vars_free(&env_vars)))
			ret = RT_FAILED;
	}

	return ret;
}

rt_s zz_display_env_var(const rt_char *name)
{
	rt_char buffer[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_char message[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un message_size;
	rt_s ret = RT_FAILED;

	message_size = rt_char_get_size(name);
	if (RT_UNLIKELY(!rt_char_copy(name, message_size, message, RT_CHAR_HALF_BIG_STRING_SIZE))) goto end;
	if (RT_UNLIKELY(!rt_char_append_char(_R('='), message, RT_CHAR_HALF_BIG_STRING_SIZE, &message_size))) goto end;
	if (RT_UNLIKELY(!rt_env_var_get(name, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append(buffer, buffer_size, message, RT_CHAR_HALF_BIG_STRING_SIZE, &message_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), message, RT_CHAR_HALF_BIG_STRING_SIZE, &message_size))) goto end;
	if (RT_UNLIKELY(!rt_console_write_str_with_size(message, message_size))) goto end;

	ret = RT_OK;
end:
	return ret;
}

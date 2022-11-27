#include <rpr.h>

rt_s zz_display_env_vars()
{
	struct rt_env_vars env_vars;
	rt_b env_vars_created = RT_FALSE;
	rt_char **env_vars_array;
	rt_s ret;

	if (!rt_env_vars_create(&env_vars))
		goto error;
	env_vars_created = RT_TRUE;

	if (!rt_env_vars_get_array(&env_vars, &env_vars_array))
		goto error;

	while (*env_vars_array) {
		if (!rt_console_write_string(*env_vars_array)) goto error;
		if (!rt_console_write_string_with_size(_R("\n"), 1)) goto error;
		env_vars_array++;
	}

	ret = RT_OK;
free:
	if (env_vars_created) {
		env_vars_created = RT_FALSE;
		if (!rt_env_vars_free(&env_vars) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_display_env_var(const rt_char *name)
{
	rt_char buffer[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_char message[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un message_size;
	rt_s ret;

	message_size = rt_char_get_size(name);
	if (!rt_char_copy(name, message_size, message, RT_CHAR_HALF_BIG_STRING_SIZE)) goto error;
	if (!rt_char_append_char(_R('='), message, RT_CHAR_HALF_BIG_STRING_SIZE, &message_size)) goto error;
	if (!rt_env_var_get(name, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size)) goto error;
	if (!rt_char_append(buffer, buffer_size, message, RT_CHAR_HALF_BIG_STRING_SIZE, &message_size)) goto error;
	if (!rt_char_append_char(_R('\n'), message, RT_CHAR_HALF_BIG_STRING_SIZE, &message_size)) goto error;
	if (!rt_console_write_string_with_size(message, message_size)) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
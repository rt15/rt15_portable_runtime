#include <rpr.h>

/**
 * Check the array against the block.
 */
static rt_s zz_check_env_vars(struct rt_env_vars *env_vars)
{
	rt_char *env_vars_block;
	rt_char **env_vars_array;
	rt_un i;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_env_vars_get_block(env_vars, &env_vars_block))) goto end;
	if (RT_UNLIKELY(!rt_env_vars_get_array(env_vars, &env_vars_array))) goto end;

	if (!env_vars_block[0]) {
		if (RT_UNLIKELY(env_vars_array[0]))
			goto end;
	} else {
		i = 0;
		while (RT_TRUE) {
			if (RT_UNLIKELY(!rt_char_equals(env_vars_array[i], rt_char_get_size(env_vars_array[i]), env_vars_block, rt_char_get_size(env_vars_block))))
				goto end;

			while (*env_vars_block) {
				env_vars_block++;
			}
			env_vars_block++;
			i++;

			/* End of array? */
			if (!env_vars_array[i]) {
				/* Should be also the end of the block. */
				if (RT_UNLIKELY(*env_vars_block))
					goto end;
				break;
			}
		}
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_env_vars(void)
{
	struct rt_env_vars env_vars1;
	struct rt_env_vars env_vars2;
	struct rt_env_vars env_vars3;
	rt_b env_vars1_created = RT_FALSE;
	rt_b env_vars2_created = RT_FALSE;
	rt_b env_vars3_created = RT_FALSE;
	rt_char buffer[256];
	rt_un buffer_size;
	rt_b contains;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_env_vars_create(&env_vars1))) goto end;
	env_vars1_created = RT_TRUE;
	if (RT_UNLIKELY(!zz_check_env_vars(&env_vars1))) goto end;

	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars1, _R("PATH"), &contains))) goto end;
	if (RT_UNLIKELY(!contains)) goto end;

	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars1, _R("pATH"), &contains))) goto end;
#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!contains)) goto end;
#else
	if (RT_UNLIKELY(contains)) goto end;
#endif

	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars1, _R("RT_VAR_NAME"), &contains))) goto end;
	if (RT_UNLIKELY(contains)) goto end;
	buffer[0] = _R('a');
	buffer[1] = 0;
	if (RT_UNLIKELY(rt_env_vars_get_env_var(&env_vars1, _R("RT_VAR_NAME"), buffer, 256, &buffer_size))) goto end;
	if (RT_UNLIKELY(buffer_size)) goto end;
	if (RT_UNLIKELY(buffer[0])) goto end;

	/* Add a variable to process environment. */
	if (RT_UNLIKELY(!rt_env_var_set(_R("RT_VAR_NAME"), _R("VALUE")))) goto end;

	/* Check process environment */
	if (RT_UNLIKELY(!rt_env_var_get(_R("RT_VAR_NAME"), buffer, 256, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_equals(_R("VALUE"), rt_char_get_size(_R("VALUE")), buffer, buffer_size))) goto end;

	/* Check that it is not added to env_vars1. */
	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars1, _R("RT_VAR_NAME"), &contains))) goto end;
	if (RT_UNLIKELY(contains)) goto end;
	if (RT_UNLIKELY(!zz_check_env_vars(&env_vars1))) goto end;

	/* Create env_vars2 with the variable. */
	if (RT_UNLIKELY(!rt_env_vars_create(&env_vars2))) goto end;
	env_vars2_created = RT_TRUE;
	if (RT_UNLIKELY(!zz_check_env_vars(&env_vars2))) goto end;

	/* Check that the new variable is in env_vars2. */
	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars2, _R("RT_VAR_NAME"), &contains))) goto end;
	if (RT_UNLIKELY(!contains)) goto end;
	if (RT_UNLIKELY(!rt_env_vars_get_env_var(&env_vars2, _R("RT_VAR_NAME"), buffer, 256, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_equals(_R("VALUE"), rt_char_get_size(_R("VALUE")), buffer, buffer_size))) goto end;

	/* Remove the variable. */
	if (RT_UNLIKELY(!rt_env_var_delete(_R("RT_VAR_NAME")))) goto end;

	/* Check process environment */
	if (RT_UNLIKELY(rt_env_var_get(_R("RT_VAR_NAME"), buffer, 256, &buffer_size))) goto end;

	/* Check that the variable is still in env_vars2. */
	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars2, _R("RT_VAR_NAME"), &contains))) goto end;
	if (RT_UNLIKELY(!contains)) goto end;

	/* Create env_vars3 without the variable. */
	if (RT_UNLIKELY(!rt_env_vars_create(&env_vars3))) goto end;
	env_vars3_created = RT_TRUE;
	if (RT_UNLIKELY(!zz_check_env_vars(&env_vars3))) goto end;

	/* Check that the variable is not in env_vars3. */
	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars3, _R("RT_VAR_NAME"), &contains))) goto end;
	if (RT_UNLIKELY(contains)) goto end;

	/* Add RT_VAR_NAME1 to env_vars3. */
	if (RT_UNLIKELY(!rt_env_vars_add_env_var(&env_vars3, _R("RT_VAR_NAME1"), _R("value1")))) goto end;
	if (RT_UNLIKELY(!zz_check_env_vars(&env_vars3))) goto end;
	if (RT_UNLIKELY(!rt_env_vars_get_env_var(&env_vars3, _R("RT_VAR_NAME1"), buffer, 256, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_equals(_R("value1"), rt_char_get_size(_R("value1")), buffer, buffer_size))) goto end;

	/* Add RT_VAR_NAME2 to env_vars3. */
	if (RT_UNLIKELY(!rt_env_vars_add_env_var(&env_vars3, _R("RT_VAR_NAME2"), _R("")))) goto end;
	if (RT_UNLIKELY(!zz_check_env_vars(&env_vars3))) goto end;
	if (RT_UNLIKELY(!rt_env_vars_get_env_var(&env_vars3, _R("RT_VAR_NAME2"), buffer, 256, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_equals(_R(""), rt_char_get_size(_R("")), buffer, buffer_size))) goto end;

	/* Remove PATH from env_vars3. */
	if (RT_UNLIKELY(!rt_env_vars_remove_env_var(&env_vars3, _R("PATH")))) goto end;
	if (RT_UNLIKELY(!zz_check_env_vars(&env_vars3))) goto end;
	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars3, _R("PATH"), &contains))) goto end;
	if (RT_UNLIKELY(contains)) goto end;
	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars3, _R("RT_VAR_NAME2"), &contains))) goto end;
	if (RT_UNLIKELY(!contains)) goto end;

	/* Remove RT_VAR_NAME2 from env_vars3. */
	if (RT_UNLIKELY(!rt_env_vars_remove_env_var(&env_vars3, _R("RT_VAR_NAME2")))) goto end;
	if (RT_UNLIKELY(!zz_check_env_vars(&env_vars3))) goto end;
	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars3, _R("RT_VAR_NAME2"), &contains))) goto end;
	if (RT_UNLIKELY(contains)) goto end;

	/* Add variable using merge. */
	if (RT_UNLIKELY(!rt_env_vars_merge_env_var(&env_vars3, _R("RT_VAR_NAME3"), _R("value3")))) goto end;
	if (RT_UNLIKELY(!rt_env_vars_get_env_var(&env_vars3, _R("RT_VAR_NAME3"), buffer, 256, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_equals(_R("value3"), rt_char_get_size(_R("value3")), buffer, buffer_size))) goto end;

	/* Replace variable using merge. */
	if (RT_UNLIKELY(!rt_env_vars_merge_env_var(&env_vars3, _R("RT_VAR_NAME3"), _R("This is a variable value")))) goto end;
	if (RT_UNLIKELY(!rt_env_vars_get_env_var(&env_vars3, _R("RT_VAR_NAME3"), buffer, 256, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_equals(_R("This is a variable value"), rt_char_get_size(_R("This is a variable value")), buffer, buffer_size))) goto end;

	ret = RT_OK;
end:
	if (env_vars3_created) {
		if (RT_UNLIKELY(!rt_env_vars_free(&env_vars3)))
			ret = RT_FAILED;
	}
	if (env_vars2_created) {
		if (RT_UNLIKELY(!rt_env_vars_free(&env_vars2)))
			ret = RT_FAILED;
	}
	if (env_vars1_created) {
		if (RT_UNLIKELY(!rt_env_vars_free(&env_vars1)))
			ret = RT_FAILED;
	}

	return ret;
}

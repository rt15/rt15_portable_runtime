#include <rpr.h>

/**
 * Check the array against the block.
 */
static rt_s zz_check_env_vars(struct rt_env_vars *env_vars)
{
	rt_char *env_vars_block;
	rt_char **env_vars_array;
	rt_un i;
	rt_s ret;

	if (RT_UNLIKELY(!rt_env_vars_get_block(env_vars, &env_vars_block))) goto error;
	if (RT_UNLIKELY(!rt_env_vars_get_array(env_vars, &env_vars_array))) goto error;

	if (!env_vars_block[0]) {
		if (RT_UNLIKELY(env_vars_array[0]))
			goto error;
	} else {
		i = 0;
		while (RT_TRUE) {
			if (RT_UNLIKELY(!rt_char_equals(env_vars_array[i], rt_char_get_size(env_vars_array[i]), env_vars_block, rt_char_get_size(env_vars_block))))
				goto error;

			while (*env_vars_block) {
				env_vars_block++;
			}
			env_vars_block++;
			i++;

			/* End of array? */
			if (!env_vars_array[i]) {
				/* Should be also the end of the block. */
				if (RT_UNLIKELY(*env_vars_block))
					goto error;
				break;
			}
		}
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
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
	rt_s ret;

	if (RT_UNLIKELY(!rt_env_vars_create(&env_vars1))) goto error;
	env_vars1_created = RT_TRUE;
	if (RT_UNLIKELY(!zz_check_env_vars(&env_vars1))) goto error;

	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars1, _R("PATH"), &contains))) goto error;
	if (RT_UNLIKELY(!contains)) goto error;

	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars1, _R("pATH"), &contains))) goto error;
#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!contains)) goto error;
#else
	if (RT_UNLIKELY(contains)) goto error;
#endif

	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars1, _R("RT_VAR_NAME"), &contains))) goto error;
	if (RT_UNLIKELY(contains)) goto error;
	buffer[0] = _R('a');
	buffer[1] = 0;
	if (RT_UNLIKELY(rt_env_vars_get_env_var(&env_vars1, _R("RT_VAR_NAME"), buffer, 256, &buffer_size))) goto error;
	if (RT_UNLIKELY(buffer_size)) goto error;
	if (RT_UNLIKELY(buffer[0])) goto error;

	/* Add a variable to process environment. */
	if (RT_UNLIKELY(!rt_env_var_set(_R("RT_VAR_NAME"), _R("VALUE")))) goto error;

	/* Check process environment */
	if (RT_UNLIKELY(!rt_env_var_get(_R("RT_VAR_NAME"), buffer, 256, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_char_equals(_R("VALUE"), rt_char_get_size(_R("VALUE")), buffer, buffer_size))) goto error;

	/* Check that it is not added to env_vars1. */
	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars1, _R("RT_VAR_NAME"), &contains))) goto error;
	if (RT_UNLIKELY(contains)) goto error;
	if (RT_UNLIKELY(!zz_check_env_vars(&env_vars1))) goto error;

	/* Create env_vars2 with the variable. */
	if (RT_UNLIKELY(!rt_env_vars_create(&env_vars2))) goto error;
	env_vars2_created = RT_TRUE;
	if (RT_UNLIKELY(!zz_check_env_vars(&env_vars2))) goto error;

	/* Check that the new variable is in env_vars2. */
	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars2, _R("RT_VAR_NAME"), &contains))) goto error;
	if (RT_UNLIKELY(!contains)) goto error;
	if (RT_UNLIKELY(!rt_env_vars_get_env_var(&env_vars2, _R("RT_VAR_NAME"), buffer, 256, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_char_equals(_R("VALUE"), rt_char_get_size(_R("VALUE")), buffer, buffer_size))) goto error;

	/* Remove the variable. */
	if (RT_UNLIKELY(!rt_env_var_delete(_R("RT_VAR_NAME")))) goto error;

	/* Check process environment */
	if (RT_UNLIKELY(rt_env_var_get(_R("RT_VAR_NAME"), buffer, 256, &buffer_size))) goto error;

	/* Check that the variable is still in env_vars2. */
	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars2, _R("RT_VAR_NAME"), &contains))) goto error;
	if (RT_UNLIKELY(!contains)) goto error;

	/* Create env_vars3 without the variable. */
	if (RT_UNLIKELY(!rt_env_vars_create(&env_vars3))) goto error;
	env_vars3_created = RT_TRUE;
	if (RT_UNLIKELY(!zz_check_env_vars(&env_vars3))) goto error;

	/* Check that the variable is not in env_vars3. */
	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars3, _R("RT_VAR_NAME"), &contains))) goto error;
	if (RT_UNLIKELY(contains)) goto error;

	/* Add RT_VAR_NAME1 to env_vars3. */
	if (RT_UNLIKELY(!rt_env_vars_add_env_var(&env_vars3, _R("RT_VAR_NAME1"), _R("value1")))) goto error;
	if (RT_UNLIKELY(!zz_check_env_vars(&env_vars3))) goto error;
	if (RT_UNLIKELY(!rt_env_vars_get_env_var(&env_vars3, _R("RT_VAR_NAME1"), buffer, 256, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_char_equals(_R("value1"), rt_char_get_size(_R("value1")), buffer, buffer_size))) goto error;

	/* Add RT_VAR_NAME2 to env_vars3. */
	if (RT_UNLIKELY(!rt_env_vars_add_env_var(&env_vars3, _R("RT_VAR_NAME2"), _R("")))) goto error;
	if (RT_UNLIKELY(!zz_check_env_vars(&env_vars3))) goto error;
	if (RT_UNLIKELY(!rt_env_vars_get_env_var(&env_vars3, _R("RT_VAR_NAME2"), buffer, 256, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_char_equals(_R(""), rt_char_get_size(_R("")), buffer, buffer_size))) goto error;

	/* Remove PATH from env_vars3. */
	if (RT_UNLIKELY(!rt_env_vars_remove_env_var(&env_vars3, _R("PATH")))) goto error;
	if (RT_UNLIKELY(!zz_check_env_vars(&env_vars3))) goto error;
	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars3, _R("PATH"), &contains))) goto error;
	if (RT_UNLIKELY(contains)) goto error;
	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars3, _R("RT_VAR_NAME2"), &contains))) goto error;
	if (RT_UNLIKELY(!contains)) goto error;

	/* Remove RT_VAR_NAME2 from env_vars3. */
	if (RT_UNLIKELY(!rt_env_vars_remove_env_var(&env_vars3, _R("RT_VAR_NAME2")))) goto error;
	if (RT_UNLIKELY(!zz_check_env_vars(&env_vars3))) goto error;
	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(&env_vars3, _R("RT_VAR_NAME2"), &contains))) goto error;
	if (RT_UNLIKELY(contains)) goto error;

	/* Add variable using merge. */
	if (RT_UNLIKELY(!rt_env_vars_merge_env_var(&env_vars3, _R("RT_VAR_NAME3"), _R("value3")))) goto error;
	if (RT_UNLIKELY(!rt_env_vars_get_env_var(&env_vars3, _R("RT_VAR_NAME3"), buffer, 256, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_char_equals(_R("value3"), rt_char_get_size(_R("value3")), buffer, buffer_size))) goto error;

	/* Replace variable using merge. */
	if (RT_UNLIKELY(!rt_env_vars_merge_env_var(&env_vars3, _R("RT_VAR_NAME3"), _R("This is a variable value")))) goto error;
	if (RT_UNLIKELY(!rt_env_vars_get_env_var(&env_vars3, _R("RT_VAR_NAME3"), buffer, 256, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_char_equals(_R("This is a variable value"), rt_char_get_size(_R("This is a variable value")), buffer, buffer_size))) goto error;

	ret = RT_OK;
free:
	if (env_vars3_created) {
		env_vars3_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_env_vars_free(&env_vars3) && ret))
			goto error;
	}
	if (env_vars2_created) {
		env_vars2_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_env_vars_free(&env_vars2) && ret))
			goto error;
	}
	if (env_vars1_created) {
		env_vars1_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_env_vars_free(&env_vars1) && ret))
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

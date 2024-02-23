#include "layer005/rt_env_vars.h"

#include "layer001/rt_memory.h"
#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer003/rt_char.h"
#include "layer004/rt_os_version.h"
#include "layer004/rt_static_heap.h"

rt_s rt_env_vars_create(struct rt_env_vars *env_vars)
{
	rt_un block_size;
#ifdef RT_DEFINE_WINDOWS
	rt_char *windows_env_vars_block;
	rt_char *library_env_vars_block;
	rt_b vista_or_earlier;
#else
	rt_char **env_vars_array;
	rt_un buffer_size;
	rt_char *env_vars_block;
#endif
	rt_s ret;

	/* Makes sure that we can call rt_env_vars_free in the error handler without issue. */
	env_vars->env_vars_array = RT_NULL;

#ifdef RT_DEFINE_WINDOWS

	/* Makes sure that the error handler will free the Windows block if needed (we will initialize it next line). */
	env_vars->windows_block = RT_TRUE;

	/* GetEnvironmentStrings returns null in case of error. */
	env_vars->env_vars_block = GetEnvironmentStrings();
	if (RT_UNLIKELY(!env_vars->env_vars_block)) {
		/* GetEnvironmentStrings is not supposed to set last error. */
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto error;
	}

	/* In Vista 6.0.6001.18631 kernel32.dll disassembly, GetEnvironmentStrings returns a copy of the environment variables block. */
	if (RT_UNLIKELY(!rt_os_version_is_greater_or_equal_to(6, 0, 0, &vista_or_earlier)))
		goto error;
	if (!vista_or_earlier) {
		/* GetEnvironmentStrings return direct pointer on the environment on old (At least XP) Windows versions. */
		/* As a result the block is updated when a variable is added to the environment. */
		/* To avoid that, we copy the block if we are not under Vista. */

		windows_env_vars_block = env_vars->env_vars_block;

		/* Computing the block size to allocate space for a copy. */
		block_size = 0;
		while (RT_TRUE) {
			/* Find next zero character. */
			while (windows_env_vars_block[block_size]) {
				block_size++;
			}

			/* Count terminating zero. */
			block_size++;

			/* Quit if next one is zero too. */
			if (!windows_env_vars_block[block_size])
				break;
		}
		/* Count terminating zero. */
		block_size++;

		if (RT_UNLIKELY(!rt_static_heap_alloc((void**)&library_env_vars_block, block_size * sizeof(rt_char)))) {
			/* The Windows environment block will be free by rt_env_vars_free in the error handler. */
			goto error;
		}

		env_vars->windows_block = RT_FALSE;
		env_vars->env_vars_block = library_env_vars_block;
		RT_MEMORY_COPY(windows_env_vars_block, library_env_vars_block, block_size * sizeof(rt_char));

		/* We just replaced the Windows environment block in the struct, so it is now our responsibility to free it. */
		/* Returns zero and set last error in case of issue. */
		if (RT_UNLIKELY(!FreeEnvironmentStrings(windows_env_vars_block))) {
			/* The library environment block will be free by rt_env_vars_free in the error handler. */
			goto error;
		}
	}

#else

	/* Makes sure that we can call rt_env_vars_free in the error handler without issue. */
	env_vars->env_vars_block = RT_NULL;

	/* __environ point on a null terminated array of pointers to zero terminated variables. */
	env_vars_array = __environ;
	if (!env_vars_array || !*env_vars_array) {
		/* No variables, just put 2 zeros. */
		if (RT_UNLIKELY(!rt_static_heap_alloc((void**)&env_vars->env_vars_block, 2 * sizeof(rt_char))))
			goto error;
		env_vars->env_vars_block[0] = 0;
		env_vars->env_vars_block[1] = 0;
	} else {
		/* Compute the block size from the array of variables. */
		block_size = 0;
		while (*env_vars_array) {
			block_size += rt_char_get_size(*env_vars_array);
			block_size++; /* Zero character for the variable. */
			env_vars_array++;
		}
		/* Zero character after the last string terminating zero character. */
		block_size++;

		if (RT_UNLIKELY(!rt_static_heap_alloc((void**)&env_vars->env_vars_block, block_size * sizeof(rt_char))))
			goto error;

		/* Notice that __environ might have been modified by another thread. Environment is not thread safe under Linux. */
		/* If the environment is now bigger, their will be an error copying the strings. */
		/* If it is smaller, then the allocated block is too big but that has no impact. */
		env_vars_array = __environ;
		if (!env_vars_array || !*env_vars_array) {
			/* There is at least room for one environment variable, so we can safely write two characters. */
			env_vars->env_vars_block[0] = 0;
			env_vars->env_vars_block[1] = 0;
		} else {
			env_vars_block = env_vars->env_vars_block;

			buffer_size = 0;
			while (*env_vars_array) {
				if (RT_UNLIKELY(!rt_char_append(*env_vars_array, rt_char_get_size(*env_vars_array), env_vars_block, block_size, &buffer_size)))
					goto error;

				/* We need to keep the terminating zero. */
				buffer_size++;

				/* Switch to next variable. */
				env_vars_array++;
			}
			buffer_size--; /* We will overwrite last zero and rt_char_append_char will add one, checking the buffer capacity. */
			if (RT_UNLIKELY(!rt_char_append_char(_R('\0'), env_vars_block, block_size, &buffer_size)))
				goto error;
		}
	}
#endif

	ret = RT_OK;
free:
	return ret;

error:
	rt_env_vars_free(env_vars);
	ret = RT_FAILED;
	goto free;
}

rt_s rt_env_vars_get_block(struct rt_env_vars *env_vars, rt_char **env_vars_block)
{
	*env_vars_block = env_vars->env_vars_block;
	return RT_OK;
}

rt_s rt_env_vars_get_array(struct rt_env_vars *env_vars, rt_char ***env_vars_array)
{
	rt_char *env_vars_block;
	rt_un vars_count;
	rt_un i;
	rt_s ret;

	/* We only build the array if it not already there. */
	if (!env_vars->env_vars_array) {
		env_vars_block = env_vars->env_vars_block;

		/* Count the variables in the block. */
		vars_count = 0;
		if (*env_vars_block) {
			while (RT_TRUE) {
				vars_count++;

				/* Find next zero character. */
				while (*env_vars_block) {
					env_vars_block++;
				}

				/* Quit if next one is zero too. */
				env_vars_block++;
				if (!*env_vars_block)
					break;
			}
		}

		/* Alloc variables count plus one for the terminating null. */
		if (RT_UNLIKELY(!rt_static_heap_alloc((void**)&env_vars->env_vars_array, (vars_count + 1) * sizeof(rt_char*))))
			goto error;

		if (vars_count) {
			/* Build the array of pointer by pointing into the block. */
			env_vars_block = env_vars->env_vars_block;
			i = 0;
			while (RT_TRUE) {
				env_vars->env_vars_array[i] = env_vars_block;
				i++;

				/* Find next zero character. */
				while (*env_vars_block) {
					env_vars_block++;
				}

				/* Quit if next one is zero too. */
				env_vars_block++;
				if (!*env_vars_block) {
					env_vars->env_vars_array[i] = RT_NULL;
					break;
				}

			}
		} else {
			/* We have allocated the size of the terminating null. */
			env_vars->env_vars_array[0] = RT_NULL;
		}
	}

	*env_vars_array = env_vars->env_vars_array;
	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

/**
 * Find a pointer on <tt>env_var_name</tt> in <tt>env_vars->env_vars_block</tt>.<br>
 * Point at the string <tt>variable=value</tt>.
 */
static rt_s rt_env_vars_get_pointer(struct rt_env_vars *env_vars, const rt_char *env_var_name, rt_char **result)
{
#ifdef RT_DEFINE_WINDOWS
	rt_char local_env_var_name[RT_CHAR_HALF_BIG_STRING_SIZE];
#else
	const rt_char *local_env_var_name;
#endif
	rt_un local_env_var_name_size = rt_char_get_size(env_var_name);
	rt_char *env_vars_block;
	rt_char current_env_var_name[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un i;
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	/* Build upper case variable name under Windows. */
	if (RT_UNLIKELY(!rt_char_copy(env_var_name, local_env_var_name_size, local_env_var_name, RT_CHAR_HALF_BIG_STRING_SIZE)))
		goto error;
	rt_char_fast_upper(local_env_var_name);
#else
	local_env_var_name = env_var_name;
#endif

	env_vars_block = env_vars->env_vars_block;
	if (!*env_vars_block) {
		/* Empty block. */
		*result = RT_NULL;
	} else {
		while (RT_TRUE) {
			i = 0;

			/* Find next equals character. */
			while (env_vars_block[i] != _R('=') && env_vars_block[i]) {
				i++;
			}

#ifdef RT_DEFINE_WINDOWS
			/* There are variable names starting with '=' under Windows. */
			if (env_vars_block[i] == _R('=') && !i) {
				/* Find next equals character. */
				i++;
				while (env_vars_block[i] != _R('=') && env_vars_block[i]) {
					i++;
				}
			}
#endif

			/* Zero before equals. Something went wrong. */
			if (RT_UNLIKELY(!env_vars_block[i])) {
				rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
				goto error;
			}

			/* Copy the variable name for later comparison. */
			if (RT_UNLIKELY(!rt_char_copy(env_vars_block, i, current_env_var_name, RT_CHAR_HALF_BIG_STRING_SIZE)))
				goto error;

#ifdef RT_DEFINE_WINDOWS
			/* Build upper case variable name under Windows. */
			rt_char_fast_upper(current_env_var_name);
#endif
			/* Check if we found the variable. */
			if (rt_char_equals(current_env_var_name, i, local_env_var_name, local_env_var_name_size)) {
				*result = env_vars_block;
				break;
			}

			/* Find next zero character, starting after the equal sign. */
			env_vars_block = &env_vars_block[i + 1];
			while (*env_vars_block) {
				env_vars_block++;
			}

			/* Quit if next one is zero too. */
			env_vars_block++;
			if (!*env_vars_block) {
				/* Variable not found. */
				*result = RT_NULL;
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

rt_s rt_env_vars_contains_env_var(struct rt_env_vars *env_vars, const rt_char *env_var_name, rt_b *contains)
{
	rt_char *env_var;
	rt_s ret;

	if (RT_UNLIKELY(!rt_env_vars_get_pointer(env_vars, env_var_name, &env_var)))
		goto error;
	if (env_var) {
		*contains = RT_TRUE;
	} else {
		*contains = RT_FALSE;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_env_vars_get_env_var(struct rt_env_vars *env_vars, const rt_char *env_var_name, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_char *env_var;
	rt_char *env_var_value;
	rt_s ret;

	if (RT_UNLIKELY(!rt_env_vars_get_pointer(env_vars, env_var_name, &env_var)))
		goto error;
	if (RT_UNLIKELY(!env_var)) {
		/* Variable not found. */
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	/* Point after the name and '='. */
	env_var_value = &env_var[rt_char_get_size(env_var_name) + 1];

	/* Copy the value. */
	*buffer_size = rt_char_get_size(env_var_value);
	if (RT_UNLIKELY(!rt_char_copy(env_var_value, *buffer_size, buffer, buffer_capacity)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	if (buffer_capacity)
		buffer[0] = 0;
	*buffer_size = 0;
	ret = RT_FAILED;
	goto free;
}

rt_s rt_env_vars_remove_env_var(struct rt_env_vars *env_vars, const rt_char *env_var_name)
{
	rt_char *env_var;
	rt_char *destination;
	rt_b was_null;
	rt_s ret;

	/* The variable might be present multiple times with different cases. */
	while (RT_TRUE) {

		/* Search the variable to delete. */
		if (RT_UNLIKELY(!rt_env_vars_get_pointer(env_vars, env_var_name, &env_var)))
			goto error;

		/* Remove only if it exists. */
		if (env_var) {

			/* The array will be built back if and when needed. */
			if (RT_UNLIKELY(!rt_static_heap_free((void**)&env_vars->env_vars_array)))
				goto error;

			/* We will copy the remaining of the block in place of the variable to remove. */
			destination = env_var;

			/* Skip the variable name and value. */
			while (*env_var)
				env_var++;
			/* Skip the variable terminating zero. */
			env_var++;

			was_null = RT_TRUE;
			while (RT_TRUE) {
				if (!*env_var) {
					if (was_null) {
						/* Two zeros, we are at the end of the block. Add the second terminating zero. */
						*destination = 0;
						break;
					}
					was_null = RT_TRUE;
				} else {
					was_null = RT_FALSE;
				}
				*destination = *env_var;
				destination++;
				env_var++;
			}
		} else {
			/* Variable not found. */
			break;
		}
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_env_vars_add_env_var(struct rt_env_vars *env_vars, const rt_char *env_var_name, const rt_char *value)
{
	rt_char *old_env_vars_block;
	rt_char *new_env_vars_block;
	rt_un old_env_vars_block_size;
	rt_un new_env_vars_block_size;
	rt_un output_size;
	rt_un env_var_name_size = rt_char_get_size(env_var_name);
	rt_un value_size = rt_char_get_size(value);
	rt_s ret;

	/* Will be free in case of error, if different from old_env_vars_block. */
	new_env_vars_block = RT_NULL;

	/* We keep the old block even in case of error. */
	old_env_vars_block = env_vars->env_vars_block;

	/* The array will be built back if and when needed. */
	if (RT_UNLIKELY(!rt_static_heap_free((void**)&env_vars->env_vars_array)))
		goto error;

	/* Compute the size of the existing block. */
	old_env_vars_block_size = 0;
	while (RT_TRUE) {
		/* Find next zero character. */
		while (old_env_vars_block[old_env_vars_block_size]) {
			old_env_vars_block_size++;
		}

		/* Quit if next one is zero too. */
		old_env_vars_block_size++;
		if (!old_env_vars_block[old_env_vars_block_size])
			break;
	}
	/* Count terminating zero. */
	old_env_vars_block_size++;

	/* Compute the size of the future block (existing block size plus room for the new variable. */
	new_env_vars_block_size = old_env_vars_block_size;
	new_env_vars_block_size += env_var_name_size;
	new_env_vars_block_size++; /* Equals. */
	new_env_vars_block_size += value_size;
	new_env_vars_block_size++; /* Terminating zero. */

	/* Copy the existing block into the new block. */
#ifdef RT_DEFINE_WINDOWS
	if (env_vars->windows_block) {
		if (RT_UNLIKELY(!rt_static_heap_alloc((void**)&new_env_vars_block, new_env_vars_block_size * sizeof(rt_char))))
			goto error;
		RT_MEMORY_COPY(old_env_vars_block, new_env_vars_block, old_env_vars_block_size * sizeof(rt_char));
	} else {
		new_env_vars_block = old_env_vars_block;
		if (RT_UNLIKELY(!rt_static_heap_realloc((void**)&new_env_vars_block, new_env_vars_block_size * sizeof(rt_char))))
			goto error;
	}
#else
	new_env_vars_block = old_env_vars_block;
	if (RT_UNLIKELY(!rt_static_heap_realloc((void**)&new_env_vars_block, new_env_vars_block_size * sizeof(rt_char))))
		goto error;
#endif

	if (old_env_vars_block_size == 2) {
		/* There was no variables. */
		output_size = 0;
	} else {
		/* Append the new variable at the end of the block, overwriting the second terminating zero. */
		output_size = old_env_vars_block_size - 1;
	}
	if (RT_UNLIKELY(!rt_char_append(env_var_name, env_var_name_size, new_env_vars_block, new_env_vars_block_size, &output_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append_char(_R('='), new_env_vars_block, new_env_vars_block_size, &output_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(value, value_size, new_env_vars_block, new_env_vars_block_size, &output_size))) goto error;

	/* Append the second terminating zero. */
	if (RT_UNLIKELY(!rt_char_append_char(_R('\0'), new_env_vars_block, new_env_vars_block_size, &output_size)))
		goto error;

#ifdef RT_DEFINE_WINDOWS
	if (env_vars->windows_block) {
		/* The block is now necessarily managed by us. */
		env_vars->windows_block = RT_FALSE;

		/* Free Windows block. */
		/* Returns zero and set last error in case of issue. */
		if (RT_UNLIKELY(!FreeEnvironmentStrings(env_vars->env_vars_block))) {
			env_vars->env_vars_block = RT_NULL;
			goto error;
		}
	}
#endif

	env_vars->env_vars_block = new_env_vars_block;

	ret = RT_OK;
free:
	return ret;

error:
	/* If it is not a realloc. */
	if (new_env_vars_block != old_env_vars_block) {
		rt_static_heap_free((void**)&new_env_vars_block);
	}
	ret = RT_FAILED;
	goto free;
}

rt_s rt_env_vars_merge_env_var(struct rt_env_vars *env_vars, const rt_char *env_var_name, const rt_char *value)
{
	rt_b contains;
	rt_s ret;

	if (RT_UNLIKELY(!rt_env_vars_contains_env_var(env_vars, env_var_name, &contains)))
		goto error;
	if (contains) {
		if (RT_UNLIKELY(!rt_env_vars_remove_env_var(env_vars, env_var_name)))
			goto error;
	}
	if (RT_UNLIKELY(!rt_env_vars_add_env_var(env_vars, env_var_name, value)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_env_vars_free(struct rt_env_vars *env_vars)
{
	rt_s ret;

	ret = RT_OK;

	/* On Windows, env_vars_block might have been allocated by Windows. */
#ifdef RT_DEFINE_WINDOWS
	if (env_vars->windows_block) {
		/* env_vars_block allocated by Windows */
		if (env_vars->env_vars_block) {
			/* Returns zero and set last error in case of issue. */
			if (!FreeEnvironmentStrings(env_vars->env_vars_block))
				ret = RT_FAILED;
			env_vars->env_vars_block = RT_NULL;
		}
	} else {
		/* env_vars_block allocated by the library. */
		if (!(rt_static_heap_free((void**)&env_vars->env_vars_block)))
			ret = RT_FAILED;
	}
#else
	if (!(rt_static_heap_free((void**)&env_vars->env_vars_block)))
		ret = RT_FAILED;
#endif

	if (!(rt_static_heap_free((void**)&env_vars->env_vars_array)))
		ret = RT_FAILED;

	return ret;
}

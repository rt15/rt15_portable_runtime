#ifndef RT_ENV_VARS_H
#define RT_ENV_VARS_H

#include "layer000/rt_types.h"

/**
 * @file
 * Set of environment variables.
 *
 * <p>
 * The main purpose of this structure is to be used as argument in rt_process_create.<br>
 * Corresponds to <tt>__environ</tt> Linux variable and <tt>GetEnvironmentStrings</tt> in Windows.<br>
 * Manage a copy of the environment so variables are added/removed only in given rt_env_vars structure, not in the process environment.
 * </p>
 *
 * <p>
 * The variables can be retrieved in 2 structures:
 * </p>
 * <ul>
 *   <li>The Windows structure with rt_env_vars_get_block (Concatenated zero terminated strings with a terminating zero at the end).</li
 *   <li>The Linux structure with rt_env_vars_get_array (Null terminated array of pointers to zero terminated strings).</li>
 * </ul>
 *
 * <p>
 * Environment variables are case sensitive under Linux but not under Windows.
 * </p>
 *
 * <p>
 * Maximum environment variable names size is <tt>RT_CHAR_HALF_BIG_STRING_SIZE</tt>.
 * </p>
 *
 * <p>
 * While parsing the variables, beware that Windows defines some variables starting with equals like <tt>=ExitCode=00000000</tt>.<br>
 * These variables are not visible using <tt>set</tt> command line and might be there for DOS compatibility reasons.
 * </p>
 */

struct rt_env_vars {
	/* Windows like concatenated zero terminated strings with 2 zeros after the last string. */
	/* Cannot be RT_NULL but can contain two zeros if there are no variables. */
	rt_char *env_vars_block;
	/* Linux like null terminated array of zero terminated strings. */
	rt_char **env_vars_array;
#ifdef RT_DEFINE_WINDOWS
	/* Whether env_vars_block has been allocated by Windows or this library. */
	rt_b windows_block;
#endif
};

/**
 * Initialize <tt>env_vars</tt>, with the process environment variables.
 *
 * <p>
 * Do not forget to call <tt>rt_env_vars_free</tt>.
 * </p>
 */
RT_API rt_s rt_env_vars_create(struct rt_env_vars *env_vars);

RT_API rt_s rt_env_vars_get_block(struct rt_env_vars *env_vars, rt_char **env_vars_block);

RT_API rt_s rt_env_vars_get_array(struct rt_env_vars *env_vars, rt_char ***env_vars_array);

/**
 * Find whether a variable exists in <tt>env_vars</tt>.
 */
RT_API rt_s rt_env_vars_contains_env_var(struct rt_env_vars *env_vars, const rt_char *env_var_name, rt_b *contains);

/**
 * Return the value of <tt>env_var_name</tt>.
 *
 * <p>
 * Fails if the variable does not exist.<br>
 * In case of error, buffer starts with a zero character.
 * </p>
 *
 * @param buffer_size Out parameter.
 */
RT_API rt_s rt_env_vars_get_env_var(struct rt_env_vars *env_vars, const rt_char *env_var_name, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * Be sure that the variable does not exist yet.<br>
 * You can use <tt>rt_env_vars_remove_env_var</tt> to ensure that.
 *
 * <p>
 * The <tt>rt_env_vars</tt> structure should be considered corrupted in case of failure.
 * </p>
 */
RT_API rt_s rt_env_vars_add_env_var(struct rt_env_vars *env_vars, const rt_char *env_var_name, const rt_char *value);

/**
 * Remove an environment variable of given <tt>rt_env_vars</tt> structure if it exists.<br>
 * Can be used to ensure that an environment variable does not exist before adding it.
 */
RT_API rt_s rt_env_vars_remove_env_var(struct rt_env_vars *env_vars, const rt_char *env_var_name);

/**
 * If the variable already exists, replace the value.<br>
 * If the variable does not exist, add the variable.
 */
RT_API rt_s rt_env_vars_merge_env_var(struct rt_env_vars *env_vars, const rt_char *env_var_name, const rt_char *value);

RT_API rt_s rt_env_vars_free(struct rt_env_vars *env_vars);

#endif /* RT_ENV_VARS_H */

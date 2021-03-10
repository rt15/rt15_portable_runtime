#ifndef RT_ENV_VAR_H
#define RT_ENV_VAR_H

#include "layer000/rt_types.h"

/**
 * @file
 * Environment variable manipulation.
 *
 * <p>
 * Beware that you cannot modify environment in a thread safe way under Linux.<br>
 * putenv()/setenv()/clearenv()/unsetenv() are not thread safe.<br>
 * Many functions are calling getenv which can crash if the environment is modified.<br>
 * And it is pretty much the same thing under Windows.
 * </p>
 *
 */

/**
 * Fails if the variable does not exist.
 *
 * <p>
 * In case of error, buffer starts with a zero character.
 * </p>
 *
 * @param buffer_capacity Must be less than RT_TYPE_MAX_UN32.
 * @param buffer_size Out parameter.
 */
rt_s rt_env_var_get(const rt_char *name, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 *
 * @param value Must not be RT_NULL.
 */
rt_s rt_env_var_set(const rt_char *name, const rt_char *value);

/**
 *
 */
rt_s rt_env_var_delete(const rt_char *name);

#endif /* RT_ENV_VAR_H */

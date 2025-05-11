#include "layer003/rt_env_var.h"

#include "layer001/rt_memory.h"
#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"

rt_s rt_env_var_get(const rt_char *name, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD returned_value;
#else
	char *returned_value;
	size_t length;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	/* Ensure that the 32 or 64 bits integer will fit into a DWORD. */
	if (RT_UNLIKELY(buffer_capacity > RT_TYPE_MAX_UN32)) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}
	returned_value = GetEnvironmentVariable(name, buffer, (DWORD)buffer_capacity);
	if (RT_UNLIKELY(!returned_value))
		goto end;

	/* If buffer is too small, GetEnvironmentVariable return the required buffer size and buffer is unknown. */
	if (RT_UNLIKELY(returned_value >= buffer_capacity)) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto end;
	}
	*buffer_size = returned_value;

#else
	returned_value = getenv(name);
	if (RT_UNLIKELY(!returned_value)) {
		/* The getenv function might not set errno. The only failure reason might be that the variable is not found. */
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	/* Check buffer size. */
	length = strlen(returned_value);
	if (RT_UNLIKELY(length + 1 > buffer_capacity)) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto end;
	}

	/* Copy result to buffer, including zero character. */
	RT_MEMORY_COPY(returned_value, buffer, length + 1);
	*buffer_size = length;
#endif

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!ret)) {
		if (buffer_capacity)
			buffer[0] = 0;
		*buffer_size = 0;
	}

	return ret;
}

rt_s rt_env_var_set(const rt_char *name, const rt_char *value)
{
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS

	/* Returns FALSE and sets last error in case of failure. */
	if (RT_UNLIKELY(!SetEnvironmentVariable(name, value)))
		goto end;

#else

	/* setenv make a copy of the name and the value while putenv make the process environment point to its argument. */
	/* Third parameter is named overwrite, set to nonzero to overwrite possible existing value. */
	/* Returns -1 and sets errno in case of error. */
	if (RT_UNLIKELY(setenv(name, value, 1) == -1))
		goto end;

#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_env_var_delete(const rt_char *name)
{
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS

	/* Passing RT_NULL as value remove the variable from the process environment. */
	/* Returns FALSE and sets last error in case of failure. */
	if (RT_UNLIKELY(!SetEnvironmentVariable(name, RT_NULL)))
		goto end;

#else

	/* Returns -1 and sets errno in case of error. */
	if (RT_UNLIKELY(unsetenv(name) == -1))
		goto end;

#endif

	ret = RT_OK;
end:
	return ret;
}

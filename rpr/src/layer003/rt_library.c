#include "layer003/rt_library.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"

rt_s rt_library_create(struct rt_library *library, const rt_char *name)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	/* LoadLibrary returns NULL and set last error in case of issue. */
	library->handle = LoadLibrary(name);
	if (!library->handle)
		goto error;
#else
	/* dlopen returns NULL in case of issue, but may not set errno as dlerror can be used to retrieve an error message. */
	errno = 0;
	library->handle = dlopen(name, RTLD_LAZY);
	if (!library->handle) {
		if (!errno) {
			/* dlopen did not set errno. */
			rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		}
		goto error;
	}
#endif
	
	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_library_get_function(struct rt_library *library, const rt_char *name, rt_library_function_t *function)
{
#ifdef RT_DEFINE_WINDOWS
	rt_char8 buffer[3840];
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS

	if (RT_UNLIKELY(!WideCharToMultiByte(CP_ACP, 0, name, -1, buffer, 3840, RT_NULL, RT_NULL)))
		goto error;

	/* GetProcAddress returns NULL and set last error in case of issue. */
	*function = (rt_library_function_t)GetProcAddress(library->handle, buffer);
	if (!*function)
		goto error;
#else
	/* We consider dlsym returning NULL as an issue, which is not the actually the case for symbols. */
	errno = 0;
	*function = (rt_library_function_t)(uintptr_t)dlsym(library->handle, name);
	if (!*function) {
		if (!errno) {
			/* dlsym did not set errno. */
			rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		}
		goto error;
	}
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_library_free(struct rt_library *library)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	/* FreeLibrary returns zero and set last error in case of issue. */
	if (!FreeLibrary(library->handle))
		goto error;
#else
	/* dlclose returns nonzero in case of issue. */
	errno = 0;
	if (dlclose(library->handle)) {
		if (!errno) {
			/* dlclose did not set errno. */
			rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		}
		goto error;
	}
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

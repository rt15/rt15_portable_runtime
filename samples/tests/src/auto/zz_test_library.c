#include <rpr.h>

#ifdef RT_DEFINE_WINDOWS
#define LIBRARY_NAME _R("msvcrt.dll")
#else
#define LIBRARY_NAME "libc.so.6"
#endif

typedef char* (RT_CDECL *zz_test_library_strcpy_t)(char *destination, const char *source);

rt_s zz_test_library(void)
{
	struct rt_library library;
	rt_b library_created = RT_FALSE;
	zz_test_library_strcpy_t function;
	rt_char8 buffer[64];
	rt_s ret;

	if (RT_UNLIKELY(!rt_library_create(&library, LIBRARY_NAME))) goto error;
	library_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_library_get_function(&library, _R("strcpy"), (rt_library_function_t*)&function))) goto error;

	function(buffer, "Hello, World!");

	if (RT_UNLIKELY(!rt_char8_equals(buffer, rt_char8_get_size(buffer), "Hello, World!", 13))) goto error;

	ret = RT_OK;
free:
	if (library_created) {
		library_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_library_free(&library) && ret))
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

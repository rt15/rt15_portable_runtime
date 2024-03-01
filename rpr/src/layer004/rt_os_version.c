#include "layer004/rt_os_version.h"

#include "layer001/rt_os_headers.h"
#include "layer003/rt_char.h"
#include "layer003/rt_fast_initialization.h"

static struct rt_fast_initialization rt_os_version_initialization = RT_FAST_INITIALIZATION_STATIC_INIT;

static rt_b rt_os_version_initialization_successful;

/* Save last error while initializing. */
#ifdef RT_DEFINE_WINDOWS
static DWORD rt_os_version_initialization_error;
#else
static int rt_os_version_initialization_error;
#endif

static rt_un rt_os_version_major;
static rt_un rt_os_version_minor;
static rt_un rt_os_version_patch;

#ifdef RT_DEFINE_LINUX
/**
 * Parse the version string under Linux.
 *
 * <p>
 * The version should look like "100", "3.0", "3.2.1".<br>
 * The function stops on the first character that is not a digit or a dot.<br>
 * All three <tt>major</tt>, <tt>minor</tt> and <tt>patch</tt> are set, possibly to zero.
 * </p>
 *
 * @param version Often modified by this function.
 */
static rt_s rt_os_version_parse(rt_char *version, rt_un *major, rt_un *minor, rt_un *patch)
{
	rt_char *start = version;
	rt_char *end = version;
	rt_char end_character;
	rt_s ret;

	*major = 0;
	*minor = 0;
	*patch = 0;

	while ((*end >= _R('0')) && (*end <= _R('9')))
		end++;
	end_character = *end;
	*end = 0;
	if (RT_UNLIKELY(!rt_char_convert_to_un(start, major)))
		goto error;
	if (end_character == _R('.')) {

		/* Go to next version number. */
		start = ++end;

		while ((*end >= _R('0')) && (*end <= _R('9')))
			end++;

		if (start != end) {
			end_character = *end;
			*end = 0;
			if (RT_UNLIKELY(!rt_char_convert_to_un(start, minor)))
				goto error;

			if (end_character == _R('.')) {

				/* Go to next version number. */
				start = ++end;

				while ((*end >= _R('0')) && (*end <= _R('9')))
					end++;

				if (start != end) {
					end_character = *end;
					*end = 0;
					if (RT_UNLIKELY(!rt_char_convert_to_un(start, patch)))
						goto error;
				}
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
#endif

rt_s rt_os_version_get(rt_un *major, rt_un *minor, rt_un *patch)
{
#ifdef RT_DEFINE_WINDOWS
	OSVERSIONINFOEX os_version_info;
#else
	struct utsname uts_name;
	rt_char version[RT_CHAR_HALF_BIG_STRING_SIZE];
#endif
	rt_s ret;

	if (rt_fast_initialization_is_required(&rt_os_version_initialization)) {

#ifdef RT_DEFINE_WINDOWS

		os_version_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		/* Returns zero and sets last error in case of failure. */
#ifdef RT_DEFINE_VC
#pragma warning(disable:4996)
#endif
		if (GetVersionEx((OSVERSIONINFO*)&os_version_info)) {
			rt_os_version_major = os_version_info.dwMajorVersion;
			rt_os_version_minor = os_version_info.dwMinorVersion;
			rt_os_version_patch = os_version_info.wServicePackMajor;
			rt_os_version_initialization_successful = RT_TRUE;
		} else {
			rt_os_version_initialization_error = GetLastError();
			rt_os_version_initialization_successful = RT_FALSE;
		}
#ifdef RT_DEFINE_VC
#pragma warning(default:4996)
#endif

#else
		/* On error, -1 is returned, and errno is set appropriately. */
		if (uname(&uts_name) == -1)
			goto handle_initialization_error;

		/* uts_name.release looks like "4.4.0-66-generic". */
		/* But it might look like "2.6" or "100" too. */

		/* Copy the release name into a buffer that we can modify. */
		if (!rt_char_copy(uts_name.release, rt_char_get_size(uts_name.release), version, RT_CHAR_HALF_BIG_STRING_SIZE))
			goto handle_initialization_error;

		if (!rt_os_version_parse(version, &rt_os_version_major, &rt_os_version_minor, &rt_os_version_patch))
			goto handle_initialization_error;

		rt_os_version_initialization_successful = RT_TRUE;
		goto initialized;

handle_initialization_error:
		rt_os_version_initialization_error = errno;
		rt_os_version_initialization_successful = RT_FALSE;

initialized:

#endif

		rt_fast_initialization_notify_done(&rt_os_version_initialization);
	}

	if (RT_LIKELY(rt_os_version_initialization_successful)) {
		*major = rt_os_version_major;
		if (minor) {
			*minor = rt_os_version_minor;
		}
		if (patch) {
			*patch = rt_os_version_patch;
		}
	} else {
		/* Put saved error as last error. */
#ifdef RT_DEFINE_WINDOWS
		SetLastError(rt_os_version_initialization_error);
#else
		errno = rt_os_version_initialization_error;
#endif
		goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_os_version_is_greater_or_equal_to(rt_un major, rt_un minor, rt_un patch, rt_b *result)
{
	rt_un current_major;
	rt_un current_minor;
	rt_un current_patch;
	rt_s ret;

	if (RT_UNLIKELY(!rt_os_version_get(&current_major, &current_minor, &current_patch)))
		goto error;

	if (current_major > major) {
		*result = RT_TRUE;
	} else if (current_major == major) {
		if (current_minor > minor) {
			*result = RT_TRUE;
		} else if (current_minor == minor) {
			if (current_patch >= patch) {
				*result = RT_TRUE;
			} else {
				*result = RT_FALSE;
			}
		} else {
			*result = RT_FALSE;
		}
	} else {
		*result = RT_FALSE;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

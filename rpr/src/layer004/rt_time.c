#include "layer004/rt_time.h"

#include "layer001/rt_memory.h"
#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer003/rt_fast_initialization.h"

#ifdef RT_DEFINE_WINDOWS

typedef time_t (RT_CDECL *rt_time_time_t)(time_t *unix_time);
typedef rt_n32 (RT_CDECL *rt_time_localtime_s_t)(struct rt_time_info *time_info, const time_t *unix_time);
typedef time_t (RT_CDECL *rt_time_mktime_t)(struct rt_time_info *time_info);

static struct rt_fast_initialization rt_time_initialization = RT_FAST_INITIALIZATION_STATIC_INIT;

/* Whether initialization has been successful or not. */
static rt_b rt_time_initialization_successful;

/* Error code of failed initialization. */
static DWORD rt_time_initialization_error;

static rt_time_time_t rt_time_time;
static rt_time_localtime_s_t rt_time_localtime_s;
static rt_time_mktime_t rt_time_mktime;
#endif

#ifdef RT_DEFINE_WINDOWS

static rt_s rt_time_initialize_do(void)
{
	rt_h library_handle;
	rt_s ret;

	library_handle = LoadLibrary(_R("msvcrt.dll"));
	if (RT_UNLIKELY(!library_handle))
		goto error;

#ifdef RT_DEFINE_GCC
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
	rt_time_time = (rt_time_time_t)GetProcAddress(library_handle, "time");
	if (RT_UNLIKELY(!rt_time_time))
		goto error;

#ifdef RT_DEFINE_64
	rt_time_localtime_s = (rt_time_localtime_s_t)GetProcAddress(library_handle, "_localtime64_s");
#else
	rt_time_localtime_s = (rt_time_localtime_s_t)GetProcAddress(library_handle, "_localtime32_s");
#endif
	if (RT_UNLIKELY(!rt_time_localtime_s))
		goto error;

	rt_time_mktime = (rt_time_mktime_t)GetProcAddress(library_handle, "mktime");
	if (RT_UNLIKELY(!rt_time_mktime))
		goto error;
#ifdef RT_DEFINE_GCC
#pragma GCC diagnostic pop
#endif

	ret = RT_OK;
free:
	return ret;
error:
	rt_time_initialization_error = GetLastError();
	ret = RT_FAILED;
	goto free;
}

static rt_s rt_time_initialize(void)
{
	rt_s ret;

	if (rt_fast_initialization_is_required(&rt_time_initialization)) {
		rt_time_initialization_successful = rt_time_initialize_do();
		rt_fast_initialization_notify_done(&rt_time_initialization);
	}
	if (RT_UNLIKELY(!rt_time_initialization_successful)) {
		/* Set last error as when the initialization has failed. */
		SetLastError(rt_time_initialization_error);
		goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

#endif

rt_s rt_time_get_unix_time(rt_n *unix_time)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS

	if (RT_UNLIKELY(!rt_time_initialize()))
		goto error;

	if (RT_UNLIKELY(rt_time_time(unix_time) == -1))
		goto error;

#else

	if (RT_UNLIKELY(time(unix_time) == -1))
		goto error;

#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_time_info_create_accurate(struct rt_time_info *time_info, rt_un *milliseconds)
{
#ifdef RT_DEFINE_WINDOWS
	SYSTEMTIME system_time;
	TIME_ZONE_INFORMATION time_zone_information;
	DWORD time_zone_id;
#else
	struct timespec time_spec;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	/* This function cannot fail. */
	GetLocalTime(&system_time);

	RT_MEMORY_ZERO(time_info, sizeof(struct rt_time_info));

	time_info->seconds = system_time.wSecond;
	time_info->minutes = system_time.wMinute;
	time_info->hours = system_time.wHour;
	time_info->day = system_time.wDay;
	time_info->month = system_time.wMonth;
	time_info->year = system_time.wYear;
	*milliseconds = system_time.wMilliseconds;

	time_zone_id = GetTimeZoneInformation(&time_zone_information);
	switch (time_zone_id) {
	case TIME_ZONE_ID_STANDARD:
		time_info->daylight_saving_time = 0;
		break;
	case TIME_ZONE_ID_DAYLIGHT:
		time_info->daylight_saving_time = 1;
		break;
	case TIME_ZONE_ID_UNKNOWN:
		time_info->daylight_saving_time = -1;
		break;
	case TIME_ZONE_ID_INVALID:
		/* GetTimeZoneInformation sets last error and returns TIME_ZONE_ID_INVALID in case of error. */
		goto error;
	default:
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto error;
	}

#else
	/* Returns -1 and sets errno in case of issue. */
	if (RT_UNLIKELY(clock_gettime(CLOCK_REALTIME, &time_spec) == -1))
		goto error;

	if (RT_UNLIKELY(!rt_time_info_create_local(time_info, time_spec.tv_sec)))
		goto error;

	*milliseconds = time_spec.tv_nsec / 1000000;

#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_time_info_create_local(struct rt_time_info *time_info, rt_n unix_time)
{
#ifdef RT_DEFINE_WINDOWS
	rt_n32 errno_value;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS

	if (RT_UNLIKELY(!rt_time_initialize()))
		goto error;

	/* Returns 0 in case of success. Returns an errno in case of error. */
	errno_value = rt_time_localtime_s(time_info, &unix_time);
	if (RT_UNLIKELY(errno_value)) {
		rt_error_set_last_from_errno(errno_value);
		goto error;
	}

#else

	/* Returns null and sets errno in case of error. */
	if (RT_UNLIKELY(!localtime_r(&unix_time, (struct tm*)time_info)))
		goto error;

#endif

	time_info->month += 1;
	time_info->year += 1900;
	time_info->day_of_week += 1;
	time_info->day_of_year += 1;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_time_get_unix_time_from_time_info(struct rt_time_info *time_info, rt_n *unix_time)
{
	rt_n result;
	rt_s ret;

	time_info->month -= 1;
	time_info->year -= 1900;
	time_info->day_of_week -= 1;
	time_info->day_of_year -= 1;

#ifdef RT_DEFINE_WINDOWS

	if (RT_UNLIKELY(!rt_time_initialize()))
		goto error;

	/* Returns -1 in case of error. */
	result = rt_time_mktime(time_info);
	if (RT_UNLIKELY(result == -1))
		goto error;

#else

	/* Returns -1 in case of error. */
	result = mktime((struct tm*)time_info);
	if (RT_UNLIKELY(result == -1))
		goto error;

#endif

	time_info->month += 1;
	time_info->year += 1900;
	time_info->day_of_week += 1;
	time_info->day_of_year += 1;

	if (unix_time)
		*unix_time = result;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

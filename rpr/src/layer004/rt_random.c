#include "layer004/rt_random.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer003/rt_fast_initialization.h"

/**
 *
 * <p>
 * Linux notes:<br>
 * If number generator is not initialized, call RAND_poll (Equivalent of srand).
 * </p>
 *
 * <p>
 * Windows notes:<br>
 * RtlGenRandom is not part of public win32 api but it should remain in Windows as it is used by rand_s of current CRT.<br>
 * If they remove it, all applications statically linked with CRT and using rand_s will fail.<br>
 * The CryptGenRandom public alternative comes with a lot of overhead.<br>
 * Unlike what could be deduced from NTSecAPI.h, calling convention of RtlGenRandom is stdcall (ret 8 at the end).
 * </p>
 */

#ifdef RT_DEFINE_WINDOWS

typedef BOOLEAN (APIENTRY *rt_random_rtl_gen_random_t)(PVOID RandomBuffer, ULONG RandomBufferLength);

static struct rt_fast_initialization rt_random_initialization = RT_FAST_INITIALIZATION_STATIC_INIT;

/* Whether initialization of random generator has been successful or not. */
static rt_b rt_random_initialization_successful;

/* Error code of failed initialization. */
static DWORD rt_random_initialization_error;

static rt_random_rtl_gen_random_t rt_random_rtl_gen_random;

#endif

#ifdef RT_DEFINE_WINDOWS

static rt_s rt_random_initialize(void)
{
	rt_h library_handle;
	rt_s ret;

	library_handle = LoadLibrary(_R("advapi32.dll"));
	if (RT_UNLIKELY(!library_handle))
		goto error;
#ifdef RT_DEFINE_GCC
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
	rt_random_rtl_gen_random = (rt_random_rtl_gen_random_t)GetProcAddress(library_handle, "SystemFunction036");
#ifdef RT_DEFINE_GCC
#pragma GCC diagnostic pop
#endif
	if (RT_UNLIKELY(!rt_random_rtl_gen_random))
		goto error;

	ret = RT_OK;
free:
	return ret;
error:
	rt_random_initialization_error = GetLastError();
	ret = RT_FAILED;
	goto free;
}

#endif

rt_s rt_random_get_bytes(void *area, rt_un32 size)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	if (rt_fast_initialization_is_required(&rt_random_initialization)) {
		rt_random_initialization_successful = rt_random_initialize();
		rt_fast_initialization_notify_done(&rt_random_initialization);
	}
	if (RT_UNLIKELY(!rt_random_initialization_successful)) {
		/* Set last error as when the initialization has failed. */
		SetLastError(rt_random_initialization_error);
		goto error;
	}
#endif

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!rt_random_rtl_gen_random(area, size)))
		goto error;
#else
	if (RT_UNLIKELY(!RAND_bytes(area, size))) {
		/* RAND_bytes uses err_get_error and does not set errno so we use rt_error_set_last. */
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
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

rt_s rt_random_get_integer(rt_n *result)
{
	return rt_random_get_bytes(result, sizeof(rt_n));
}

rt_s rt_random_get_unsigned_integer(rt_un *result)
{
	return rt_random_get_bytes(result, sizeof(rt_un));
}

rt_s rt_random_get_unsigned_integer_with_boundaries(rt_un lower_bound, rt_un upper_bound, rt_un *result)
{
	rt_un unsigned_integer;
	rt_s ret;

	if (RT_UNLIKELY(!rt_random_get_unsigned_integer(&unsigned_integer)))
		goto error;
	unsigned_integer = unsigned_integer % (upper_bound + 1 - lower_bound);
	*result = unsigned_integer + lower_bound;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_random_get_integer_with_boundaries(rt_n lower_bound, rt_n upper_bound, rt_n *result)
{
	rt_un unsigned_integer;
	rt_s ret;

	if (RT_UNLIKELY(!rt_random_get_unsigned_integer(&unsigned_integer)))
		goto error;
	unsigned_integer = unsigned_integer % (rt_un)(upper_bound + 1 - lower_bound);
	*result = unsigned_integer + lower_bound;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

#include "layer000/rt_types.h"

/**
 * This special source file fix a compilation error related to FPU when not using CRT.
 *
 * <p>
 * It has no associated header.
 * </p>
 */

#if defined(__cplusplus)
extern "C" {
#endif
void *_fltused;
#if defined(__cplusplus)
};
#endif

#if defined(RT_DEFINE_WINDOWS) && defined(RT_SHARED_LIBRARY)

/* If we are building the dll, we do not use the CRT in it, so we need memcmp/memcpy/memmove/memset code from rpr_mem.h. */
#include "rpr_mem.h"

/* Define the dll entry point. */
rt_n32 RT_STDCALL DllMainCRTStartup(RT_UNUSED rt_h dll_instance_handle, RT_UNUSED rt_un32 reason, RT_UNUSED void *reserved)
{
	return 1;
}

#endif

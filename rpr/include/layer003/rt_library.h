#ifndef RT_LIBRARY_H
#define RT_LIBRARY_H

#include "layer000/rt_types.h"

struct rt_library {
#ifdef RT_DEFINE_WINDOWS
	rt_h handle;
#else
	void *handle;
#endif
};

/**
 * Fake function pointer type.
 */
typedef void (*rt_library_function_t)(void);

RT_API rt_s rt_library_create(struct rt_library *library, const rt_char *name);

RT_API rt_s rt_library_get_function(struct rt_library *library, const rt_char *name, rt_library_function_t *function);

RT_API rt_s rt_library_free(struct rt_library *library);

#endif /* RT_LIBRARY_H */

#ifndef RT_THREAD_LOCAL_STORAGE_H
#define RT_THREAD_LOCAL_STORAGE_H

#include "layer000/rt_types.h"

/**
 * @file
 * Thread local storage.
 * 
 * <p>
 * The <tt>data</tt> pointer is initialized to RT_NULL by the operating systems.
 * </p>
 */

struct rt_thread_local_storage {
	/* TlsAlloc returns a DWORD under Windows, and pthread_key_t is an unsigned int under Linux. */
	rt_un32 index;
};

RT_API rt_s rt_thread_local_storage_create(struct rt_thread_local_storage *thread_local_storage);
RT_API rt_s rt_thread_local_storage_set(struct rt_thread_local_storage *thread_local_storage, void *data);
RT_API rt_s rt_thread_local_storage_get(struct rt_thread_local_storage *thread_local_storage, void **data);
RT_API rt_s rt_thread_local_storage_free(struct rt_thread_local_storage *thread_local_storage);

#endif /* RT_THREAD_LOCAL_STORAGE_H */

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
#ifdef RT_DEFINE_WINDOWS
	/* TlsAlloc returns a DWORD under Windows. */
	rt_un32 tls_index;
#else
	/* Under Linux, pthread_key_t is an unsigned int. */
	rt_un32 key;
#endif
};

rt_s rt_thread_local_storage_create(struct rt_thread_local_storage *thread_local_storage);
rt_s rt_thread_local_storage_set(struct rt_thread_local_storage *thread_local_storage, void *data);
rt_s rt_thread_local_storage_get(struct rt_thread_local_storage *thread_local_storage, void **data);
rt_s rt_thread_local_storage_free(struct rt_thread_local_storage *thread_local_storage);

#endif /* RT_THREAD_LOCAL_STORAGE_H */

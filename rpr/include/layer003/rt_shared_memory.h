#ifndef RT_SHARED_MEMORY_H
#define RT_SHARED_MEMORY_H

#include "layer000/rt_types.h"

/**
 * @file
 * Shared memory can be used for inter-process communication.
 */

struct rt_shared_memory {
#ifdef RT_DEFINE_WINDOWS
	rt_h file_mapping_handle;
#endif
	void *area;
	rt_un size;
};

enum rt_shared_memory_right {
	RT_SHARED_MEMORY_RIGHT_READ = 1,
	RT_SHARED_MEMORY_RIGHT_READ_WRITE,
	RT_SHARED_MEMORY_RIGHT_EXECUTE_READ,
	RT_SHARED_MEMORY_RIGHT_EXECUTE_READ_WRITE
};

/**
 * <p>
 * The memory is zero initialized.
 * </p>
 * 
 * @param name The name has to start with a slash.
 */
RT_API rt_s rt_shared_memory_create(struct rt_shared_memory *shared_memory, const rt_char *name, rt_un size, enum rt_shared_memory_right shared_memory_right);

RT_API rt_s rt_shared_memory_free(struct rt_shared_memory *shared_memory);

/**
 * Can be called once to get ride of the Linux shared memory after all processes are done. 
 */
RT_API rt_s rt_shared_memory_destroy(const rt_char *name);

#endif /* RT_SHARED_MEMORY_H */

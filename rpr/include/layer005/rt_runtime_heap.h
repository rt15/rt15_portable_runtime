#ifndef RT_RUNTIME_HEAP_H
#define RT_RUNTIME_HEAP_H

#include "layer000/rt_types.h"
#include "layer003/rt_heap.h"

/**
 * Multithreaded heap based on the process heap on Windows and runtime heap on Linux.
 */
struct rt_runtime_heap {
	struct rt_heap heap;
};

RT_API rt_s rt_runtime_heap_create(struct rt_runtime_heap *runtime_heap);

#endif /* RT_RUNTIME_HEAP_H */

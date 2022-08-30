#include "layer005/rt_runtime_heap.h"

#include "layer004/rt_static_heap.h"

static void *rt_runtime_heap_alloc(struct rt_heap *heap, void **area, rt_un size)
{
	return rt_static_heap_alloc(area, size);
}

static void *rt_runtime_heap_realloc(struct rt_heap *heap, void **area, rt_un size)
{
	return rt_static_heap_realloc(area, size);
}

static rt_s rt_runtime_heap_free(struct rt_heap *heap, void **area)
{
	return rt_static_heap_free(area);
}

static rt_s rt_runtime_heap_close(struct rt_heap *heap)
{
	return RT_OK;
}

rt_s rt_runtime_heap_create(struct rt_runtime_heap *runtime_heap)
{
	runtime_heap->heap.alloc = &rt_runtime_heap_alloc;
	runtime_heap->heap.realloc = &rt_runtime_heap_realloc;
	runtime_heap->heap.free = &rt_runtime_heap_free;
	runtime_heap->heap.close = &rt_runtime_heap_close;
	return RT_OK;
}

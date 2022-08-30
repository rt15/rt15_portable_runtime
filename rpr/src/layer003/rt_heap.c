#include "layer003/rt_heap.h"

#include "layer002/rt_error.h"

void *rt_heap_alloc_if_needed(void *buffer, rt_un buffer_capacity, void **heap_buffer, rt_un *heap_buffer_capacity, void **area, rt_un size, struct rt_heap *heap)
{
	/* If a buffer is provided, attempt to use it. */
	if (size <= buffer_capacity) {
		/* The buffer is enough, don't go any further. */
		*area = buffer;
		goto free;
	}

	/* buffer_capacity is not enough. Try to use heap buffer. */
	if (heap_buffer_capacity && (size <= *heap_buffer_capacity)) {
		/* Given heap buffer is enough. */
		*area = *heap_buffer;
		goto free;
	}

	/* Both buffer and heap_buffer are not enough, use heap if available. */
	if (heap) {
		/* If a too small heap buffer exists, realloc. */
		if (*heap_buffer) {
			/* If this fails, heap_buffer is left untouched and the caller can try to free it later on. */
			*area = heap->realloc(heap, heap_buffer, size);
		} else {
			/* Allocate space from the heap. */
			*area = heap->alloc(heap, heap_buffer, size);
		}
		*heap_buffer_capacity = size;
	} else {
		/* Buffer is not enough and no heap has been provided. Notify error. */
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		*area = RT_NULL;
	}

free:
	return *area;
}

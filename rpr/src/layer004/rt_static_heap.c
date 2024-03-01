#include "layer004/rt_static_heap.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer003/rt_fast_initialization.h"

#ifdef RT_DEFINE_WINDOWS

static struct rt_fast_initialization rt_static_heap_initialization = RT_FAST_INITIALIZATION_STATIC_INIT;

/* Process heap handle. */
/* No need to free heap handle from GetProcessHeap. */
static rt_h rt_static_heap_process_heap_handle;

#endif

void *rt_static_heap_alloc(void **area, rt_un size)
{
#ifdef RT_DEFINE_WINDOWS

	if (rt_fast_initialization_is_required(&rt_static_heap_initialization)) {
		rt_static_heap_process_heap_handle = GetProcessHeap();
		if (RT_UNLIKELY(!rt_static_heap_process_heap_handle)) {
			/* GetProcessHeap returns null and calls SetLastError in case of failure. */
			goto error;
		}
		rt_fast_initialization_notify_done(&rt_static_heap_initialization);
	}

	*area = HeapAlloc(rt_static_heap_process_heap_handle, 0, size);
	if (!*area) {
		/* HeapAlloc does not call SetLastError. */
		rt_error_set_last(RT_ERROR_NOT_ENOUGH_MEMORY);
	}
	goto free;
error:
	*area = RT_NULL;
free:

#else
	/* Malloc sets errno. */
	*area = malloc(size);
#endif

	return *area;
}

void *rt_static_heap_realloc(void **area, rt_un size)
{
	void *result;

#ifdef RT_DEFINE_WINDOWS
	/* HeapReAlloc return NULL and call SetLastError in case of failure. Current area is left unchanged. */
	result = HeapReAlloc(rt_static_heap_process_heap_handle, 0, *area, size);
#else
	/* realloc sets errno. Current area is left unchanged. */
	result = realloc(*area, size);
#endif
	/* If re-alloc has been successful, *area is no more allocated. */
	if (result) {
		*area = result;
	}
	return result;
}

rt_s rt_static_heap_free(void **area)
{
	rt_s ret;

	if (*area) {
#ifdef RT_DEFINE_WINDOWS
		/* HeapFree return 0 and call SetLastError in case of error. */
		ret = HeapFree(rt_static_heap_process_heap_handle, 0, *area);
#else
		/* The free function returns void. */
		free(*area);
		ret = RT_OK;
#endif
		*area = RT_NULL;
	} else {
		ret = RT_OK;
	}
	return ret;
}

void *rt_static_heap_alloc_if_needed(void *buffer, rt_un buffer_capacity, void **heap_buffer, rt_un *heap_buffer_capacity, void **area, rt_un size)
{
	if (size <= buffer_capacity) {
		/* The buffer is enough, no need to allocate anything. */
		*area = buffer;
	} else {
		if (heap_buffer_capacity && (size <= (rt_un)*heap_buffer_capacity)) {
			/* The heap buffer is enough, no need to realloc. */
			*area = *heap_buffer;
		} else {
			/* We need to allo/realloc. */
			if (*heap_buffer) {
				*area = rt_static_heap_realloc(heap_buffer, size);
			} else {
				*area = rt_static_heap_alloc(heap_buffer, size);
			}
			*heap_buffer_capacity = size;
		}
	}
	return *area;
}

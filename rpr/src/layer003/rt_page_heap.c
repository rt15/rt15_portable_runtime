#include "layer003/rt_page_heap.h"

#include "layer001/rt_memory.h"
#include "layer002/rt_virtual_memory.h"

static void *rt_page_heap_alloc_method(struct rt_heap *heap, void **area, rt_un size)
{
	return rt_page_heap_alloc(area, size);
}

static void *rt_page_heap_realloc_method(struct rt_heap *heap, void **area, rt_un size)
{
	return rt_page_heap_realloc(area, size);
}

static rt_s rt_page_heap_free_method(struct rt_heap *heap, void **area)
{
	return rt_page_heap_free(area);
}

static rt_s rt_page_heap_close_method(struct rt_heap *heap)
{
	/* Nothing to do. */
	return RT_OK;
}

rt_s rt_page_heap_create(struct rt_page_heap *page_heap)
{
	page_heap->heap.alloc = &rt_page_heap_alloc_method;
	page_heap->heap.realloc = &rt_page_heap_realloc_method;
	page_heap->heap.free = &rt_page_heap_free_method;
	page_heap->heap.close = &rt_page_heap_close_method;
	return RT_OK;
}

void *rt_page_heap_alloc(void **area, rt_un size)
{
	rt_un *area_size;

	if (rt_virtual_memory_alloc((void**)&area_size, size + sizeof(rt_un))) {
		area_size[0] = size;
		area_size++;
	}
	/* At this point, area_size point either just after the size or is RT_NULL. */
	*area = area_size;

	return *area;
}

void *rt_page_heap_realloc(void **area, rt_un size)
{
	rt_un *area_size;
	rt_un new_required_size;
	rt_un new_required_pages_count;
	void* old_area;
	rt_un old_size;
	rt_un old_required_size;
	rt_un old_required_pages_count;
	rt_un page_size;
	rt_un size_to_copy;
	void *new_area;

	/* Retrieve old size. */
	area_size = *area;
	area_size--;
	old_size = area_size[0];

	/* Compute old and new required sizes. */
	old_required_size = old_size + sizeof(rt_un);
	new_required_size = size + sizeof(rt_un);

	page_size = rt_virtual_memory_get_page_size();

	old_required_pages_count = old_required_size / page_size;
	if (RT_MEMORY_MODULO_POWER_OF_TWO(old_required_size, page_size))
		old_required_pages_count++;

	new_required_pages_count = new_required_size / page_size;
	if (RT_MEMORY_MODULO_POWER_OF_TWO(new_required_size, page_size))
		new_required_pages_count++;

	if (old_required_pages_count == new_required_pages_count) {
		/* The old area is big enough, nothing to do. We also keep the original size. */
		new_area = *area;
	} else {
		/* We need more or less pages. If this fails we will return RT_NULL and are is left unchanged. */
		if (rt_page_heap_alloc(&new_area, size)) {

			/* Copy from the old area to the new area. */
			if (size < old_size) {
				size_to_copy = size;
			} else {
				size_to_copy = old_size;
			}
			RT_MEMORY_COPY(*area, new_area, size_to_copy);

			old_area = *area;
			if (rt_page_heap_free(&old_area)) {
				/* All great, update area pointer. */
				*area = new_area;
			} else {
				/* We failed to free the old area. We free the new one, let area unchanged and return RT_NULL. */
				rt_page_heap_free(new_area);
			}
		}
	}

	return new_area;
}

rt_s rt_page_heap_free(void **area)
{
	rt_un *area_size;
	rt_s ret;

	if (*area) {
		area_size = *area;
		area_size--;
		*area = RT_NULL;
		ret = rt_virtual_memory_free((void**)&area_size, area_size[0] + sizeof(rt_un));
	} else {
		ret = RT_OK;
	}
	return ret;
}

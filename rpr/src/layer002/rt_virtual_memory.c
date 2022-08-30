#include "layer002/rt_virtual_memory.h"

#include "layer001/rt_os_headers.h"

/* Access are not synchronized but it is not big of a deal if two threads write to this variable. */
static rt_un rt_virtual_memory_page_size;

rt_un rt_virtual_memory_get_page_size()
{
#ifdef RT_DEFINE_WINDOWS
	SYSTEM_INFO system_info;
#endif
	if (rt_virtual_memory_page_size == 0) {
#ifdef RT_DEFINE_WINDOWS
		/* GetSystemInfo cannot fail. */
		GetSystemInfo(&system_info);
		rt_virtual_memory_page_size = system_info.dwPageSize;
#else
		/* getpagesize cannot fail. */
		rt_virtual_memory_page_size = getpagesize();
#endif
	}
	return rt_virtual_memory_page_size;
}

void *rt_virtual_memory_alloc(void **area, rt_un size)
{
#ifdef RT_DEFINE_WINDOWS
	/* Returns NULL and set last error in case of issue. */
	*area = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#else
	*area = (rt_un*)mmap(0, size, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (*area == MAP_FAILED)
		*area = RT_NULL;
#endif
	return *area;
}

rt_s rt_virtual_memory_free(void **area, rt_un size)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	ret = VirtualFree(*area, 0, MEM_RELEASE);
#else
	ret = (munmap(*area, size) == 0);
#endif
	*area = RT_NULL;
	return ret;
}

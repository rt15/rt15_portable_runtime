#include <rpr.h>

rt_s zz_test_virtual_memory(void)
{
	rt_un page_size;
	rt_un size;
	rt_char8 *area = RT_NULL;
	rt_un i;
	rt_s ret = RT_FAILED;

	page_size = rt_virtual_memory_get_page_size();

	/* Allocate 18 pages by requesting 17 pages + 1 byte. */
	size = page_size * 17 + 1;

	if (RT_UNLIKELY(!rt_virtual_memory_alloc((void**)&area, size)))
		goto end;

	/* The area should be page aligned. */
	if (RT_UNLIKELY(RT_TYPE_MAKE_UINTEGER(area) % page_size))
		goto end;

	/* Make sure we can write all 17 pages plus the extra one. */
	for (i = 0; i < 18 * page_size; i++) {
		area[i] = 'a';
	}

	ret = RT_OK;
end:
	if (area) {
		if (RT_UNLIKELY(!rt_virtual_memory_free((void**)&area, size)))
			ret = RT_FAILED;
	}
	return ret;
}

#include <rpr.h>

rt_s zz_test_virtual_memory()
{
	rt_un page_size;
	rt_un size;
	rt_char8 *area = RT_NULL;
	rt_un i;
	rt_s ret;

	page_size = rt_virtual_memory_get_page_size();

	/* Allocate 18 pages by requesting 17 pages + 1 byte. */
	size = page_size * 17 + 1;

	if (!rt_virtual_memory_alloc((void**)&area, size))
		goto error;

	/* The area should be page aligned. */
	if (RT_TYPE_MAKE_UINTEGER(area) % page_size)
		goto error;

	/* Make sure we can write all 17 pages plus the extra one. */
	for (i = 0; i < 18 * page_size; i++) {
		area[i] = 'a';
	}

	ret = RT_OK;
free:
	if (area) {
		if (!rt_virtual_memory_free((void**)&area, size) && ret)
			goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

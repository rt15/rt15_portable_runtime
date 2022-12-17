#include "layer002/rt_comparison.h"

#include "layer001/rt_memory.h"

rt_s rt_comparison_default_callback(const void *item1, const void *item2, RT_UNUSED void *context, rt_n *comparison_result)
{
	const rt_uchar8 *str1 = (rt_uchar8*)item1;
	const rt_uchar8 *str2 = (rt_uchar8*)item2;

	while (*str1 && (*str1 == *str2)) {
		str1++;
		str2++;
	}
	*comparison_result = *str1 - *str2;
	return RT_OK;
}

rt_s rt_comparison_default_with_size_callback(const void *item1, rt_un item1_size, const void *item2, rt_un item2_size, RT_UNUSED void *context, rt_n *comparison_result)
{
	const rt_uchar8 *str1 = (rt_uchar8*)item1;
	const rt_uchar8 *str2 = (rt_uchar8*)item2;
	rt_un smaller_size = (item1_size <= item2_size) ? item1_size : item2_size;
	rt_un i;

	for (i = 0; i < smaller_size; i++) {
		if (str1[i] != str2[i])
			break;
	}
	if (i == smaller_size) {
		if (item1_size > smaller_size) {
			*comparison_result = str1[i];
		} else if (item2_size > smaller_size) {
			*comparison_result = -str2[i];
		} else {
			*comparison_result = 0;
		}
	} else {
		*comparison_result = str1[i] - str2[i];
	}
	return RT_OK;
}

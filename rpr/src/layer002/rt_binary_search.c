#include "layer002/rt_binary_search.h"

rt_s rt_binary_search_index(const void *area, const void *item, rt_un size, rt_un item_size, rt_comparison_callback_t callback, void *context, rt_un *item_index)
{
	const rt_char8 *area_chars;
	rt_un lower_bound;
	rt_un upper_bound;
	rt_un middle;
	rt_n comparison_result;
	rt_s ret;

	if (size) {
		area_chars = area;

		lower_bound = 0;
		upper_bound = size - 1;

		while (lower_bound < upper_bound) {
			/* Code must guarantee the interval is reduced at each iteration. */
			middle = lower_bound + (upper_bound - lower_bound) / 2;

			if (!callback(&area_chars[middle * item_size], item, context, &comparison_result))
				goto error;
			if (comparison_result < 0) {
				lower_bound = middle + 1;
			} else {
				upper_bound = middle;
			}
		}

		/* At this point lower_bound should be equal to upper_bound. */

		/* Deferred test for equality. */
		if (!callback(&area_chars[lower_bound * item_size], item, context, &comparison_result))
			goto error;
		if (comparison_result) {
			*item_index = RT_TYPE_MAX_UN;
		} else {
			*item_index = lower_bound;
		}
	} else {
		*item_index = RT_TYPE_MAX_UN;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_binary_search_insertion_index(const void *area, const void *item, rt_un size, rt_un item_size, rt_comparison_callback_t callback, void *context, rt_un *item_index)
{
	const rt_char8 *area_chars;
	rt_un lower_bound;
	rt_un upper_bound;
	rt_un middle;
	rt_n comparison_result;
	rt_s ret;

	if (size) {
		area_chars = area;

		lower_bound = 0;
		upper_bound = size - 1;

		while (lower_bound < upper_bound) {
			/* Code must guarantee the interval is reduced at each iteration. */
			middle = lower_bound + (upper_bound - lower_bound) / 2;

			if (!callback(&area_chars[middle * item_size], item, context, &comparison_result))
				goto error;
			if (comparison_result < 0) {
				lower_bound = middle + 1;
			} else {
				upper_bound = middle;
			}
		}
		if (!callback(&area_chars[upper_bound * item_size], item, context, &comparison_result))
			goto error;
		if (comparison_result < 0) {
			*item_index = upper_bound + 1;
		} else {
			*item_index = upper_bound;
		}
	} else {
		/* No item yet, insert at first index. */
		*item_index = 0;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

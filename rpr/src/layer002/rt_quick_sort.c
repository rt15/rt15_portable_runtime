#include "layer002/rt_quick_sort.h"

#include "layer001/rt_memory.h"

/**
 * Selection sort should be better in our case as it should swap less items than insertion sort.
 * 
 * <p>
 * We put the max of the unsorted array as the last element, decrease the size of the unsorted array, then repeat.
 * </p>
 */
static rt_s rt_quick_sort_selection_sort(rt_char8 *low, rt_char8 *high, rt_un item_size, rt_comparison_callback_t callback, void *context)
{
	rt_char8 *end_of_unsorted = high;
	rt_char8 *max;
	rt_char8 *in_unsorted;
	rt_n comparison_result;
	rt_s ret;

	/* While the unsorted array has at least two elements (low and end_of_unsorted). */
	while (end_of_unsorted > low) {
		/* We search for the max of the unsorted array. */
		max = low;
		for (in_unsorted = low + item_size; in_unsorted <= end_of_unsorted; in_unsorted += item_size) {
			if (RT_UNLIKELY(!callback(in_unsorted, max, context, &comparison_result)))
				goto error;
			if (comparison_result > 0)
				max = in_unsorted;
		}

		/* We put the maximum of the unsorted array at the end of the unsorted array. */
		rt_memory_swap(max, end_of_unsorted, item_size);

		/* The last item of the unsorted array will be now part of the sorted array at the end. */
		end_of_unsorted -= item_size;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

/**
 * Partition the data into 2 parts. At the end:<br>
 * From low to pivot, all the values are less or equals to the value pointed by pivot.<br>
 * From pivot to high, all the values are greater or equals to the value pointed by pivot.
 */
static rt_s rt_quick_sort_partition(rt_char8 *low, rt_char8 *high, rt_un size, rt_un item_size, rt_comparison_callback_t callback, void *context, rt_char8 **pivot)
{
	rt_char8 *middle = low + (size / 2) * item_size;
	rt_n comparison_result;
	rt_char8 *current_low;
	rt_char8 *current_high;
	rt_char8* local_pivot;
	rt_s ret;

	/* Sort low, middle and high items. */
	/* As a result, the pivot is chosen from 3 values, and is stored in the middle of the array. */

	if (RT_UNLIKELY(!callback(low, middle, context, &comparison_result)))
		goto error;
	if (comparison_result > 0)
		rt_memory_swap(low, middle, item_size);
	
	if (RT_UNLIKELY(!callback(low, high, context, &comparison_result)))
		goto error;
	if (comparison_result > 0)
		rt_memory_swap(low, high, item_size);

	if (RT_UNLIKELY(!callback(middle, high, context, &comparison_result)))
		goto error;
	if (comparison_result > 0)
		rt_memory_swap(middle, high, item_size);
	
	current_low = low;
	current_high = high;
	local_pivot = middle;

	while (RT_TRUE) {

		/* Search the first low that is greater than the pivot. */
		while (RT_TRUE) {
			if (current_low >= local_pivot)
				 break;
			current_low += item_size;
			if (current_low == local_pivot)
				break;
			if (RT_UNLIKELY(!callback(current_low, local_pivot, context, &comparison_result)))
				goto error;
			if (comparison_result > 0)
				break;
		}

		/* Search the first high that is smaller than the pivot, starting from the end. */
		while (RT_TRUE) {
			if (current_high <= local_pivot)
				break;
			current_high -= item_size;
			if (current_high == local_pivot)
				break;
			if (RT_UNLIKELY(!callback(current_high, local_pivot, context, &comparison_result)))
				goto error;
			if (comparison_result < 0)
				break;
		}

		/* If indexes cross, the partitioning is done. */
		if (current_low >= current_high)
			break;

		/* Both values pointed by current_low and current_high are on the wrong side of the pivot, so we swap them. */
		rt_memory_swap(current_low, current_high, item_size);

		/* If we moved the pivot, we have to reflect this move into local_pivot pointer. */
		if (local_pivot == current_high)
			local_pivot = current_low;
		else if (local_pivot == current_low)
			local_pivot = current_high;
	}
	*pivot = local_pivot;
			
	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_quick_sort(const void *area, rt_un size, rt_un item_size, rt_comparison_callback_t callback, void *context)
{
	rt_char8 *low = (rt_char8*)area;
	rt_char8 *high = (rt_char8*)area + (size - 1) * item_size;
	rt_un sub_array_size;
	rt_char8 *pivot;
	rt_char8 *low_stack[62];
	rt_char8 *high_stack[62];
	rt_un stacks_size = 0;
	rt_s ret;

recurse:

	if (high != low) {
		sub_array_size = (high - low) / item_size + 1;

		if (sub_array_size <= 8) {
	
			if (RT_UNLIKELY(!rt_quick_sort_selection_sort(low, high, item_size, callback, context)))
				goto error;

		} else {

			if (RT_UNLIKELY(!rt_quick_sort_partition(low, high, sub_array_size, item_size, callback, context, &pivot)))
				goto error;

			/* We do the smaller partition first to limit the stack size. */
			if (pivot - low >= high - pivot) {
				/* We save the bigger partition (the lower one) for later. */
				if (low < pivot) {
					low_stack[stacks_size] = low;
					high_stack[stacks_size] = pivot - item_size;
					stacks_size++;
				}
				if (high > pivot) {
					low = pivot + item_size;
					goto recurse;
				}
			} else {
				/* We save the bigger partition (the upper one) for later. */
				if (high > pivot) {
					low_stack[stacks_size] = pivot + item_size;
					high_stack[stacks_size] = high;
					stacks_size++;
				}
				if (low < pivot) {
					high = pivot - item_size;
					goto recurse;
				}
			}
		}
	}

	/* Process what is left in the stack. */
	if (stacks_size) {
		stacks_size--;
		low = low_stack[stacks_size];
		high = high_stack[stacks_size];
		goto recurse;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

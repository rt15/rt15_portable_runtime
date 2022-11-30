#ifndef RT_QUICK_SORT_H
#define RT_QUICK_SORT_H

#include "layer000/rt_types.h"
#include "layer001/rt_comparison.h"

/**
 * @file
 * Quick sort implementation based on Microsoft qsort.
 */

/**
 * <p>
 * Can fail only if the comparison callback fails.
 * </p>
 */
rt_s rt_quick_sort(void *area, rt_un size, rt_un item_size, rt_comparison_callback_t callback, void *context);

#endif /* RT_QUICK_SORT_H */

#ifndef RT_COMPARISON_H
#define RT_COMPARISON_H

#include "layer000/rt_types.h"

/**
 * @file
 * Comparison callback used in some other headers.
 */

 /**
  * <p>
  * Basically, for ascending sort, <tt>comparison_result</tt> = <tt>item1</tt> - <tt>item2</tt>.
  * </p>
  */
typedef rt_s (*rt_comparison_callback_t)(const void *item1, const void *item2, void *context, rt_n *comparison_result);

typedef rt_s (*rt_comparison_with_size_callback_t)(const void *item1, rt_un item1_size, const void *item2, rt_un item2_size, void *context, rt_n *comparison_result);

rt_s rt_comparison_default_callback(const void *item1, const void *item2, void *context, rt_n *comparison_result);

rt_s rt_comparison_default_with_size_callback(const void *item1, rt_un item1_size, const void *item2, rt_un item2_size, void *context, rt_n *comparison_result);

#endif /* RT_COMPARISON_H */

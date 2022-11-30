#ifndef RT_COMPARISON_H
#define RT_COMPARISON_H

/**
 * @file
 * Comparison callback used in some other headers.
 */

 /**
  * <p>
  * Basically, for ascending sort, <tt>comparison_result</tt> = <tt>item1</tt> - <tt>item2</tt>.
  * </p>
  */
typedef rt_s (*rt_comparison_callback_t)(void *item1, void *item2, void *context, rt_n *comparison_result);

#endif /* RT_COMPARISON_H */

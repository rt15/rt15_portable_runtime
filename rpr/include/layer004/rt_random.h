#ifndef RT_RANDOM_H
#define RT_RANDOM_H

#include "layer000/rt_types.h"

/**
 * Fill given <tt>area</tt> with <tt>size</tt> random bytes.
 */
rt_s rt_random_get_bytes(void *area, rt_un32 size);

/**
 * Provide a random integer (Which can be positive or negative.
 */
rt_s rt_random_get_integer(rt_n *result);

/**
 * Provide a random positive integer.
 */
rt_s rt_random_get_unsigned_integer(rt_un *result);

/**
 * Provide a random positive integer between <tt>lower_bound</tt> included and <tt>upper_bound</tt> included.
 */
rt_s rt_random_get_unsigned_integer_with_boundaries(rt_un lower_bound, rt_un upper_bound, rt_un *result);

/**
 * Provide a random integer between <tt>lower_bound</tt> included and <tt>upper_bound</tt> included.
 */
rt_s rt_random_get_integer_with_boundaries(rt_n lower_bound, rt_n upper_bound, rt_n *result);

#endif /* RT_RANDOM_H */

#ifndef RT_DOUBLE_H
#define RT_DOUBLE_H

#include <layer000/rt_types.h>

#define RT_DOUBLE_EPSILON 1e-10

/**
 * Round <tt>x</tt> to the nearest integer.
 */
#define RT_DOUBLE_ROUND(x) ((x) >= 0 ? (rt_n)((x) + 0.5 + RT_DOUBLE_EPSILON) : (rt_n)((x) - 0.5 - RT_DOUBLE_EPSILON))

#define RT_DOUBLE_ABS(x) ((x) >= 0 ? (x) : -(x))

#define RT_DOUBLE_EQUALS(x, y) (RT_DOUBLE_ABS((x) - (y)) < RT_DOUBLE_EPSILON)

#endif /* RT_DOUBLE_H */

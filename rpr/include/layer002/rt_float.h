#ifndef RT_FLOAT_H
#define RT_FLOAT_H

#include <layer000/rt_types.h>

#define RT_FLOAT_EPSILON 1e-5f

/**
 * Round <tt>x</tt> to the nearest integer.
 */
#define RT_FLOAT_ROUND(x) ((x) >= 0 ? (rt_n32)((x) + 0.5 + RT_FLOAT_EPSILON) : (rt_n32)((x) - 0.5 - RT_FLOAT_EPSILON))

#define RT_FLOAT_ABS(x) ((x) >= 0 ? (x) : -(x))

#define RT_FLOAT_EQUALS(x, y) (RT_FLOAT_ABS((x) - (y)) < RT_FLOAT_EPSILON)

#endif /* RT_FLOAT_H */

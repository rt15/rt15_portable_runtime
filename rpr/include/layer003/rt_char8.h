#ifndef RT_CHAR8_H
#define RT_CHAR8_H

#include "layer000/rt_types.h"

/* String size that can be used once in a function and avoid stack probe. */
#define RT_CHAR8_BIG_STRING_SIZE 3840
/* String size that can be used twice in a function and avoid stack probe. */
#define RT_CHAR8_HALF_BIG_STRING_SIZE 1920
/* String size that can be used three times in a function and avoid stack probe. */
#define RT_CHAR8_THIRD_BIG_STRING_SIZE 1280
/* String size that can be used four times in a function and avoid stack probe. */
#define RT_CHAR8_QUARTER_BIG_STRING_SIZE 960

/**
 * Test two strings for equality.
 *
 * <p>
 * This function can be faster than traditional strcmp because it first checks that the strings have the same size.<br>
 * If the size is different, RT_FALSE is immediately returned. As a result, this function cannot return an rt_n32 with the difference like classical strcmp.
 * </p>
 *
 */
rt_b rt_char8_equals(const rt_char8 *string1, rt_un string1_size, const rt_char8 *string2, rt_un string2_size);

/**
 *
 * <p>
 * If the buffer is too small, the function writes as much characters as possible and a null terminating character.
 * </p>
 *
 * <p>
 * Null characters in <tt>suffix</tt> are ignored.
 * </p>
 *
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
rt_s rt_char8_append(const rt_char8 *suffix, rt_un suffix_size, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 *
 * <p>
 * If the buffer is too small, the function writes as much characters as possible and a null terminating character.
 * </p>
 *
 * <p>
 * Null characters in <tt>string</tt> are ignored.
 * </p>
 */
rt_s rt_char8_copy(const rt_char8 *string, rt_un string_size, rt_char8 *buffer, rt_un buffer_capacity);

rt_un rt_char8_get_size(const rt_char8 *string);

/**
 * <p>
 * Always write a null terminating character if <tt>buffer_size</tt> is greater than 0.
 * </p>
 */
rt_s rt_char8_append_n(rt_n value, rt_un base, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size);

#endif /* RT_CHAR8_H */

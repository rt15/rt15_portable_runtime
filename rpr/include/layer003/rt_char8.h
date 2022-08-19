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

#define RT_CHAR8_FAST_LOWER_CHAR(CHARACTER) ((CHARACTER >= 'A' && CHARACTER <= 'Z') ? CHARACTER + 32 : CHARACTER)
#define RT_CHAR8_FAST_UPPER_CHAR(CHARACTER) ((CHARACTER >= 'a' && CHARACTER <= 'z') ? CHARACTER - 32 : CHARACTER)

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
 * If the buffer is too small, the function writes as much characters as possible and a zero terminating character.
 * </p>
 *
 * <p>
 * Zero characters in <tt>suffix</tt> are ignored.
 * </p>
 *
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
rt_s rt_char8_append(const rt_char8 *suffix, rt_un suffix_size, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 *
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
rt_s rt_char8_append_char(rt_char8 character, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 *
 * <p>
 * If the buffer is too small, the function writes as much characters as possible and a zero terminating character.
 * </p>
 *
 * <p>
 * Zero characters in <tt>str</tt> are ignored.
 * </p>
 */
rt_s rt_char8_copy(const rt_char8 *str, rt_un str_size, rt_char8 *buffer, rt_un buffer_capacity);

rt_un rt_char8_get_size(const rt_char8 *str);

/**
 * <p>
 * Always write a zero terminating character if <tt>buffer_size</tt> is greater than 0.
 * </p>
 *
 * @param base Between 2 and 36, both included.
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
rt_s rt_char8_append_n(rt_n value, rt_un base, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * Manage only characters from 'A' to 'Z'.<br>
 * Leave other characters unchanged.
 *
 * @return The size of the str.
 */
rt_un rt_char8_fast_lower(rt_char8 *str);

/**
 * Manage only characters from 'a' to 'z'.<br>
 * Leave other characters unchanged.
 *
 * @return The size of the str.
 */
rt_un rt_char8_fast_upper(rt_char8 *str);

/**
 * Find <tt>searched</tt> character into <tt>str</tt>.
 * @return index of <tt>searched</tt> in <tt>str</tt> or RT_TYPE_MAX_UN if not found.
 */
rt_un rt_char8_search_char(const rt_char8 *str, rt_char8 searched);

rt_s rt_char8_convert_to_un(const rt_char8* str, rt_un *result);

#endif /* RT_CHAR8_H */

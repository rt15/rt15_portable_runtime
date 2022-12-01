#ifndef RT_CHAR_H
#define RT_CHAR_H

#include "layer000/rt_types.h"

/* String size that can be used once in a function and avoid stack probe. */
#define RT_CHAR_BIG_STRING_SIZE 1920
/* String size that can be used twice in a function and avoid stack probe. */
#define RT_CHAR_HALF_BIG_STRING_SIZE 960
/* String size that can be used three times in a function and avoid stack probe. */
#define RT_CHAR_THIRD_BIG_STRING_SIZE 640
/* String size that can be used four times in a function and avoid stack probe. */
#define RT_CHAR_QUARTER_BIG_STRING_SIZE 480

#define RT_CHAR_FAST_LOWER_CHAR(CHARACTER) ((CHARACTER >= _R('A') && CHARACTER <= _R('Z')) ? CHARACTER + 32 : CHARACTER)
#define RT_CHAR_FAST_UPPER_CHAR(CHARACTER) ((CHARACTER >= _R('a') && CHARACTER <= _R('z')) ? CHARACTER - 32 : CHARACTER)

/**
 * Test two strings for equality.
 *
 * <p>
 * This function can be faster than traditional strcmp because it first checks that the strings have the same size.<br>
 * If the size is different, RT_FALSE is immediately returned. As a result, this function cannot return an rt_n32 with the difference like classical strcmp.
 * </p>
 *
 */
rt_b rt_char_equals(const rt_char *str1, rt_un str1_size, const rt_char *str2, rt_un str2_size);

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
rt_s rt_char_append(const rt_char *suffix, rt_un suffix_size, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 *
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
rt_s rt_char_append_char(rt_char character, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

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
rt_s rt_char_copy(const rt_char *str, rt_un str_size, rt_char *buffer, rt_un buffer_capacity);

rt_un rt_char_get_size(const rt_char *str);

/**
 * <p>
 * Always write a zero terminating character if <tt>buffer_size</tt> is greater than 0.
 * </p>
 *
 * @param base Between 2 and 36, both included.
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
rt_s rt_char_append_n(rt_n value, rt_un base, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * Manage only characters from 'A' to 'Z'.<br>
 * Leave other characters unchanged.
 *
 * @return The size of the str.
 */
rt_un rt_char_fast_lower(rt_char *str);

/**
 * Manage only characters from 'a' to 'z'.<br>
 * Leave other characters unchanged.
 *
 * @return The size of the str.
 */
rt_un rt_char_fast_upper(rt_char *str);

/**
 * Find <tt>searched</tt> character into <tt>str</tt>.
 * @return index of <tt>searched</tt> in <tt>str</tt> or RT_TYPE_MAX_UN if not found.
 */
rt_un rt_char_search_char(const rt_char *str, rt_char searched);

rt_s rt_char_convert_to_un(const rt_char *str, rt_un *result);

rt_s rt_char_convert_to_un_with_size(const rt_char *str, rt_un size, rt_un *result);

/**
 * Remove trailing characters <= ' '.
 *
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
void rt_char_right_trim(rt_char *buffer, rt_un *buffer_size);

#endif /* RT_CHAR_H */

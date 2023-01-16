#ifndef RT_CHAR8_H
#define RT_CHAR8_H

#include "layer000/rt_types.h"

/**
 * @file
 * Characters management.
 *
 * <p>
 * Functions must never rely on <tt>rt_heap</tt>.<br>
 * Introduce new char header if necessary.
 * </p>
 */

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
 * If the size is different, RT_FALSE is immediately returned. As a result, this function cannot return an rt_n32 with the difference like classical strcmp.<br>
 * If the size is the same, it is using <tt>RT_MEMORY_COMPARE</tt> as both sizes are known, and this macro should also be faster than strcmp.
 * </p>
 *
 */
rt_b rt_char8_equals(const rt_char8 *str1, rt_un str1_size, const rt_char8 *str2, rt_un str2_size);

/**
 * Equivalent to strcmp.
 *
 * <p>
 * Return 0 if the strings are equals, a negative value if the first string is less than the second one, a positive value otherwise.<br>
 * Characters are compared as unsigned characters.
 * </p>
 *
 * <p>
 * Prefer <tt>rt_char8_equals</tt> if you are only interested in equality/inequality.
 * </p>
 *
 * <p>
 * When the size of <tt>str1</tt> and <tt>str2</tt> is known and is the same, use directly <tt>RT_MEMORY_COMPARE</tt>.
 * </p>
 */
rt_n rt_char8_compare(const rt_char8 *str1, const rt_char8 *str2);

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
rt_s rt_char8_append_un(rt_un value, rt_un base, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size);

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

rt_s rt_char8_convert_to_un(const rt_char8 *str, rt_un *result);
rt_s rt_char8_convert_to_un_with_size(const rt_char8 *str, rt_un str_size, rt_un *result);

rt_s rt_char8_convert_to_n(const rt_char8 *str, rt_n *result);
rt_s rt_char8_convert_to_n_with_size(const rt_char8 *str, rt_un str_size, rt_n *result);

rt_s rt_char8_convert_hex_to_un(const rt_char8 *str, rt_un *result);
rt_s rt_char8_convert_hex_to_un_with_size(const rt_char8 *str, rt_un str_size, rt_un *result);

/**
 * Remove characters <= ' ' at the left and/or at the right of the string.
 *
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
void rt_char8_trim(rt_b left, rt_b right, rt_char8 *buffer, rt_un *buffer_size);

/**
 * Add some <tt>character</tt> instances at the left of <tt>input</tt> to ensure that <tt>buffer</tt> size is at least <tt>size</tt>.
 *
 * <p>
 * <tt>buffer</tt> can be the same as <tt>input</tt>.<br>
 * This function takes a <tt>buffer</tt> because we have to copy <tt>input</tt> even if we pad in place.
 * </p>
 *
 * @param buffer_size Out parameter.
 */
rt_s rt_char8_left_pad(const rt_char8 *input, rt_un input_size, rt_char8 character, rt_un size, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * Add some <tt>character</tt> instances at the right of <tt>input</tt> to ensure that <tt>buffer</tt> size is at least <tt>size</tt>.
 *
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
rt_s rt_char8_right_pad(rt_char8 character, rt_un size, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size);

rt_s rt_char8_ends_with(const rt_char8 *str, rt_un str_size, const rt_char8 *searched, rt_un searched_size);

/**
 * Search <tt>searched</tt> in <tt>str</tt>.
 *
 * <p>
 * If <tt>searched</tt> (or <tt>str</tt>) is empty, then RT_TYPE_MAX_UN is returned.
 * </p>
 *
 * @return index of <tt>searched</tt> in <tt>str</tt> or RT_TYPE_MAX_UN if not found.
 */
rt_un rt_char8_search(const rt_char8 *str, const rt_char8 *searched);

/**
 * Find <tt>searched</tt> character into <tt>str</tt>.
 * @return index of <tt>searched</tt> in <tt>str</tt> or RT_TYPE_MAX_UN if not found.
 */
rt_un rt_char8_search_char(const rt_char8 *str, rt_char8 searched);

/**
 * @return Number of occurrences of <tt>searched</tt> in <tt>str</tt>.
 */
rt_un rt_char8_count_occurrences(const rt_char8 *str, const rt_char8 *searched);

/**
 * Concatenate the variadic arguments at the end of given <tt>buffer</tt>.
 *
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
rt_s RT_CDECL rt_char8_concat(rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size, ...);
rt_s rt_char8_vconcat(rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size, va_list args_list);

/**
 * Replace all occurrences of <tt>searched</tt> in <tt>str</tt> with <tt>replacement</tt>.<br>
 * The result is written in <tt>buffer</tt>. <tt>str</tt> is not modified.
 *
 * <p>
 * The result is written in a new buffer because doing it in place would be expensive if the size of <tt>replacement</tt> was
 * greater than the size of <tt>searched</tt>.<br>
 * <tt>buffer</tt> must be different than <tt>str</tt>.
 * </p>
 *
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
rt_s rt_char8_replace(const rt_char8 *str, rt_un str_size,
		     const rt_char8 *searched, rt_un searched_size,
		     const rt_char8 *replacement, rt_un replacement_size,
		     rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * Simple <tt>rt_comparison_callback_t</tt> implementation for <tt>rt_char8*</tt> pointers.
 */
rt_s rt_char8_comparison_callback(const void *item1, const void *item2, void *context, rt_n *comparison_result);

/**
 * Simple <tt>rt_comparison_with_size_callback_t</tt> implementation for <tt>rt_char8*</tt> pointers.
 */
rt_s rt_char8_comparison_with_size_callback(const void *item1, rt_un item1_size, const void *item2, rt_un item2_size, void *context, rt_n *comparison_result);

/**
 * Compute a non-cryptographic hash of given <tt>data</tt>.
 *
 * <p>
 * The algorithm is FNV-1a.
 * </p>
 *
 * @param data_size Size of <tt>data</tt>, in <tt>rt_char8</tt>.
 */
rt_un rt_char8_hash(const rt_char8 *data, rt_un data_size);

/**
 * <tt>rt_hash_callback_t</tt> implementation on for <tt>rt_char8</tt>.
 *
 * <p>
 * The algorithm is FNV-1a.
 * </p>
 *
 * @param data_size Size of <tt>data</tt>, in <tt>rt_char8</tt>.
 */
rt_s rt_char8_hash_callback(const void *data, rt_un data_size, void *context, rt_un *hash);

#endif /* RT_CHAR8_H */

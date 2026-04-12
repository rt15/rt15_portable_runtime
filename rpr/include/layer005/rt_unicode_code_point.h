#ifndef RT_UNICODE_CODE_POINT_H
#define RT_UNICODE_CODE_POINT_H

#include "layer000/rt_types.h"

/**
 * Append the characters corresponding to given <tt>code_point</tt>.
 * 
 * <p>
 * In case of Windows, appends one or more UTF-16 characters.<br>
 * In case of Linux, it depends of the system encoding.
 * </p>
 * 
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
RT_API rt_s rt_unicode_code_point_encode(rt_un32 code_point, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * Append the UTF-8 characters corresponding to given <tt>code_point</tt>.
 * 
 * @param buffer_capacity Should allow at least 5 bytes (4 bytes for the UTF-8 character, 1 for the terminating zero).
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
RT_API rt_s rt_unicode_code_point_encode_to_utf8(rt_un32 code_point, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * Append the UTF-16 characters corresponding to given <tt>code_point</tt>.
 * 
 * @param buffer_capacity In 16 bits characters. Should allow at least 3 characters (4 bytes for the UTF-16 character, 2 for the terminating zero).
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
RT_API rt_s rt_unicode_code_point_encode_to_utf16(rt_un32 code_point, rt_un16 *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * Find out the code point corresponding to the first characters in given string.
 * 
 * @param characters_read The number of characters read in the string to decode the code point (out parameter).
 */
RT_API rt_s rt_unicode_code_point_decode(const rt_char *str, rt_un str_size, rt_un32 *code_point, rt_un *characters_read);

/**
 * Find out the code point corresponding to the first UTF-8 characters in given string.
 * 
 * @param characters_read The number of characters read in the string to decode the code point (out parameter).
 */
RT_API rt_s rt_unicode_code_point_decode_from_utf8(const rt_char8 *str, rt_un str_size, rt_un32 *code_point, rt_un *characters_read);

/**
 * Find out the code point corresponding to the first UTF-16 characters in given string.
 * 
 * @param characters_read The number of characters read in the string to decode the code point (out parameter).
 */
RT_API rt_s rt_unicode_code_point_decode_from_utf16(const rt_un16 *str, rt_un str_size, rt_un32 *code_point, rt_un *characters_read);

#endif /* RT_UNICODE_CODE_POINT_H */

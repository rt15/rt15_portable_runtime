#ifndef RT_UNICODE_CODE_POINT_H
#define RT_UNICODE_CODE_POINT_H

#include "layer000/rt_types.h"

/**
 * @param buffer_size The number of characters in the buffer (out parameter).
 */
RT_API rt_s rt_unicode_code_point_encode(rt_un32 code_point, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * @param buffer_capacity Should be at least 5 (4 bytes for the UTF-8 character, 1 for the terminating zero).
 * @param buffer_size The number of characters in the buffer (out parameter).
 */
RT_API rt_s rt_unicode_code_point_encode_to_utf8(rt_un32 code_point, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * @param buffer_capacity In 16 bits characters. Should be at least 3 (4 bytes for the UTF-16 character, 2 for the terminating zero).
 * @param buffer_size The number of characters in the buffer (out parameter).
 */
RT_API rt_s rt_unicode_code_point_encode_to_utf16(rt_un32 code_point, rt_un16 *buffer, rt_un buffer_capacity, rt_un *buffer_size);

#endif /* RT_UNICODE_CODE_POINT_H */

#ifndef RT_BASE_64_H
#define RT_BASE_64_H

#include "layer000/rt_types.h"

/**
 * @file
 * Base64 encoding/decoding.
 *
 * <p>
 * Base64 is designed to store binary data into simple characters widely supported by encodings.
 * </p>
 *
 * <p>
 * Each group of 6 bits are converted in a character in 'A-Z', 'a-z', '0-9', '+' and '/' (2^6 = 64 characters).<br>
 * As a result, the input length is considered to be a multiple of 6 bits (And obviously 8 bits), so 3, 6, 9, ... characters.<br>
 * The size of the result is the considered input bits count divided by 6, so 4, 8, 12.<br>
 * The last 6 bits group of input is padded with zeros if necessary and the resulting base64 string is padded with '=' to be a multiple of 4.
 * </p>
 */

RT_API rt_s rt_base64_encode(const rt_char8 *data, rt_un data_size, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size);

RT_API rt_s rt_base64_decode(const rt_char8 *base64, rt_un base64_size, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size);

#endif /* RT_BASE_64_H */

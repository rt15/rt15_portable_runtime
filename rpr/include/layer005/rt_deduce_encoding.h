#ifndef RT_DEDUCE_ENCODING_H
#define RT_DEDUCE_ENCODING_H

#include "layer000/rt_types.h"
#include "layer003/rt_file.h"
#include "layer004/rt_encoding.h"

/**
 * @file
 * Deduce the encoding of some data.
 *
 * <p>
 * Unicode BOMs:
 * </p>
 * <table>
 *   <tr><th>Unicode encoding</th><th>BOM</th></tr>
 *   <tr><td>UTF-16, little-endian</td><td>0xFF, 0xFE</td></tr>
 *   <tr><td>UTF-16, big-endian</td><td>0xFE, 0xFF</td></tr>
 *   <tr><td>UTF-32, little-endian</td><td>0xFF, 0xFE, 0x00, 0x00</td></tr>
 *   <tr><td>UTF-32, big-endian</td><td>0x00, 0x00, 0xFE, 0xFF</td></tr>
 *   <tr><td>UTF-8</td><td>0xEF, 0xBB, 0xBF</td></tr>
 * </table>
 */

/**
 * Attempt to guess the encoding of <tt>data</tt>.
 *
 * <p>
 * If it fails to deduce the encoding, then:
 * Under Windows, it always returns RT_ENCODING_SYSTEM_DEFAULT.<br>
 * Under Linux, it returns:
 * </p>
 * <ul>
 * <li>RT_ENCODING_SYSTEM_DEFAULT if it fails to retrieve the system encoding.</li>
 * <li>The system encoding if it is not RT_ENCODING_UTF_8.</li>
 * <li>RT_ENCODING_ISO_8859_15 if the system encoding is RT_ENCODING_UTF_8.</li>
 * </ul>
 *
 * @param encoding The deduced encoding. Out parameter.
 * @param bom_size The size of the found BOM. Zero if there is none. Out parameter.
 */
RT_API rt_s rt_deduce_encoding(rt_char8 *data, rt_un data_size, enum rt_encoding *encoding, rt_un *bom_size);

RT_API rt_s rt_deduce_encoding_with_file(struct rt_file *file, rt_char8 *buffer, rt_un buffer_capacity, enum rt_encoding *encoding, rt_un *bom_size);

#endif /* RT_DEDUCE_ENCODING_H */

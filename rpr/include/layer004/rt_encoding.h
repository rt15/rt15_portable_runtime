#ifndef RT_ENCODING_H
#define RT_ENCODING_H

#include "layer000/rt_types.h"
#include "layer003/rt_heap.h"

#define RT_ENCODING_ENCODINGS_COUNT 82
#define RT_ENCODING_LABEL_SIZE 260

/**
 * @file
 * Encode from rt_char to any encoding and decode from any encoding to rt_char.
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
 *
 * <p>
 * Unfortunately, iconv does not support decomposed characters under Linux.<br>
 * If there are decomposed characters in the input then the call simply fails with errno set to EILSEQ.
 * </p>
 *
 * <p>
 * Under Windows, UTF-16 -> UTF-32 and UTF-32 -> UTF-16 conversions currently only support the BMP (code points up to 0xFFFF).
 * </p>
 */

/**
 * Enumeration of supported encodings.
 *
 * <p>
 * Enumeration names are based on Windows code page names.
 * </p>
 */
enum rt_encoding {
	RT_ENCODING_SYSTEM_DEFAULT,	/* System code page on Windows, no encoding/decoding on Linux.	*/
	RT_ENCODING_IBM037,		/* IBM EBCDIC (US-Canada).					*/
	RT_ENCODING_IBM437,		/* OEM United States.						*/
	RT_ENCODING_IBM500,		/* IBM EBCDIC (International).					*/
	RT_ENCODING_ASMO_708,		/* Arabic (ASMO 708).						*/
	RT_ENCODING_IBM737,		/* Greek (DOS).							*/
	RT_ENCODING_IBM775,		/* Baltic (DOS).						*/
	RT_ENCODING_IBM850,		/* Western European (DOS).					*/
	RT_ENCODING_IBM852,		/* Central European (DOS).					*/
	RT_ENCODING_IBM855,		/* OEM Cyrillic.						*/
	RT_ENCODING_IBM857,		/* Turkish (DOS).						*/
	RT_ENCODING_IBM860,		/* Portuguese (DOS).						*/
	RT_ENCODING_IBM861,		/* Icelandic (DOS).						*/
	RT_ENCODING_IBM863,		/* French Canadian (DOS).					*/
	RT_ENCODING_IBM864,		/* Arabic (864).						*/
	RT_ENCODING_IBM865,		/* Nordic (DOS).						*/
	RT_ENCODING_CP866,		/* Cyrillic (DOS).						*/
	RT_ENCODING_IBM869,		/* Greek, Modern (DOS).						*/
	RT_ENCODING_IBM870,		/* IBM EBCDIC (Multilingual Latin-2).				*/
	RT_ENCODING_WINDOWS_874,	/* Thai (Windows).						*/
	RT_ENCODING_CP875,		/* IBM EBCDIC (Greek Modern).					*/
	RT_ENCODING_SHIFT_JIS,		/* Japanese (Shift-JIS).					*/
	RT_ENCODING_GB2312,		/* Chinese Simplified (GB2312).					*/
	RT_ENCODING_CP949,		/* Korean.							*/
	RT_ENCODING_BIG5,		/* Chinese Traditional (Big5).					*/
	RT_ENCODING_IBM1026,		/* IBM EBCDIC (Turkish Latin-5).				*/
	RT_ENCODING_IBM1047,		/* IBM Latin-1.							*/
	RT_ENCODING_UTF_16,		/* Unicode (UTF-16 with BOM, system endianness).		*/
	RT_ENCODING_UTF_16LE,		/* Unicode (UTF-16 without BOM little endian).			*/
	RT_ENCODING_UTF_16BE,		/* Unicode (UTF-16 without BOM big endian).			*/
	RT_ENCODING_WINDOWS_1250,	/* Central European (Windows).					*/
	RT_ENCODING_WINDOWS_1251,	/* Cyrillic (Windows).						*/
	RT_ENCODING_WINDOWS_1252,	/* Western European (Windows).					*/
	RT_ENCODING_WINDOWS_1253,	/* Greek (Windows).						*/
	RT_ENCODING_WINDOWS_1254,	/* Turkish (Windows).						*/
	RT_ENCODING_WINDOWS_1255,	/* Hebrew (Windows).						*/
	RT_ENCODING_WINDOWS_1256,	/* Arabic (Windows).						*/
	RT_ENCODING_WINDOWS_1257,	/* Baltic (Windows).						*/
	RT_ENCODING_WINDOWS_1258,	/* Vietnamese (Windows).					*/
	RT_ENCODING_CP1361,		/* Korean (Johab).						*/
	RT_ENCODING_MAC,		/* Western European (Mac).					*/
	RT_ENCODING_UTF_32,		/* Unicode (UTF-32 with BOM, system endianness).		*/
	RT_ENCODING_UTF_32LE,		/* Unicode (UTF-32 without BOM little endian).			*/
	RT_ENCODING_UTF_32BE,		/* Unicode (UTF-32 without BOM big endian).			*/
	RT_ENCODING_US_ASCII,		/* US-ASCII.							*/
	RT_ENCODING_T_61,		/* T.61.							*/
	RT_ENCODING_ISO_6937,		/* ISO-6937.							*/
	RT_ENCODING_IBM273,		/* IBM EBCDIC (Germany).					*/
	RT_ENCODING_IBM277,		/* IBM EBCDIC (Denmark-Norway).					*/
	RT_ENCODING_IBM278,		/* IBM EBCDIC (Finland-Sweden).					*/
	RT_ENCODING_IBM280,		/* IBM EBCDIC (Italy).						*/
	RT_ENCODING_IBM284,		/* IBM EBCDIC (Spain).						*/
	RT_ENCODING_IBM285,		/* IBM EBCDIC (UK).						*/
	RT_ENCODING_IBM297,		/* IBM EBCDIC (France).						*/
	RT_ENCODING_IBM420,		/* IBM EBCDIC (Arabic).						*/
	RT_ENCODING_IBM423,		/* IBM EBCDIC (Greek).						*/
	RT_ENCODING_IBM424,		/* IBM EBCDIC (Hebrew).						*/
	RT_ENCODING_KOI8_R,		/* Cyrillic (KOI8-R).						*/
	RT_ENCODING_IBM871,		/* IBM EBCDIC (Icelandic).					*/
	RT_ENCODING_IBM880,		/* IBM EBCDIC (Cyrillic Russian).				*/
	RT_ENCODING_IBM905,		/* IBM EBCDIC (Turkish).					*/
	RT_ENCODING_EUC_JP,		/* Japanese (JIS 0208-1990 and 0212-1990).			*/
	RT_ENCODING_KOI8_U,		/* Cyrillic (KOI8-U).						*/
	RT_ENCODING_ISO_8859_1,		/* Western European (ISO).					*/
	RT_ENCODING_ISO_8859_2,		/* Central European (ISO).					*/
	RT_ENCODING_ISO_8859_3,		/* Latin 3 (ISO).						*/
	RT_ENCODING_ISO_8859_4,		/* Baltic (ISO).						*/
	RT_ENCODING_ISO_8859_5,		/* Cyrillic (ISO).						*/
	RT_ENCODING_ISO_8859_6,		/* Arabic (ISO).						*/
	RT_ENCODING_ISO_8859_7,		/* Greek (ISO).							*/
	RT_ENCODING_ISO_8859_8,		/* Hebrew (ISO-Visual).						*/
	RT_ENCODING_ISO_8859_9,		/* Turkish (ISO).						*/
	RT_ENCODING_ISO_8859_13,	/* Estonian (ISO).						*/
	RT_ENCODING_ISO_8859_15,	/* Latin 9 (ISO).						*/
	RT_ENCODING_ISO_2022_JP,	/* Japanese (JIS).						*/
	RT_ENCODING_CS_ISO_2022_JP,	/* Japanese (JIS-Allow 1 byte Kana).				*/
	RT_ENCODING_ISO_2022_KR,	/* Korean (ISO).						*/
	RT_ENCODING_EUC_KR,		/* Korean (EUC).						*/
	RT_ENCODING_HZ_GB2312,		/* Chinese Simplified (HZ).					*/
	RT_ENCODING_GB18030,		/* Chinese Simplified (GB18030).				*/
	RT_ENCODING_UTF_7,		/* Unicode (UTF-7).						*/
	RT_ENCODING_UTF_8		/* Unicode (UTF-8 without BOM).					*/
};

struct rt_encoding_info {
	const rt_char *name;
	/* Windows code page. */
	rt_un32 code_page;
	/* A code unit is the "word size" of the character encoding scheme (1 for UTF-8, 2 for UTF-16). A code point can be made of several code units. */
	/* This is also the size of the zero terminating character. */
	rt_un code_unit_size;
	/*  A code point is a numerical value that maps to a specific character (The max is 4 for UTF-8 and UTF-16, 1 for ISO-8859-1). */
	rt_un max_code_point_size;
	/* Under Windows, CPINFOEX CodePageName is MAX_PATH. Under Linux the name is used as label. */
	rt_char label[RT_ENCODING_LABEL_SIZE];
};

/**
 * Find out the system default encoding.
 *
 * <p>
 * Fails if the system encoding is not supported by this library (which is quite possible).<br>
 * In this case, the returned encoding is zero (RT_ENCODING_SYSTEM_DEFAULT).
 * </p>
 */
RT_API rt_s rt_encoding_get_system(enum rt_encoding *encoding);

/**
 * Retrieve information about given <tt>encoding</tt>.
 *
 * <p>
 * If called with <tt>RT_ENCODING_SYSTEM_DEFAULT</tt>, retrieve information about the system default encoding.<br>
 * It will fail if the system encoding is not supported by this library.
 * </p>
 */
RT_API rt_s rt_encoding_get_info(enum rt_encoding encoding, struct rt_encoding_info *encoding_info);

/**
 * Retrieve the number of code units in <tt>data</tt>.
 *
 * <p>
 * The UTF-16 encoding terminates with 2 zero bytes and the UTF-32 encoding terminates with 4 zero bytes.<br>
 * </p>
 *
 * <p>
 * Terminating zeros have to be aligned.<br>
 * For UTF-32, it is by design as each character is 4 bytes.<br>
 * For UTF-16, a character is either 2 or 4 bytes. If the character is 4 bytes, both bytes couples are never zeros.
 * </p>
 *
 * @return The size of the buffer in code units, not including the terminating zero bytes.
 */
RT_API rt_un rt_encoding_get_size(const rt_char8 *data, rt_un code_unit_size);

/**
 * Encode given <tt>input</tt> string into given encoded <tt>output_encoding</tt>.
 *
 * <p>
 * This function ensures that the result is zero terminated.
 * </p>
 * <p>
 *
 * If <tt>output_encoding</tt> is RT_ENCODING_SYSTEM_DEFAULT then:
 * </p>
 * <ul>
 *   <li>Under Windows, system ANSI output encoding is assumed.</li>
 *   <li>Under Linux, no encoding conversion is made between the input and the result.</li>
 * </ul>
 *
 * <p>
 * Under Windows, the encoding of <tt>input</tt> is UTF-16LE.<br>
 * Under Linux the encoding of <tt>input</tt> is system encoding.
 * </p>
 *
 * @param input_size Input size, in characters.
 * @param buffer_capacity Buffer capacity, in bytes.
 * @param heap_buffer_capacity Heap buffer capacity, in bytes.
 * @param output_size Output size, in bytes, not including the zero terminating character.
 */
RT_API rt_s rt_encoding_encode(const rt_char *input, rt_un input_size, enum rt_encoding output_encoding, rt_char8 *buffer, rt_un buffer_capacity, void **heap_buffer, rt_un *heap_buffer_capacity, rt_char8 **output, rt_un *output_size, struct rt_heap *heap);

/**
 * Decode given <tt>input</tt> encoded as <tt>input_encoding</tt> into a string.
 *
 * <p>
 * This function ensures that the result is zero terminated.
 * </p>
 *
 * <p>
 * If <tt>input_encoding</tt> is RT_ENCODING_SYSTEM_DEFAULT then:
 * </p>
 * <ul>
 *   <li>Under Windows, system ANSI input encoding is assumed.</li>
 *   <li>Under Linux, no encoding conversion is made between the input and the result.</li>
 * </ul>
 *
 * <p>
 * Under Windows, the encoding of <tt>output</tt> is UTF-16LE.<br>
 * Under Linux the encoding of <tt>output</tt> is the system encoding.
 * </p>
 *
 * @param input_size Input size, in bytes.
 * @param buffer_capacity Buffer capacity, in characters.
 * @param heap_buffer_capacity Heap buffer capacity, in bytes.
 * @param output_size Output size, in characters, not including the zero terminating character.
 */
RT_API rt_s rt_encoding_decode(const rt_char8 *input, rt_un input_size, enum rt_encoding input_encoding, rt_char *buffer, rt_un buffer_capacity, void **heap_buffer, rt_un *heap_buffer_capacity, rt_char **output, rt_un *output_size, struct rt_heap *heap);

#endif /* RT_ENCODING_H */

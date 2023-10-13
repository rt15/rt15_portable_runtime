#include <rpr.h>

static rt_char *zz_string_0 = _R("eaeaeaeaeaeaeaea");
static rt_uchar8 zz_ascii[17] = { 0x65, 0x61, 0x65, 0x61, 0x65, 0x61, 0x65, 0x61, 0x65, 0x61, 0x65, 0x61, 0x65, 0x61, 0x65, 0x61, 0x00 };
static rt_uchar8 zz_ebcdic[17] = { 0x85, 0x81, 0x85, 0x81, 0x85, 0x81, 0x85, 0x81, 0x85, 0x81, 0x85, 0x81, 0x85, 0x81, 0x85, 0x81, 0x00 };
#ifdef RT_DEFINE_LINUX
/* icconv includes shift out and shift in characters at the beginning of the line. */
static rt_uchar8 zz_iso_2022_kr[21] = { 0x1B, '$', ')', 'C', 0x65, 0x61, 0x65, 0x61, 0x65, 0x61, 0x65, 0x61, 0x65, 0x61, 0x65, 0x61, 0x65, 0x61, 0x65, 0x61, 0x00 };
#endif

static rt_char *zz_string_1 = _R("ιθ");
static rt_uchar8 zz_iso_88591_1[3] = { 0xE9, 0xE8, 0x00 };
static rt_uchar8 zz_utf8_1[5] = { 0xC3, 0xA9, 0xC3, 0xA8, 0x00 };

/* e then acute accent, e then grave accent. */
/* iconv does not support decomposed characters under Linux. */
#ifdef RT_DEFINE_WINDOWS
static rt_char zz_string_2[5] = { _R('e'), 0x0301, _R('e'), 0x0300, 0x0000 };
/* "e?e`"; */
static rt_uchar8 zz_iso_88591_2[5] = { 0x65, 0x3F, 0x65, 0x60, 0x00 };
static rt_uchar8 zz_utf8_2[7] = { 0x65, 0xCC, 0x81, 0x65, 0xCC, 0x80, 0x00 };
#endif

static rt_uchar8 zz_utf16_bom_only[4] = { 0xFF, 0xFE, 0x00, 0x00 };
static rt_uchar8 zz_utf32_bom_only[8] = { 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static rt_uchar8 zz_utf16_le_with_bom[8] = { 0xFF, 0xFE, 0xE9, 0x00, 0xE8, 0x00, 0x00, 0x00 };
static rt_uchar8 zz_utf16_be_with_bom[8] = { 0xFE, 0xFF, 0x00, 0xE9, 0x00, 0xE8, 0x00, 0x00 };
static rt_uchar8 zz_utf16_le[6] = { 0xE9, 0x00, 0xE8, 0x00, 0x00, 0x00 };
static rt_uchar8 zz_utf16_be[6] = { 0x00, 0xE9, 0x00, 0xE8, 0x00, 0x00 };

static rt_uchar8 zz_utf32_le_with_bom[62] = { 0xFF, 0xFE, 0x00, 0x00, 0xE9, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static rt_uchar8 zz_utf32_be_with_bom[62] = { 0x00, 0x00, 0xFE, 0xFF, 0x00, 0x00, 0x00, 0xE9, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x00, 0x00 };
static rt_uchar8 zz_utf32_le[12] = { 0xE9, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static rt_uchar8 zz_utf32_be[12] = { 0x00, 0x00, 0x00, 0xE9, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x00, 0x00 };

/* Small t with cedilla (U+0163) is available in ISO-8859-2 but not in ISO-8859-1. */
#ifdef RT_DEFINE_GCC
#ifdef RT_DEFINE_LINUX
static rt_char zz_string_3[5] = { (rt_char)0xC3, (rt_char)0xA9, (rt_char)0xC5, (rt_char)0xA3, (rt_char)0x00 };
#else
static rt_char zz_string_3[3] = { 0x00E9, 0x0163, 0x0000 };
#endif
#else
/* We use \u syntax only with VC because of this warning under gcc: */
/* "universal character names are only valid in C99 or C++" */
/* It seems that there is no other way to ignore this warning expect using -std=c99. */
static rt_char *zz_string_3 = _R("ι\u0163");
#endif

static rt_uchar8 zz_iso_88592[3] = { 0xE9, 0xFE, 0x00 };
static rt_uchar8 zz_utf8_3[5] = { 0xC3, 0xA9, 0xC5, 0xA3, 0x00 };

static rt_s zz_test_encoding_get_system()
{
	enum rt_encoding encoding;
	rt_s ret;

	if (RT_UNLIKELY(!rt_encoding_get_system(&encoding))) goto error;
	if (RT_UNLIKELY(encoding >= RT_ENCODING_ENCODINGS_COUNT)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_encoding_get_info()
{
	struct rt_encoding_info encoding_info;
	rt_un i;
	rt_s ret;

	for (i = 0; i < RT_ENCODING_ENCODINGS_COUNT; i++) {
		if (RT_UNLIKELY(!rt_encoding_get_info(i, &encoding_info)))
			goto error;
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_encoding_get_size()
{
	rt_char8 chars[4] = { 1, 1, 1, 0 };
	rt_un16 wide_chars[4] = { 1, 1, 1, 0 };
	rt_un32 very_wide_chars[4] = { 1, 1, 1, 0 };
	rt_s ret;

	if (RT_UNLIKELY(rt_encoding_get_size(chars, 1) != 3)) goto error;
	if (RT_UNLIKELY(rt_encoding_get_size((rt_char8*)wide_chars, 2) != 3)) goto error;
	if (RT_UNLIKELY(rt_encoding_get_size((rt_char8*)very_wide_chars, 4) != 3)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_encoding_encode(const rt_char *input, rt_un input_size, const rt_char8 *encoded, enum rt_encoding encoding, rt_un code_unit_size, struct rt_heap *heap)
{
	rt_un encoded_size = rt_encoding_get_size(encoded, code_unit_size) * code_unit_size;
	rt_char8 encoded_buffer[200];
	rt_un encoded_buffer_size = encoded_size + 1 * code_unit_size;
	rt_char8 *output;
	rt_un output_size;
	void *heap_buffer = RT_NULL;
	rt_un heap_buffer_capacity = 0;
	rt_un i;
	rt_s ret;

	/* Makes sure the output buffer is clean. */
	for (i = 0; i < 200; i++)
		encoded_buffer[i] = 'z';

	if (RT_UNLIKELY(!rt_encoding_encode(input, input_size, encoding, encoded_buffer, encoded_buffer_size, RT_NULL, RT_NULL, &output, &output_size, RT_NULL))) goto error;
	if (RT_UNLIKELY(output != encoded_buffer)) goto error;
	if (RT_UNLIKELY(rt_encoding_get_size(output, code_unit_size) * code_unit_size != output_size)) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(output, output_size, encoded, encoded_size))) goto error;

	/* Make the buffer too small. */
	encoded_buffer_size--;

	if (RT_UNLIKELY(!rt_encoding_encode(input, input_size, encoding, encoded_buffer, encoded_buffer_size, &heap_buffer, &heap_buffer_capacity, &output, &output_size, heap))) goto error;
	if (RT_UNLIKELY(!heap_buffer)) goto error;
	if (RT_UNLIKELY(output != heap_buffer)) goto error;
	if (RT_UNLIKELY(rt_encoding_get_size(output, code_unit_size) * code_unit_size != output_size)) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(output, output_size, encoded, encoded_size))) goto error;
#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(heap_buffer_capacity != output_size + code_unit_size)) goto error;
#endif

	/* Makes sure the heap buffer is clean. */
	for (i = 0; i < output_size + 1; i++)
		output[i] = 'z';

	/* Try again, without buffer. */
	heap_buffer_capacity = 1;
	if (RT_UNLIKELY(!rt_encoding_encode(input, input_size, encoding, RT_NULL, 0, &heap_buffer, &heap_buffer_capacity, &output, &output_size, heap))) goto error;
	if (RT_UNLIKELY(!heap_buffer)) goto error;
	if (RT_UNLIKELY(output != heap_buffer)) goto error;
	if (RT_UNLIKELY(rt_encoding_get_size(output, code_unit_size) * code_unit_size != output_size)) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(output, output_size, encoded, encoded_size))) goto error;
#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(heap_buffer_capacity != output_size + code_unit_size)) goto error;
#endif

	ret = RT_OK;
free:
	if (heap_buffer) {
		if (RT_UNLIKELY(!heap->free(heap, &heap_buffer) && ret))
			goto error;
	}

	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_encoding_decode(const rt_char8 *input, rt_un input_size, const rt_char *decoded, enum rt_encoding encoding, struct rt_heap *heap)
{
	rt_un decoded_size = rt_char_get_size(decoded);
	rt_char decoded_buffer[200];
	rt_un decoded_buffer_size = decoded_size + 1;
	rt_char *output;
	rt_un output_size;
	void *heap_buffer = RT_NULL;
	rt_un heap_buffer_capacity = 0;
	rt_un i;
	rt_s ret;

	/* Makes sure the output buffer is clean. */
	for (i = 0; i < 200; i++)
		decoded_buffer[i] = _R('z');

	if (RT_UNLIKELY(!rt_encoding_decode(input, input_size, encoding, decoded_buffer, decoded_buffer_size, RT_NULL, RT_NULL, &output, &output_size, RT_NULL))) goto error;
	if (RT_UNLIKELY(output != decoded_buffer)) goto error;
	if (RT_UNLIKELY(rt_char_get_size(output) != output_size)) goto error;
	if (RT_UNLIKELY(!rt_char_equals(output, output_size, decoded, decoded_size))) goto error;

	/* Make the buffer too small. */
	decoded_buffer_size--;

	if (RT_UNLIKELY(!rt_encoding_decode(input, input_size, encoding, decoded_buffer, decoded_buffer_size, &heap_buffer, &heap_buffer_capacity, &output, &output_size, heap))) goto error;
	if (RT_UNLIKELY(!heap_buffer)) goto error;
	if (RT_UNLIKELY(output != heap_buffer)) goto error;
	if (RT_UNLIKELY(rt_char_get_size(output) != output_size)) goto error;
	if (RT_UNLIKELY(!rt_char_equals(output, output_size, decoded, decoded_size))) goto error;
#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(heap_buffer_capacity != (output_size + 1) * sizeof(rt_char))) goto error;
#endif

	/* Makes sure the heap buffer is clean. */
	for (i = 0; i < output_size + 1; i++)
		output[i] = _R('z');

	/* Try again, without buffer. */
	heap_buffer_capacity = 1;
	if (RT_UNLIKELY(!rt_encoding_decode(input, input_size, encoding, RT_NULL, 0, &heap_buffer, &heap_buffer_capacity, &output, &output_size, heap))) goto error;
	if (RT_UNLIKELY(!heap_buffer)) goto error;
	if (RT_UNLIKELY(output != heap_buffer)) goto error;
	if (RT_UNLIKELY(rt_char_get_size(output) != output_size)) goto error;
	if (RT_UNLIKELY(!rt_char_equals(output, output_size, decoded, decoded_size))) goto error;
#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(heap_buffer_capacity != (output_size + 1) * sizeof(rt_char))) goto error;
#endif

	ret = RT_OK;
free:
	if (heap_buffer) {
		if (RT_UNLIKELY(!heap->free(heap, &heap_buffer) && ret))
			goto error;
	}

	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_encoding_encode_decode_do(const rt_char *str, const rt_char8 *encoded, enum rt_encoding encoding, rt_un code_unit_size, rt_b encode, rt_b decode, struct rt_heap *heap)
{
	rt_un encoded_size;
	rt_s ret;

	if (encode) {
		if (RT_UNLIKELY(!zz_test_encoding_encode(str, rt_char_get_size(str), encoded, encoding, code_unit_size, heap))) goto error;
	}

	if (decode) {
		encoded_size = rt_encoding_get_size(encoded, code_unit_size) * code_unit_size;
		if (RT_UNLIKELY(!zz_test_encoding_decode(encoded, encoded_size, str, encoding, heap))) goto error;
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_encoding_encode_decode()
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	enum rt_encoding encoding;
	rt_b skip;
	rt_uchar8 *encoded;
	rt_un i;
	rt_s ret;

	if (RT_UNLIKELY(!rt_runtime_heap_create(&runtime_heap)))
		goto error;
	runtime_heap_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(_R(""),      "",                              RT_ENCODING_US_ASCII,   1, RT_TRUE,  RT_TRUE,  &runtime_heap.heap))) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(_R(""),      "",                              RT_ENCODING_ISO_8859_1, 1, RT_TRUE,  RT_TRUE,  &runtime_heap.heap))) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(_R(""),      "",                              RT_ENCODING_UTF_8,      1, RT_TRUE,  RT_TRUE,  &runtime_heap.heap))) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(_R(""),      "\0",                            RT_ENCODING_UTF_16LE,   2, RT_TRUE,  RT_TRUE,  &runtime_heap.heap))) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(_R(""),      (rt_char8*)zz_utf16_bom_only,    RT_ENCODING_UTF_16,     2, RT_TRUE,  RT_TRUE,  &runtime_heap.heap))) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(_R(""),      (rt_char8*)zz_utf32_bom_only,    RT_ENCODING_UTF_32,     4, RT_TRUE,  RT_TRUE,  &runtime_heap.heap))) goto error;

	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(zz_string_1, (rt_char8*)zz_iso_88591_1,       RT_ENCODING_ISO_8859_1, 1, RT_TRUE,  RT_TRUE,  &runtime_heap.heap))) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(zz_string_1, (rt_char8*)zz_utf8_1,            RT_ENCODING_UTF_8,      1, RT_TRUE,  RT_TRUE,  &runtime_heap.heap))) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(zz_string_1, (rt_char8*)zz_utf16_le_with_bom, RT_ENCODING_UTF_16,     2, RT_TRUE,  RT_TRUE,  &runtime_heap.heap))) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(zz_string_1, (rt_char8*)zz_utf16_be_with_bom, RT_ENCODING_UTF_16,     2, RT_FALSE, RT_TRUE,  &runtime_heap.heap))) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(zz_string_1, (rt_char8*)zz_utf16_le,          RT_ENCODING_UTF_16LE,   2, RT_TRUE,  RT_TRUE,  &runtime_heap.heap))) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(zz_string_1, (rt_char8*)zz_utf16_be,          RT_ENCODING_UTF_16BE,   2, RT_TRUE,  RT_TRUE,  &runtime_heap.heap))) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(zz_string_1, (rt_char8*)zz_utf32_le_with_bom, RT_ENCODING_UTF_32,     4, RT_TRUE,  RT_TRUE,  &runtime_heap.heap))) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(zz_string_1, (rt_char8*)zz_utf32_be_with_bom, RT_ENCODING_UTF_32,     4, RT_FALSE, RT_TRUE,  &runtime_heap.heap))) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(zz_string_1, (rt_char8*)zz_utf32_le,          RT_ENCODING_UTF_32LE,   4, RT_TRUE,  RT_TRUE,  &runtime_heap.heap))) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(zz_string_1, (rt_char8*)zz_utf32_be,          RT_ENCODING_UTF_32BE,   4, RT_TRUE,  RT_TRUE,  &runtime_heap.heap))) goto error;
#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(zz_string_2, (rt_char8*)zz_iso_88591_2,       RT_ENCODING_ISO_8859_1, 1, RT_TRUE,  RT_FALSE, &runtime_heap.heap))) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(zz_string_2, (rt_char8*)zz_utf8_2,            RT_ENCODING_UTF_8,      1, RT_TRUE,  RT_TRUE,  &runtime_heap.heap))) goto error;
#endif
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(zz_string_3, (rt_char8*)zz_iso_88592,         RT_ENCODING_ISO_8859_2, 1, RT_TRUE,  RT_TRUE,  &runtime_heap.heap))) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(zz_string_3, (rt_char8*)zz_utf8_3,            RT_ENCODING_UTF_8,      1, RT_TRUE,  RT_TRUE,  &runtime_heap.heap))) goto error;

	for (i = 1; i < RT_ENCODING_ENCODINGS_COUNT; i++) {

		skip = RT_FALSE;

		encoding = (enum rt_encoding)i;
		switch (encoding) {
		case RT_ENCODING_UTF_16:
		case RT_ENCODING_UTF_16LE:
		case RT_ENCODING_UTF_16BE:
		case RT_ENCODING_UTF_32:
		case RT_ENCODING_UTF_32LE:
		case RT_ENCODING_UTF_32BE:
			skip = RT_TRUE;
			break;
		case RT_ENCODING_IBM037:
		case RT_ENCODING_IBM500:
		case RT_ENCODING_IBM870:
		case RT_ENCODING_CP875:
		case RT_ENCODING_IBM1026:
		case RT_ENCODING_IBM1047:
		case RT_ENCODING_IBM273:
		case RT_ENCODING_IBM277:
		case RT_ENCODING_IBM278:
		case RT_ENCODING_IBM280:
		case RT_ENCODING_IBM284:
		case RT_ENCODING_IBM285:
		case RT_ENCODING_IBM297:
		case RT_ENCODING_IBM420:
		case RT_ENCODING_IBM423:
		case RT_ENCODING_IBM424:
		case RT_ENCODING_IBM871:
		case RT_ENCODING_IBM880:
		case RT_ENCODING_IBM905:
			encoded = zz_ebcdic;
			break;
#ifdef RT_DEFINE_LINUX
		case RT_ENCODING_ISO_2022_KR:
			encoded = zz_iso_2022_kr;
			break;
#endif
		default:
			encoded = zz_ascii;
		}
		if (!skip) {
			if (RT_UNLIKELY(!zz_test_encoding_encode_decode_do(zz_string_0, (rt_char8*)encoded, encoding, 1, RT_TRUE, RT_TRUE, &runtime_heap.heap)))
				goto error;
		}
	}

	ret = RT_OK;
free:
	if (runtime_heap_created) {
		runtime_heap_created = RT_FALSE;
		if (RT_UNLIKELY(!runtime_heap.heap.close(&runtime_heap.heap) && ret))
			goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_encoding()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_encoding_get_system())) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_get_info())) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_get_size())) goto error;
	if (RT_UNLIKELY(!zz_test_encoding_encode_decode())) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

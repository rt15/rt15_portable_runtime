#include <rpr.h>

static  rt_uchar8 zz_utf8_1[2] = { 0x61, 0x00 };
static  rt_uchar8 zz_utf8_2[3] = { 0xC3, 0xA9, 0x00 };
static  rt_uchar8 zz_utf8_3[4] = { 0xE1, 0x85, 0x98, 0x00 };
static  rt_uchar8 zz_utf8_4[5] = { 0xF0, 0x90, 0x8C, 0xB8, 0x00 };

static  rt_un16 zz_utf16_basic_plane[2] = { 0xE9, 0x00 };
static  rt_un16 zz_utf16_supplementary_plane[3] = { 0xD800, 0xDF38, 0x00 };

static rt_s zz_test_unicode_code_point_encode_to_utf8(rt_un32 code_point, const rt_char8 *expected)
{
	rt_un expected_size = rt_char8_get_size(expected);
	rt_char8 buffer[8];
	rt_un buffer_size;
	rt_un i;
	rt_s ret;

	if (rt_unicode_code_point_encode_to_utf8(code_point, buffer, expected_size, &buffer_size)) goto error;
	if (!rt_unicode_code_point_encode_to_utf8(code_point, buffer, expected_size + 1, &buffer_size)) goto error;
	if (buffer_size != expected_size) goto error;
	/* This loop also checks the terminating zero. */
	for (i = 0; i <= expected_size; i++) {
		if (buffer[i] != expected[i])
			goto error;
	}

#ifdef RT_DEFINE_LINUX
	/* This test assumes that the system encoding is UTF-8. */
	for (i = 0; i < 8; i++)
		buffer[i] = 12;
	if (rt_unicode_code_point_encode(code_point, buffer, expected_size, &buffer_size)) goto error;
	if (!rt_unicode_code_point_encode(code_point, buffer, expected_size + 1, &buffer_size)) goto error;
	if (buffer_size != expected_size) goto error;
	/* This loop also checks the terminating zero. */
	for (i = 0; i <= expected_size; i++) {
		if (buffer[i] != expected[i])
			goto error;
	}
#endif

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_unicode_code_point_encode_to_utf16(rt_un32 code_point, const rt_un16 *expected)
{
	rt_un expected_size = rt_encoding_get_size((const rt_char8*)expected, 2);
	rt_un16 buffer[8];
	rt_un buffer_size;
	rt_un i;
	rt_s ret;

	if (rt_unicode_code_point_encode_to_utf16(code_point, buffer, expected_size, &buffer_size)) goto error;
	if (!rt_unicode_code_point_encode_to_utf16(code_point, buffer, expected_size + 1, &buffer_size)) goto error;
	if (buffer_size != expected_size) goto error;
	/* This loop also checks the terminating zero. */
	for (i = 0; i <= expected_size; i++) {
		if (buffer[i] != expected[i])
			goto error;
	}

#ifdef RT_DEFINE_WINDOWS
	for (i = 0; i < 8; i++)
		buffer[i] = 12;
	if (rt_unicode_code_point_encode(code_point, buffer, expected_size, &buffer_size)) goto error;
	if (!rt_unicode_code_point_encode(code_point, buffer, expected_size + 1, &buffer_size)) goto error;
	if (buffer_size != expected_size) goto error;
	/* This loop also checks the terminating zero. */
	for (i = 0; i <= expected_size; i++) {
		if (buffer[i] != expected[i])
			goto error;
	}
#endif

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_unicode_code_point()
{
	rt_s ret;

	if (!zz_test_unicode_code_point_encode_to_utf8(0x61, (rt_char8*)zz_utf8_1)) goto error;
	if (!zz_test_unicode_code_point_encode_to_utf8(0xE9, (rt_char8*)zz_utf8_2)) goto error;
	if (!zz_test_unicode_code_point_encode_to_utf8(0x1158, (rt_char8*)zz_utf8_3)) goto error;
	if (!zz_test_unicode_code_point_encode_to_utf8(0x10338, (rt_char8*)zz_utf8_4)) goto error;

	if (!zz_test_unicode_code_point_encode_to_utf16(0xE9, zz_utf16_basic_plane)) goto error;
	if (!zz_test_unicode_code_point_encode_to_utf16(0x10338, zz_utf16_supplementary_plane)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

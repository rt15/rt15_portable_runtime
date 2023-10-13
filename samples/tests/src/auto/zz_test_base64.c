#include <rpr.h>

static rt_s zz_test_base64_with_size(const rt_char8 *data, rt_un data_size, const rt_char8 *base64)
{
	rt_char8 buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret;

	if (RT_UNLIKELY(!rt_base64_encode(data, data_size, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(buffer_size != rt_char8_get_size(base64))) goto error;

	if (RT_UNLIKELY(!rt_char8_equals(buffer, buffer_size, base64, rt_char8_get_size(base64)))) goto error;

	if (RT_UNLIKELY(!rt_base64_decode(base64, rt_char8_get_size(base64), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(buffer_size != data_size)) goto error;

	if (RT_UNLIKELY(!rt_char8_equals(buffer, buffer_size, data, data_size))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_base64_do(const rt_char8 *data, const rt_char8 *base64)
{
	return zz_test_base64_with_size(data, rt_char8_get_size(data), base64);
}

rt_s zz_test_base64()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_base64_with_size("\0",   1, "AA=="))) goto error;
	if (RT_UNLIKELY(!zz_test_base64_with_size("\xE9", 1, "6Q=="))) goto error;

	if (RT_UNLIKELY(!zz_test_base64_do("f",       "Zg==")))         goto error;
	if (RT_UNLIKELY(!zz_test_base64_do("fo",      "Zm8=")))         goto error;
	if (RT_UNLIKELY(!zz_test_base64_do("foo",     "Zm9v")))         goto error;
	if (RT_UNLIKELY(!zz_test_base64_do("foob",    "Zm9vYg==")))     goto error;
	if (RT_UNLIKELY(!zz_test_base64_do("fooba",   "Zm9vYmE=")))     goto error;
	if (RT_UNLIKELY(!zz_test_base64_do("foobar",  "Zm9vYmFy")))     goto error;
	if (RT_UNLIKELY(!zz_test_base64_do("foobart", "Zm9vYmFydA=="))) goto error;

	if (RT_UNLIKELY(!zz_test_base64_do("Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.",
					   "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4="))) {
		goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

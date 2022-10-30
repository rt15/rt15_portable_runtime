#include <rpr.h>

static rt_s zz_test_encoding_get_system()
{
	enum rt_encoding encoding;
	rt_s ret;

	if (!rt_encoding_get_system(&encoding)) goto error;
	if (encoding >= RT_ENCODING_ENCODINGS_COUNT) goto error;

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
		if (!rt_encoding_get_info(i, &encoding_info))
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

	if (rt_encoding_get_size(chars, 1) != 3) goto error;
	if (rt_encoding_get_size((rt_char8*)wide_chars, 2) != 3) goto error;
	if (rt_encoding_get_size((rt_char8*)very_wide_chars, 4) != 3) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_encoding()
{
	rt_s ret;

	if (!zz_test_encoding_get_system()) goto error;
	if (!zz_test_encoding_get_info()) goto error;
	if (!zz_test_encoding_get_size()) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

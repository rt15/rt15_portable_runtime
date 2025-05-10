#include <rpr.h>

static rt_s zz_test_uuid_str(const rt_char *uuid_str)
{
	struct rt_uuid uuid;
	rt_char buffer[64];
	rt_un buffer_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_uuid_parse(&uuid, uuid_str))) goto end;

	buffer_size = 0;
	if (RT_UNLIKELY(!rt_uuid_append(&uuid, buffer, 37, &buffer_size))) goto end;

	if (RT_UNLIKELY(rt_char_get_size(buffer) != buffer_size)) goto end;

	if (RT_UNLIKELY(!rt_char_equals(buffer, buffer_size, uuid_str, rt_char_get_size(uuid_str)))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_uuid_str8(const rt_char8 *uuid_str)
{
	struct rt_uuid uuid;
	rt_char8 buffer[64];
	rt_un buffer_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_uuid_parse8(&uuid, uuid_str))) goto end;

	buffer_size = 0;
	if (RT_UNLIKELY(!rt_uuid_append8(&uuid, buffer, 37, &buffer_size))) goto end;

	if (RT_UNLIKELY(rt_char8_get_size(buffer) != buffer_size)) goto end;

	if (RT_UNLIKELY(!rt_char8_equals(buffer, buffer_size, uuid_str, rt_char8_get_size(uuid_str)))) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_uuid(void)
{
	const rt_char *uuid_str = _R("8f471bef-30fc-4922-8eb1-82532a65d697");
	const rt_char8 *uuid_str8 = "8f471bef-30fc-4922-8eb1-82532a65d697";
	struct rt_uuid uuid;
	rt_char buffer[64];
	rt_un buffer_size;
	rt_char8 buffer8[64];
	rt_un buffer8_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_uuid_create(&uuid))) goto end;

	/* Test hard-coded UUID. */
	if (RT_UNLIKELY(!zz_test_uuid_str(uuid_str))) goto end;

	/* Test generated UUID. */
	buffer_size = 0;
	if (RT_UNLIKELY(!rt_uuid_append(&uuid, buffer, 37, &buffer_size))) goto end;
	if (RT_UNLIKELY(!zz_test_uuid_str(buffer))) goto end;

	/* Test hard-coded UUID. */
	if (RT_UNLIKELY(!zz_test_uuid_str8(uuid_str8))) goto end;

	/* Test generated UUID. */
	buffer8_size = 0;
	if (RT_UNLIKELY(!rt_uuid_append8(&uuid, buffer8, 37, &buffer8_size))) goto end;
	if (RT_UNLIKELY(!zz_test_uuid_str8(buffer8))) goto end;

	ret = RT_OK;
end:
	return ret;
}

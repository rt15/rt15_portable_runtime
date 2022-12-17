#include <rpr.h>

rt_s zz_test_hash_do(const rt_char8 *data, rt_un expected)
{
	rt_un data_size = rt_char8_get_size(data);
	rt_un hash;
	rt_s ret;

	hash = rt_hash(data, data_size);
	if (hash != expected) goto error;

	hash = 99;
	if (!rt_hash_default_callback(data, data_size, RT_NULL, &hash)) goto error;
	if (hash != expected) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_hash()
{
	rt_s ret;
	if (!zz_test_hash_do("Foo", 17490737515057045975ul)) goto error;
	if (!zz_test_hash_do("Bar", 1603655411640082298ul)) goto error;
	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

#include "layer001/rt_hash.h"

rt_un rt_hash(const rt_char8 *data, rt_un data_size)
{
	rt_un i;
	rt_un ret = 14695981039346656037ull;

	for (i = 0; i < data_size; i++) {
		ret ^= data[i];
		ret *= 1099511628211ull;
	}

	return ret;
}

rt_s rt_hash_default_callback(const rt_char8 *data, rt_un data_size, RT_UNUSED void *context, rt_un *hash)
{
	rt_un local_hash = 14695981039346656037ull;
	rt_un i;

	for (i = 0; i < data_size; i++) {
		local_hash ^= data[i];
		local_hash *= 1099511628211ull;
	}

	*hash = local_hash;

	return RT_OK;
}

#include <rpr.h>

static rt_s zz_test_socket_address_check_stuctures_sizes()
{
	rt_s ret;

	if (RT_UNLIKELY(sizeof(struct rt_socket_address) != 28)) goto error;
	if (RT_UNLIKELY(sizeof(struct rt_socket_address_ipv4) != 16)) goto error;
	if (RT_UNLIKELY(sizeof(struct rt_socket_address_ipv6) != 28)) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_socket_address()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_socket_address_check_stuctures_sizes())) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

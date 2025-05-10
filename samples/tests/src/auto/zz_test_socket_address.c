#include <rpr.h>

static rt_s zz_test_socket_address_check_stuctures_sizes(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(sizeof(struct rt_socket_address) != 28)) goto end;
	if (RT_UNLIKELY(sizeof(struct rt_socket_address_ipv4) != 16)) goto end;
	if (RT_UNLIKELY(sizeof(struct rt_socket_address_ipv6) != 28)) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_socket_address(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_socket_address_check_stuctures_sizes())) goto end;

	ret = RT_OK;
end:
	return ret;
}

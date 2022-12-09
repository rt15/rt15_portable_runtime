#include "layer004/rt_address.h"

#include "layer001/rt_memory.h"
#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"

void rt_address_create_ipv4_loopback(struct rt_address_ipv4 *address)
{
	address->u.address = htonl(INADDR_LOOPBACK);
}

void rt_address_create_ipv4_any(struct rt_address_ipv4 *address)
{
	address->u.address = htonl(INADDR_ANY);
}

void rt_address_create_ipv6_loopback(struct rt_address_ipv6 *address)
{
	*(struct in6_addr*)address = in6addr_loopback;
}

void rt_address_create_ipv6_any(struct rt_address_ipv6 *address)
{
	*(struct in6_addr*)address = in6addr_any;
}

rt_s rt_address_create_from_host_name(const rt_char *host_name, struct rt_address_ipv4 *ipv4_address, struct rt_address_ipv6 *ipv6_address, enum rt_address_family *address_family)
{
	int returned_value;
#ifdef RT_DEFINE_WINDOWS
	struct addrinfoW *address_info;
#else
	struct addrinfo *address_info;
#endif
	rt_b address_info_created = RT_FALSE;
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	returned_value = GetAddrInfo(host_name, RT_NULL, RT_NULL, &address_info);
	if (returned_value) {
		/* GetAddrInfoW returns an error code in case of error, zero otherwise. */
		SetLastError(returned_value);
		goto error;
	}
	address_info_created = RT_TRUE;
#else
	/* getaddrinfo returns 0 if it is successful. It returns an error code in case of error and set errno only if the error code is EAI_SYSTEM. */
	returned_value = getaddrinfo(host_name, RT_NULL, RT_NULL, &address_info);
	if (returned_value) {
		if (returned_value == EAI_SYSTEM) {
			goto error;
		} else {
			rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
			goto error;
		}
	}
	address_info_created = RT_TRUE;
#endif

	switch (address_info->ai_family) {
	case RT_ADDRESS_FAMILY_IPV4:
		RT_MEMORY_COPY(&((struct sockaddr_in*)address_info->ai_addr)->sin_addr, ipv4_address, sizeof(struct rt_address_ipv4));
		break;
	case RT_ADDRESS_FAMILY_IPV6:
		RT_MEMORY_COPY(&((struct sockaddr_in6*)address_info->ai_addr)->sin6_addr, ipv6_address, sizeof(struct rt_address_ipv6));
		break;
	default:
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto error;
	}
	*address_family = address_info->ai_family;

	ret = RT_OK;
free:
	if (address_info_created) {
		address_info_created = RT_FALSE;
#ifdef RT_DEFINE_WINDOWS
		/* FreeAddrInfo cannot fails. */
		FreeAddrInfo(address_info);
#else
		/* freeaddrinfo cannot fails. */
		freeaddrinfo(address_info);
#endif
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

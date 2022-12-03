#include "layer005/rt_socket_address.h"

#include "layer001/rt_memory.h"
#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"

void rt_socket_address_create_ipv4_loopback_address(struct rt_address_ipv4 *address)
{
	address->u.address = htonl(INADDR_LOOPBACK);
}

void rt_socket_address_create_ipv4_any_address(struct rt_address_ipv4 *address)
{
	address->u.address = htonl(INADDR_ANY);
}

void rt_socket_address_create_ipv6_loopback_address(struct rt_address_ipv6 *address)
{
	*(struct in6_addr*)address = in6addr_loopback;
}

void rt_socket_address_create_ipv6_any_address(struct rt_address_ipv6 *address)
{
	*(struct in6_addr*)address = in6addr_any;
}

void rt_socket_address_create_ipv4(struct rt_socket_address_ipv4 *socket_address, struct rt_address_ipv4 *address, rt_un port)
{
	RT_MEMORY_ZERO(socket_address, sizeof(struct rt_socket_address_ipv4));
	socket_address->address_family = RT_SOCKET_ADDRESS_FAMILY_IPV4;
	socket_address->port = htons((rt_un16)port);
	RT_MEMORY_COPY(address, &socket_address->address, sizeof(struct rt_address_ipv4));
}

void rt_socket_address_create_ipv6(struct rt_socket_address_ipv6 *socket_address, struct rt_address_ipv6 *address, rt_un port)
{
	RT_MEMORY_ZERO(socket_address, sizeof(struct rt_socket_address_ipv6));
	socket_address->address_family = RT_SOCKET_ADDRESS_FAMILY_IPV6;
	socket_address->port = htons((rt_un16)port);
	RT_MEMORY_COPY(address, &socket_address->address, sizeof(struct rt_address_ipv6));
}

rt_s rt_socket_address_get_size(struct rt_socket_address *socket_address, rt_un *socket_address_size)
{
	rt_s ret;

	switch (socket_address->address_family) {
	case RT_SOCKET_ADDRESS_FAMILY_IPV4:
		*socket_address_size = sizeof(struct rt_socket_address_ipv4);
		break;
	case RT_SOCKET_ADDRESS_FAMILY_IPV6:
		*socket_address_size = sizeof(struct rt_socket_address_ipv6);
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_socket_address_get_host_address(const rt_char *host_name, struct rt_address_ipv4 *ipv4_address, struct rt_address_ipv6 *ipv6_address, enum rt_socket_address_family *family)
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
	case RT_SOCKET_ADDRESS_FAMILY_IPV4:
		RT_MEMORY_COPY(&((struct sockaddr_in*)address_info->ai_addr)->sin_addr, ipv4_address, sizeof(struct rt_address_ipv4));
		break;
	case RT_SOCKET_ADDRESS_FAMILY_IPV6:
		RT_MEMORY_COPY(&((struct sockaddr_in6*)address_info->ai_addr)->sin6_addr, ipv6_address, sizeof(struct rt_address_ipv6));
		break;
	default:
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto error;
	}
	*family = address_info->ai_family;

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

#include "layer005/rt_socket_address.h"

#include "layer001/rt_memory.h"
#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"

void rt_socket_address_create_ipv4(struct rt_socket_address_ipv4 *socket_address, struct rt_address_ipv4 *address, rt_un port)
{
	RT_MEMORY_ZERO(socket_address, sizeof(struct rt_socket_address_ipv4));
	socket_address->address_family = RT_ADDRESS_FAMILY_IPV4;
	socket_address->port = htons((rt_un16)port);
	RT_MEMORY_COPY(address, &socket_address->address, sizeof(struct rt_address_ipv4));
}

void rt_socket_address_create_ipv6(struct rt_socket_address_ipv6 *socket_address, struct rt_address_ipv6 *address, rt_un port)
{
	RT_MEMORY_ZERO(socket_address, sizeof(struct rt_socket_address_ipv6));
	socket_address->address_family = RT_ADDRESS_FAMILY_IPV6;
	socket_address->port = htons((rt_un16)port);
	RT_MEMORY_COPY(address, &socket_address->address, sizeof(struct rt_address_ipv6));
}

rt_s rt_socket_address_get_size(struct rt_socket_address *socket_address, rt_un *socket_address_size)
{
	rt_s ret;

	switch (socket_address->address_family) {
	case RT_ADDRESS_FAMILY_IPV4:
		*socket_address_size = sizeof(struct rt_socket_address_ipv4);
		break;
	case RT_ADDRESS_FAMILY_IPV6:
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

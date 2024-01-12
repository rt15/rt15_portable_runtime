#ifndef RT_SOCKET_ADDRESS_H
#define RT_SOCKET_ADDRESS_H

#include "layer000/rt_types.h"
#include "layer004/rt_address.h"

/**
 * Parent socket address structure (sockaddr).<br>
 * Can be provided to <tt>rt_socket_connect_with_socket_address</tt> to open a connection.
 *
 * <p>
 * Underlying structure can be an rt_socket_address_ipv4 or an rt_socket_address_ipv6.<br>
 * Unlike sockaddr which has the size of rt_socket_address_ipv4 (16 bytes), rt_socket_address has the size of struct rt_socket_address_ipv6 (28 bytes).
 * </p>
 */
struct rt_socket_address {
	rt_un16 address_family;
	rt_char8 sa_data[26];
};

/**
 * rt_socket_address for IPv4 (sockaddr_in).
 */
struct rt_socket_address_ipv4 {
	rt_un16 address_family;
	rt_un16 port;
	struct rt_address_ipv4 address;
	rt_char8 zero[8];
};

/**
 * rt_socket_address for IPv6 (sockaddr_in6).
 */
struct rt_socket_address_ipv6 {
	rt_un16 address_family;
	rt_un16 port;
	rt_un32 flow_info;
	struct rt_address_ipv6 address;
	rt_un32 scope_id;
};

/**
 * Create an IPv4 socket address from an IPv4 address and a port.
 */
RT_API void rt_socket_address_create_ipv4(struct rt_socket_address_ipv4 *socket_address, struct rt_address_ipv4 *address, rt_un port);

/**
 * Create an IPv6 socket address from an IPv6 address and a port.
 */
RT_API void rt_socket_address_create_ipv6(struct rt_socket_address_ipv6 *socket_address, struct rt_address_ipv6 *address, rt_un port);

RT_API rt_s rt_socket_address_get_size(struct rt_socket_address *socket_address, rt_un *socket_address_size);

#endif /* RT_SOCKET_ADDRESS_H */

#ifndef RT_SOCKET_ADDRESS_H
#define RT_SOCKET_ADDRESS_H

#include "layer000/rt_types.h"

/**
 * Socket address family.
 *
 * <p>
 * Used as <tt>address_family</tt> parameter of <tt>rt_socket_create</tt>.
 * </p>
 *
 * <p>
 * Values are adapted for Windows and Linux.
 * </p>
 */
enum rt_socket_address_family {
	/* IPv4. */
	RT_SOCKET_ADDRESS_FAMILY_IPV4 = 2,
	/* IPv6. */
#ifdef RT_DEFINE_WINDOWS
	RT_SOCKET_ADDRESS_FAMILY_IPV6 = 23
#else
	RT_SOCKET_ADDRESS_FAMILY_IPV6 = 10
#endif
};

/**
 * Simple IPv4 32 bits address (in_addr).
 *
 * <p>
 * Can be used to create an <tt>rt_socket_address</tt> necessary to open a connection.
 * </p>
 */
struct rt_address_ipv4 {
	union {
		struct {
			rt_uchar8 byte1;
			rt_uchar8 byte2;
			rt_uchar8 byte3;
			rt_uchar8 byte4;
		} bytes;
		struct {
			rt_un16 word1;
			rt_un16 word2;
		} words;
		rt_un32 address;
	} u;
};

/**
 * Simple IPv6 128 bits address (in6_addr).
 *
 * <p>
 * Can be used to create an <tt>rt_socket_address</tt> necessary to open a connection.
 * </p>
 */
struct rt_address_ipv6 {
	union {
		rt_uchar8 bytes[16];
		rt_un16 words[8];
	} u;
};

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
 * Create a 127.0.0.1 IPv4 address.<br>
 * Can be use to connect to localhost.
 */
void rt_socket_address_create_ipv4_loopback_address(struct rt_address_ipv4 *address);

/**
 * Create an IPv4 any address.<br>
 * Can be used while binding a socket.
 */
void rt_socket_address_create_ipv4_any_address(struct rt_address_ipv4 *address);

/**
 * Create a 127.0.0.1 IPv6 address.<br>
 * Can be use to connect to localhost.
 */
void rt_socket_address_create_ipv6_loopback_address(struct rt_address_ipv6 *address);

/**
 * Create an IPv6 any address.<br>
 * Can be used while binding a socket.
 */
void rt_socket_address_create_ipv6_any_address(struct rt_address_ipv6 *address);

/**
 * Create an IPv4 socket address from an IPv4 address and a port.
 */
void rt_socket_address_create_ipv4(struct rt_socket_address_ipv4 *socket_address, struct rt_address_ipv4 *address, rt_un port);

/**
 * Create an IPv6 socket address from an IPv6 address and a port.
 */
void rt_socket_address_create_ipv6(struct rt_socket_address_ipv6 *socket_address, struct rt_address_ipv6 *address, rt_un port);

rt_s rt_socket_address_get_size(struct rt_socket_address *socket_address, rt_un *socket_address_size);

/**
 * Retrieve either the IPv4 or IPv6 address of <tt>host_name</tt>.<br>
 * This function is similar to gethostbyname/GetAddrInfo.
 *
 * <p>
 * If <tt>family</tt> is <tt>RT_SOCKET_ADDRESS_FAMILY_IPV4</tt> then the resulting address is in <tt>ipv4_address</tt>.<br>
 * If <tt>family</tt> is <tt>RT_SOCKET_ADDRESS_FAMILY_IPV6</tt> then the resulting address is in <tt>ipv6_address</tt>.
 * </p>
 */
rt_s rt_socket_address_get_host_address(const rt_char *host_name, struct rt_address_ipv4 *ipv4_address, struct rt_address_ipv6 *ipv6_address, enum rt_socket_address_family *family);

#endif /* RT_SOCKET_ADDRESS_H */

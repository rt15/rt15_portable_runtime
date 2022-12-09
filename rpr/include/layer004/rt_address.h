#ifndef RT_ADDRESS_H
#define RT_ADDRESS_H

#include "layer000/rt_types.h"

/**
 * Address family.
 *
 * <p>
 * Values are adapted for Windows and Linux.
 * </p>
 */
enum rt_address_family {
	/* IPv4. */
	RT_ADDRESS_FAMILY_IPV4 = 2,
	/* IPv6. */
#ifdef RT_DEFINE_WINDOWS
	RT_ADDRESS_FAMILY_IPV6 = 23
#else
	RT_ADDRESS_FAMILY_IPV6 = 10
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
 * Create a 127.0.0.1 IPv4 address.<br>
 * Can be use to connect to localhost.
 */
void rt_address_create_ipv4_loopback(struct rt_address_ipv4 *address);

/**
 * Create an IPv4 any address.<br>
 * Can be used while binding a socket.
 */
void rt_address_create_ipv4_any(struct rt_address_ipv4 *address);

/**
 * Create a 127.0.0.1 IPv6 address.<br>
 * Can be use to connect to localhost.
 */
void rt_address_create_ipv6_loopback(struct rt_address_ipv6 *address);

/**
 * Create an IPv6 any address.<br>
 * Can be used while binding a socket.
 */
void rt_address_create_ipv6_any(struct rt_address_ipv6 *address);

/**
 * Retrieve either the IPv4 or IPv6 address of <tt>host_name</tt>.<br>
 * This function is similar to gethostbyname/GetAddrInfo.
 *
 * <p>
 * If <tt>address_family</tt> is <tt>RT_ADDRESS_FAMILY_IPV4</tt> then the resulting address is in <tt>ipv4_address</tt>.<br>
 * If <tt>address_family</tt> is <tt>RT_ADDRESS_FAMILY_IPV6</tt> then the resulting address is in <tt>ipv6_address</tt>.
 * </p>
 */
rt_s rt_address_create_from_host_name(const rt_char *host_name, struct rt_address_ipv4 *ipv4_address, struct rt_address_ipv6 *ipv6_address, enum rt_address_family *address_family);

#endif /* RT_ADDRESS_H */

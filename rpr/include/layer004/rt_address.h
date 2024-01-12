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
	/* IPv4 (AF_INET). */
	RT_ADDRESS_FAMILY_IPV4 = 2,
	/* IPv6 (AF_INET6). */
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
RT_API void rt_address_create_ipv4_loopback(struct rt_address_ipv4 *address);

/**
 * Create an IPv4 any address.<br>
 * Can be used while binding a socket.
 */
RT_API void rt_address_create_ipv4_any(struct rt_address_ipv4 *address);

RT_API rt_s rt_address_create_ipv4(struct rt_address_ipv4 *address, const rt_char *str);

RT_API rt_s rt_address_create_ipv6(struct rt_address_ipv6 *address, const rt_char *str);

/**
 * Create a 127.0.0.1 IPv6 address.<br>
 * Can be use to connect to localhost.
 */
RT_API void rt_address_create_ipv6_loopback(struct rt_address_ipv6 *address);

/**
 * Create an IPv6 any address.<br>
 * Can be used while binding a socket.
 */
RT_API void rt_address_create_ipv6_any(struct rt_address_ipv6 *address);

/**
 * Retrieve either the IPv4 or IPv6 address of <tt>host_name</tt>.<br>
 * This function is similar to gethostbyname/GetAddrInfo.
 *
 * <p>
 * <tt>rt_socket_initialize</tt> must be called before using this function.
 * </p>
 *
 * <p>
 * If <tt>address_family</tt> is <tt>RT_ADDRESS_FAMILY_IPV4</tt> then the resulting address is in <tt>ipv4_address</tt>.<br>
 * If <tt>address_family</tt> is <tt>RT_ADDRESS_FAMILY_IPV6</tt> then the resulting address is in <tt>ipv6_address</tt>.
 * </p>
 *
 * @param address_family Mandatory in/out parameter. If zero, the function can return either an IPv4 or IPv6 address.
 */
RT_API rt_s rt_address_create_from_host_name(const rt_char *host_name, struct rt_address_ipv4 *ipv4_address, struct rt_address_ipv6 *ipv6_address, enum rt_address_family *address_family);

/**
 * Append the IPv4 address in classical form (192.168.18.139).
 *
 * @param buffer_size In/out parameter.
 */
RT_API rt_s rt_address_append_ipv4(struct rt_address_ipv4 *address, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * Append the IPv6 address in classical form (2001:db8:85a3:8d3:1319:8a2e:370:7348), 8 times 16 bits in hexa.
 *
 * <p>
 * IPv4-mapped addresses are formatted as ::ffff:192.0.2.128.<br>
 * IPv4-translated addresses are formatted as ::ffff:0:192.0.2.128.<br>
 * The longest sequence of consecutive all-zero fields is replaced with two colons (the sequence must contain at least two consecutive zeros).<br>
 * Addresses starting with 64:ff9b: (IPv4/IPv6 translations) are formatted with the classical form.
 * </p>
 *
 * @param buffer_size In/out parameter.
 */
RT_API rt_s rt_address_append_ipv6(struct rt_address_ipv6 *address, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

#endif /* RT_ADDRESS_H */

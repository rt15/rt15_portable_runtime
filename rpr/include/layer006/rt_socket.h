#ifndef RT_SOCKET_H
#define RT_SOCKET_H

#include "layer000/rt_types.h"

#include "layer002/rt_io_device.h"
#include "layer005/rt_socket_address.h"

struct rt_socket {
	struct rt_io_device io_device;
	enum rt_address_family address_family;
	rt_b blocking;
};

/**
 * Transport protocol layer type, mostly tcp/udp.
 *
 * <p>
 * Used as <tt>type</tt> parameter of <tt>rt_socket_create</tt>.
 * </p>
 *
 * <p>
 * Values are the same on both Windows and Linux.
 * </p>
 */
enum rt_socket_type {
	/* Mostly TCP. */
	RT_SOCKET_TYPE_STREAM = 1,
	/* Mostly UDP. */
	RT_SOCKET_TYPE_DGRAM = 2,
	RT_SOCKET_TYPE_RAW = 3,
	RT_SOCKET_TYPE_RDM = 4,
	RT_SOCKET_TYPE_SEQPACKET = 5
};

/**
 * Allows to specify the protocol.
 *
 * <p>
 * Used as optional <tt>protocol</tt> parameter of <tt>rt_socket_create</tt>.
 * </p>
 *
 * <p>
 * Protocols are defined as IPPROTO_XXX in operating systems headers.<br>
 * Values are correct on both Windows and Linux, except for 4 which is IPPROTO_IPV4 under Windows and IPPROTO_IPIP under Linux.<br>
 * As a result, under Linux, <tt>rt_socket_create</tt> replaces RT_SOCKET_PROTOCOL_IPV4 (4) by IPPROTO_IP (0).
 * </p>
 */
enum rt_socket_protocol {
	/* Internet Control Message Protocol. */
	RT_SOCKET_PROTOCOL_ICMP = 1,
	/* Internet Group Management Protocol. */
	RT_SOCKET_PROTOCOL_IGMP = 2,
	/* IPv4. */
	RT_SOCKET_PROTOCOL_IPV4 = 4,
	/* TCP, Transmission Control Protocol. */
	RT_SOCKET_PROTOCOL_TCP = 6,
	/* Exterior Gateway Protocol. */
	RT_SOCKET_PROTOCOL_EGP = 8,
	/* PUP protocol. */
	RT_SOCKET_PROTOCOL_PUP = 12,
	/* UDP, User Datagram Protocol. */
	RT_SOCKET_PROTOCOL_UDP = 17,
	/* XNS IDP protocol. */
	RT_SOCKET_PROTOCOL_IDP = 22,
	/* IPv6. */
	RT_SOCKET_PROTOCOL_IPV6 = 41,
	/* Encapsulation Security Payload protocol. */
	RT_SOCKET_PROTOCOL_ESP = 50,
	/* Authentication Header protocol. */
	RT_SOCKET_PROTOCOL_AH = 51,
	/* Protocol Independent Multicast. */
	RT_SOCKET_PROTOCOL_PIM = 103,
	/* Stream Control Transport Protocol. */
	RT_SOCKET_PROTOCOL_SCTP = 132,
	/* Raw IP packets. */
	RT_SOCKET_PROTOCOL_RAW = 255
};

/**
 * Socket option protocol level.
 *
 * <p>
 * Used by <tt>rt_socket_set_boolean_option</tt> and <tt>rt_socket_set_option</tt>.
 * </p>
 *
 * <p>
 * The values are the same under Windows and Linux except SOL_SOCKET which is 0xffff under Windows and 1 under Linux.<br>
 * This exception is handled by <tt>rt_socket_set_boolean_option</tt> and <tt>rt_socket_set_option</tt>.
 * </p>
 */
enum rt_socket_protocol_level {
	RT_SOCKET_PROTOCOL_LEVEL_IP = 0,
	RT_SOCKET_PROTOCOL_LEVEL_TCP = 6,
	RT_SOCKET_PROTOCOL_LEVEL_UDP = 17,
	RT_SOCKET_PROTOCOL_LEVEL_IPV6 = 41,
	RT_SOCKET_PROTOCOL_LEVEL_SOCKET = 0xffff
};

/**
 * Socket options.
 *
 * <p>
 * The values are different between Linux and Windows.<br>
 * This is taken cared of in the functions.
 * </p>
 */
enum rt_socket_option {
	/* Turn on debugging info recording. */
	RT_SOCKET_OPTION_DEBUG        = 0x0001,
	/* Socket has had listen(). */
	RT_SOCKET_OPTION_ACCEPTCONN   = 0x0002,
	/* Allows the socket to be bound to an address that is already in use. Can be used to quickly reuse a port in TIME_WAIT state. */
	RT_SOCKET_OPTION_REUSEADDR    = 0x0004,
	/* Keep connections alive. */
	RT_SOCKET_OPTION_KEEPALIVE    = 0x0008,
	/* Just use interface addresses. */
	RT_SOCKET_OPTION_DONTROUTE    = 0x0010,
	/* Permit sending of broadcast msgs. */
	RT_SOCKET_OPTION_BROADCAST    = 0x0020,
	/* Linger on close if data present. */
	RT_SOCKET_OPTION_LINGER       = 0x0080,
	/* Leave received OOB data in line. */
	RT_SOCKET_OPTION_OOBINLINE    = 0x0100,
	/* Send buffer size. */
	RT_SOCKET_OPTION_SNDBUF       = 0x1001,
	/* Receive buffer size. */
	RT_SOCKET_OPTION_RCVBUF       = 0x1002,
	/* Send low-water mark. */
	RT_SOCKET_OPTION_SNDLOWAT     = 0x1003,
	/* Receive low-water mark. */
	RT_SOCKET_OPTION_RCVLOWAT     = 0x1004,
	/* Send timeout. */
	RT_SOCKET_OPTION_SNDTIMEO     = 0x1005,
	/* Receive timeout. */
	RT_SOCKET_OPTION_RCVTIMEO     = 0x1006,
	/* Get error status and clear. */
	RT_SOCKET_OPTION_ERROR        = 0x1007,
	/* Get socket type. */
	RT_SOCKET_OPTION_TYPE         = 0x1008
};

/**
 * IPv6 options.
 *
 * <p>
 * The values are different between Linux and Windows.<br>
 * This is taken cared of in the functions.
 * </p>
 */
enum rt_socket_ipv6_option {
	/* Set/get IPv6 hop-by-hop options. */
	RT_SOCKET_IPV6_OPTION_HOPOPTS          =  1,
	/* IP unicast hop limit. */
	RT_SOCKET_IPV6_OPTION_UNICAST_HOPS     =  4,
	/* IP multicast interface. */
	RT_SOCKET_IPV6_OPTION_MULTICAST_IF     =  9,
	/* IP multicast hop limit. */
	RT_SOCKET_IPV6_OPTION_MULTICAST_HOPS   = 10,
	/* IP multicast loopback. */
	RT_SOCKET_IPV6_OPTION_MULTICAST_LOOP   = 11,
	/* Add an IP group membership. */
	RT_SOCKET_IPV6_OPTION_ADD_MEMBERSHIP   = 12,
	/* Drop an IP group membership. */
	RT_SOCKET_IPV6_OPTION_DROP_MEMBERSHIP  = 13,
	/* Receive packet information. */
	RT_SOCKET_IPV6_OPTION_PKTINFO          = 19,
	/* Receive packet hop limit. */
	RT_SOCKET_IPV6_OPTION_HOPLIMIT         = 21,
	/* Offset to checksum for raw IP socket send. */
	RT_SOCKET_IPV6_OPTION_CHECKSUM         = 26,
	/* Treat wildcard bind as AF_INET6-only. */
	RT_SOCKET_IPV6_OPTION_V6ONLY           = 27,
	/* Set/get IPv6 routing header. */
	RT_SOCKET_IPV6_OPTION_RTHDR            = 32,
	/* Receive the routing header. */
	RT_SOCKET_IPV6_OPTION_RECVRTHDR        = 38,
	/* Packet traffic class. */
	RT_SOCKET_IPV6_OPTION_TCLASS           = 39,
	/* Receive packet traffic class. */
	RT_SOCKET_IPV6_OPTION_RECVTCLASS       = 40
};

/**
 * Sending and receiving flags.
 *
 * <p>
 * Values are correct on Windows only and are adapted for Linux by the functions.
 * </p>
 */
enum rt_socket_message_flag {
	/* Process out-of-band data. */
	RT_SOCKET_MESSAGE_FLAG_OUT_OF_BAND  = 0x1,
	/* Peek at incoming message. */
	RT_SOCKET_MESSAGE_FLAG_PEEK         = 0x2,
	/* Send without using routing tables. */
	RT_SOCKET_MESSAGE_FLAG_DO_NOT_ROUTE = 0x4,
	/* Do not complete until packet is completely filled. */
	RT_SOCKET_MESSAGE_FLAG_WAIT_ALL     = 0x8
};

/**
 * <tt>rt_socket_shutdown</tt> flags.
 *
 * <p>
 * Values are correct on both Windows and Linux.
 * </p>
 */
enum rt_socket_shutdown_flag {
	/* Shutdown receive operations. */
	RT_SOCKET_SHUTDOWN_FLAG_RECEIVE = 0,
	/* Shutdown send operations. */
	RT_SOCKET_SHUTDOWN_FLAG_SEND    = 1,
	/* Shutdown both send and receive operations. */
	RT_SOCKET_SHUTDOWN_FLAG_BOTH    = 2
};

/**
 * Must be called before any other socket functions.<br />
 * Call WSAStartup under Windows and does nothing under unix.
 *
 * <p>
 * Call <tt>rt_socket_cleanup</tt> after a successful call to rt_socket_initialize when you finished using socket functions.
 * </p>
 *
 * @return RT_FALSE and set last error in case of issue.
 */
RT_API rt_s rt_socket_initialize(void);

/**
 * Create a socket.
 *
 * <p>
 * If <tt>protocol</tt> is zero, the protocol is deduced from <tt>address_family</tt> and <tt>type</tt>.<br>
 * <tt>RT_ADDRESS_FAMILY_IPV4<tt> and <tt>RT_SOCKET_TYPE_STREAM</tt> mostly means TCP.
 * <tt>RT_ADDRESS_FAMILY_IPV4<tt> and <tt>RT_SOCKET_TYPE_DGRAM</tt> mostly means UDP.
 * </p>
 *
 * <p>
 * Under Windows, an inheritable handle can be used by a process created with CreateProcess and bInheritHandles TRUE.<br>
 * Under Linux, a non-inheritable file descriptor is closed when execl/execlp/execle/execv/execvp/execvpe is used.<br>
 * It is not closed by a fork.
 * </p>
 *
 * @param address_family Mostly IPv4 or IPv6.
 * @param type Transport layer type, mostly TCP or UDP.
 * @param protocol Often zero.
 * @param inheritable Whether the socket could be used in a child process. Beware of handles/file descriptors leaks.
 */
RT_API rt_s rt_socket_create(struct rt_socket *socket, enum rt_address_family address_family, enum rt_socket_type type, enum rt_socket_protocol protocol, rt_b blocking, rt_b inheritable);

/**
 * Set a socket boolean option.
 *
 * @param protocol_level Protocol level of the option. A classical value is <tt>RT_SOCKET_PROTOCOL_LEVEL_SOCKET</tt>.
 * @param option The option to set like <tt>RT_SOCKET_OPTION_REUSEADDR</tt>.
 * @param value The value to set.
 */
RT_API rt_s rt_socket_set_boolean_option(struct rt_socket *socket, enum rt_socket_protocol_level protocol_level, enum rt_socket_option option, rt_b value);
RT_API rt_s rt_socket_set_ipv6_boolean_option(struct rt_socket *socket, enum rt_socket_protocol_level protocol_level, enum rt_socket_ipv6_option ipv6_option, rt_b value);

/**
 * Set a socket option.
 *
 * <p>
 * For boolean values, <tt>rt_socket_set_boolean_option</tt> is a bit simpler to use.
 * </p>
 *
 * @param protocol_level Protocol level of the option. A classical value is <tt>RT_SOCKET_PROTOCOL_LEVEL_SOCKET</tt>.
 * @param option The option to set like <tt>RT_SOCKET_OPTION_REUSEADDR</tt>.
 * @param value Pointer to the new option value.
 * @param value_size Size of the value pointed by <tt>value</tt>.
 */
RT_API rt_s rt_socket_set_option(struct rt_socket *socket, enum rt_socket_protocol_level protocol_level, enum rt_socket_option option, void *value, rt_un value_size);
RT_API rt_s rt_socket_set_ipv6_option(struct rt_socket *socket, enum rt_socket_protocol_level protocol_level, enum rt_socket_ipv6_option ipv6_option, void *value, rt_un value_size);

RT_API rt_s rt_socket_get_option(struct rt_socket *socket, enum rt_socket_protocol_level protocol_level, enum rt_socket_option option, void *value, rt_un *value_size);
RT_API rt_s rt_socket_get_ipv6_option(struct rt_socket *socket, enum rt_socket_protocol_level protocol_level, enum rt_socket_ipv6_option ipv6_option, void *value, rt_un *value_size);

/**
 * Make this client socket connect to <tt>host_name</tt> and <tt>port</tt>.
 *
 * <p>
 * Creates an <tt>rt_socket_address</tt> then calls <tt>rt_socket_connect_with_socket_address</tt>.<br>
 * The <tt>rt_socket_address</tt> is IPv4 or IPv6 depending on the host resolution function.
 * </p>
 */
RT_API rt_s rt_socket_connect(struct rt_socket *socket, const rt_char *host_name, rt_un port);

/**
 * Make this client socket connect to <tt>socket_address</tt>.
 *
 * <p>
 * In the case of a non-blocking socket, most of the time <tt>RT_FAILED</tt> is returned.<br>
 * You then have to use <tt>rt_error_would_block</tt> to determine if the error is a failure or if the connection is in progress.<br>
 * If the connection is in progress, you should wait for the socket to be writable to ensure that the connection is successful.
 * </p>
 */
RT_API rt_s rt_socket_connect_with_socket_address(struct rt_socket *socket, struct rt_socket_address *socket_address);

/**
 * Bind to any address and given <tt>port</tt>.
 */
RT_API rt_s rt_socket_bind(struct rt_socket *socket, rt_un port);

/**
 * Bind to given <tt>socket_address</tt>.
 */
RT_API rt_s rt_socket_bind_with_socket_address(struct rt_socket *socket, struct rt_socket_address *socket_address);

RT_API rt_s rt_socket_listen(struct rt_socket *socket);

RT_API rt_s rt_socket_listen_with_backlog(struct rt_socket *socket, rt_un backlog);

/**
 *
 * <p>
 * In the case of a non-blocking socket, you should wait for the listening socket to be readable before calling this function.
 * </p>
 *
 * <p>
 * The resulting socket is always a blocking socket.
 * </p>
 *
 * @param blocking Whether you want the returned accepted socket to be blocking or not.
 * @param socket_address If not null, will be returned with the address of the connecting entity.
 * @param socket_address_size In/out parameter. Must be null if <tt>socket_address</tt> is null. Set it with the size of <tt>socket_address</tt>. Returned with the size of the address.
 */
RT_API rt_s rt_socket_accept_connection(struct rt_socket *socket, rt_b blocking, struct rt_socket *accepted_socket, struct rt_socket_address *socket_address, rt_n32 *socket_address_size);

/**
 *
 * @param flags Combination of RT_SOCKET_MESSAGE_FLAG_XXXX flags.
 */
RT_API rt_s rt_socket_send(struct rt_socket *socket, void *data, rt_un data_size, enum rt_socket_message_flag flags, rt_un *bytes_sent);

/**
 *
 * @param flags Combination of RT_SOCKET_MESSAGE_FLAG_XXXX flags. RT_SOCKET_MESSAGE_FLAG_WAIT_ALL is a classical one.
 */
RT_API rt_s rt_socket_receive(struct rt_socket *socket, void *buffer, rt_un buffer_capacity, enum rt_socket_message_flag flags, rt_un *bytes_received);

/**
 * <tt>rt_socket_receive</tt> can receive incomplete response and RT_SOCKET_MESSAGE_FLAG_WAIT_ALL can be blocking if the remote host does not close the connection.<br>
 * This function is a workaround for this.<br>
 * It ensures that either the buffer is full or either the connection is closed.
 */
RT_API rt_s rt_socket_receive_all(struct rt_socket *socket, void *buffer, rt_un buffer_capacity, rt_un *bytes_received);

/**
 *
 * <p>
 * This function should never block, regardless of the blockiness of the socket or the <tt>SO_LINGER</tt> option.<br>
 * It should behave like close without <tt>SO_LINGER</tt>: the work is supposed to be done behind the scene.<br>
 * This behavior is not the one documented in Linux <tt>SO_LINGER</tt> manual though.
 * </p>
 *
 * @param flag One of RT_SOCKET_SHUTDOWN_FLAG_XXXX, mostly RT_SOCKET_SHUTDOWN_FLAG_BOTH.
 */
RT_API rt_s rt_socket_shutdown(struct rt_socket *socket, enum rt_socket_shutdown_flag flag);

/**
 * Close the socket.
 *
 * <p>
 * It does not block unless <tt>SO_LINGER</tt> is activated and its timeout is greater than zero.<br>
 * By default <tt>SO_LINGER</tt> is not activated.<br>
 * In case the process is terminating, <tt>exit</tt> will try to wait for the socket to finish its job though.
 * </p>
 *
 * <p>
 * If <tt>SO_LINGER</tt> is activated, its timeout is greater than zero, and the socket is non-blocking then:<br>
 * Under Linux, the call will block.<br>
 * Under Windows, the call will not block and the last error will be <tt>WSAEWOULDBLOCK</tt>.
 * </p>
 *
 * <p>
 * Activating <tt>SO_LINGER</tt> with zero as timeout (or if the timeout is reached) aborts the connection (TCP RST) and some data can easily be lost.<br>
 * There is an exception under Windows: if <tt>shutdown</tt> is called before <tt>close</tt>, then the connection is not aborted in <tt>close</tt>.
 * </p>
 *
 * <p>
 * Overall, activating <tt>SO_LINGER</tt> is pretty tricky.<br>
 * It sounds better to keep it deactivated and to rely on a protocol to make sure that all the data is sent/received.
 * </p>
 */
RT_API rt_s rt_socket_free(struct rt_socket *socket);

/**
 * Must be called after a successful call to <tt>rt_socket_initialize</tt> and after all socket functions calls.
 * Call WSACleanup under Windows and does nothing under unix.
 */
RT_API rt_s rt_socket_cleanup(void);

#endif /* RT_SOCKET_H */

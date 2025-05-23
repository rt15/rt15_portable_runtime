#include "layer006/rt_socket.h"

#include "layer001/rt_memory.h"
#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer004/rt_os_version.h"

/**
 * @file
 *
 * <p>
 * Under Windows currently WSAGetLastError is the same as GetLastError.
 * </p>
 *
 * <p>
 * Client socket:
 * </p>
 * <ol>
 *   <li>socket: Create the socket for given protocol.</li>
 *   <li>setsockopt: Set socket options.</li>
 *   <li>gethostbyname/gethostbyaddr: Retrieve a sockaddr_in structure from an host name or ip address.</li>
 *   <li>connect: Connect to given address (sockaddr) and port.</li>
 *   <li>send/recv for TCP or sendto/recvfrom for UDP.</li>
 *   <li>shutdown.</li>
 *   <li>closesocket.</li>
 * </ol>
 *
 * <p>
 * Server socket:
 * </p>
 * <ol>
 *   <li>socket: Create the socket for given protocol.</li>
 *   <li>setsockopt: Set socket options.</li>
 *   <li>bind: Associates a socket with a port (And optionally an IP address).</li>
 *   <li>listen: Indicates a readiness to accept client.</li>
 *   <li>accept: Accept an incoming connection an create associated (accepted) socket.</li>
 *   <li>send/recv for TCP or sendto/recvfrom for UDP on accepted socket.</li>
 *   <li>shutdown accepted socket only (There is no communication on the server socket).</li>
 *   <li>closesocket on both sockets.</li>
 * </ol>
 */

#ifdef RT_DEFINE_WINDOWS

rt_s rt_socket_initialize(void)
{
	WORD version_requested;
	int returned_value;
	WSADATA wsa_data;
	rt_b wsa_startup_called = RT_FALSE;
	rt_s ret = RT_FAILED;

	/* Version 2.2 is supported by even Windows NT 4.0 and Windows 95 with the Windows Socket 2 update. */
	version_requested = MAKEWORD(2, 2);
	returned_value = WSAStartup(version_requested, &wsa_data);
	if (RT_UNLIKELY(returned_value)) {
		/* WSAStartup returns error code and does not call WSASetLastError. */
		SetLastError(returned_value);
		goto end;
	}
	wsa_startup_called = RT_TRUE;

	/* Check returned version. WSAStartup return same or less than requested, never greater. */
	if (RT_UNLIKELY(LOBYTE(wsa_data.wVersion) != 2 || HIBYTE(wsa_data.wVersion) != 2)) {
		/* There has been a successful call to WSAStartup but no way to use 2.2 sockets. */
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto end;
	}

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!ret)) {
		if (wsa_startup_called) {
			WSACleanup();
		}
	}

	return ret;
}

#else

rt_s rt_socket_initialize(void)
{
	return RT_OK;
}

#endif

rt_s rt_socket_create(struct rt_socket *socket_, enum rt_address_family address_family, enum rt_socket_type type, enum rt_socket_protocol protocol, rt_b blocking, rt_b inheritable)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD flags;
	rt_b flag_no_handle_inherit_supported;
	rt_h socket_handle;
	u_long argp;
#else
	rt_un actual_type;
	int actual_protocol;
	rt_n32 socket_file_descriptor;
#endif
	rt_s ret = RT_FAILED;

	socket_->address_family = address_family;
	socket_->blocking = blocking;
#ifdef RT_DEFINE_WINDOWS
	if (blocking) {
		flags = 0;
	} else {
		/* Mandatory for non-blocking sockets. */
		flags = WSA_FLAG_OVERLAPPED;
	}

	if (!inheritable) {
		/* Flag required only to set handle non-inheritable.	*/
		/* Socket handles are inheritable by default. */

		/* WSA_FLAG_NO_HANDLE_INHERIT is supported on Windows 7 with SP1, Windows Server 2008 R2 with SP1, and later. */
		if (RT_UNLIKELY(!rt_os_version_is_greater_or_equal_to(6, 1, 1, &flag_no_handle_inherit_supported)))
			goto end;
		if (flag_no_handle_inherit_supported) {
			flags |= 0x80; /* WSA_FLAG_NO_HANDLE_INHERIT */
		}
	}

	/* WSA_FLAG_NO_HANDLE_INHERIT flag is in early versions of Windows only. */
	/* WSASocket returns INVALID_SOCKET and sets last error in case of issue. */
	socket_handle = (rt_h)WSASocket((int)address_family, (int)type, (int)protocol, RT_NULL, 0, flags);
	if (RT_UNLIKELY((rt_un)socket_handle == INVALID_SOCKET))
		goto end;

	rt_io_device_create_from_handle(&socket_->io_device, socket_handle);

	if (!inheritable && !flag_no_handle_inherit_supported) {
		/* WSA_FLAG_NO_HANDLE_INHERIT is not supported. */
		/* We manually set it to non-inheritable. */
		/* There is a race condition here: the handle could leak if CreateProcess is called in parallel. */
		if (RT_UNLIKELY(!rt_io_device_set_inheritable(&socket_->io_device, RT_FALSE))) {
			rt_socket_free(socket_);
			goto end;
		}
	}

	/* If non-blocking, then we need to configure the socket to non-blocking with ioctlsocket/FIONBIO. */
	if (!blocking) {
		/* argp must be true to enable non-blocking mode with FIONBIO command. */
		argp = RT_TRUE;

		/* Returns SOCKET_ERROR (-1) and sets WSAGetLastError in case of issue. */
		if (RT_UNLIKELY(ioctlsocket((rt_un)socket_->io_device.handle, FIONBIO, &argp))) {
			rt_socket_free(socket_);
			goto end;
		}
	}

#else

	/* Add needed flags with bitwise OR in socket type. */
	actual_type = type;
	if (!blocking) {
		actual_type |= SOCK_NONBLOCK;
	}
	if (!inheritable) {
		actual_type |= SOCK_CLOEXEC;
	}

	if (protocol == RT_SOCKET_PROTOCOL_IPV4)
		actual_protocol = IPPROTO_IP;
	else
		actual_protocol = protocol;

	/*  On error, socket function returns -1 and errno is set appropriately. */
	socket_file_descriptor = socket(address_family, actual_type, actual_protocol);
	if (RT_UNLIKELY(socket_file_descriptor == -1))
		goto end;
	rt_io_device_create_from_file_descriptor(&socket_->io_device, socket_file_descriptor);
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_socket_set_boolean_option(struct rt_socket *socket, enum rt_socket_protocol_level protocol_level, enum rt_socket_option option, rt_b value)
{
	return rt_socket_set_option(socket, protocol_level, option, &value, sizeof(value));
}

rt_s rt_socket_set_ipv6_boolean_option(struct rt_socket *socket, enum rt_socket_protocol_level protocol_level, enum rt_socket_ipv6_option ipv6_option, rt_b value)
{
	return rt_socket_set_ipv6_option(socket, protocol_level, ipv6_option, &value, sizeof(value));
}

#ifdef RT_DEFINE_LINUX

/**
 * Convert from Windows socket option to Linux socket option.
 */
static rt_s rt_socket_get_linux_option(enum rt_socket_option option, int *linux_option)
{
	rt_s ret = RT_FAILED;

	switch (option) {
	case RT_SOCKET_OPTION_DEBUG:
		*linux_option = SO_DEBUG;
		break;
	case RT_SOCKET_OPTION_ACCEPTCONN:
		*linux_option = SO_ACCEPTCONN;
		break;
	case RT_SOCKET_OPTION_REUSEADDR:
		*linux_option = SO_REUSEADDR;
		break;
	case RT_SOCKET_OPTION_KEEPALIVE:
		*linux_option = SO_KEEPALIVE;
		break;
	case RT_SOCKET_OPTION_DONTROUTE:
		*linux_option = SO_DONTROUTE;
		break;
	case RT_SOCKET_OPTION_BROADCAST:
		*linux_option = SO_BROADCAST;
		break;
	case RT_SOCKET_OPTION_LINGER:
		*linux_option = SO_LINGER;
		break;
	case RT_SOCKET_OPTION_OOBINLINE:
		*linux_option = SO_OOBINLINE;
		break;
	case RT_SOCKET_OPTION_SNDBUF:
		*linux_option = SO_SNDBUF;
		break;
	case RT_SOCKET_OPTION_RCVBUF:
		*linux_option = SO_RCVBUF;
		break;
	case RT_SOCKET_OPTION_SNDLOWAT:
		*linux_option = SO_SNDLOWAT;
		break;
	case RT_SOCKET_OPTION_RCVLOWAT:
		*linux_option = SO_RCVLOWAT;
		break;
	case RT_SOCKET_OPTION_SNDTIMEO:
		*linux_option = SO_SNDTIMEO;
		break;
	case RT_SOCKET_OPTION_RCVTIMEO:
		*linux_option = SO_RCVTIMEO;
		break;
	case RT_SOCKET_OPTION_ERROR:
		*linux_option = SO_ERROR;
		break;
	case RT_SOCKET_OPTION_TYPE:
		*linux_option = SO_TYPE;
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

/**
 * Convert from Windows IPv6 option to Linux IPv6 option.
 */
static rt_s rt_socket_get_linux_ipv6_option(enum rt_socket_ipv6_option ipv6_option, int *linux_option)
{
	rt_s ret = RT_FAILED;

	switch (ipv6_option) {
	case RT_SOCKET_IPV6_OPTION_HOPOPTS:
		*linux_option = IPV6_HOPOPTS;
		break;
	case RT_SOCKET_IPV6_OPTION_UNICAST_HOPS:
		*linux_option = IPV6_UNICAST_HOPS;
		break;
	case RT_SOCKET_IPV6_OPTION_MULTICAST_IF:
		*linux_option = IPV6_MULTICAST_IF;
		break;
	case RT_SOCKET_IPV6_OPTION_MULTICAST_HOPS:
		*linux_option = IPV6_MULTICAST_HOPS;
		break;
	case RT_SOCKET_IPV6_OPTION_MULTICAST_LOOP:
		*linux_option = IPV6_MULTICAST_LOOP;
		break;
	case RT_SOCKET_IPV6_OPTION_ADD_MEMBERSHIP:
		*linux_option = IPV6_ADD_MEMBERSHIP;
		break;
	case RT_SOCKET_IPV6_OPTION_DROP_MEMBERSHIP:
		*linux_option = IPV6_DROP_MEMBERSHIP;
		break;
	case RT_SOCKET_IPV6_OPTION_PKTINFO:
		*linux_option = IPV6_PKTINFO;
		break;
	case RT_SOCKET_IPV6_OPTION_HOPLIMIT:
		*linux_option = IPV6_HOPLIMIT;
		break;
	case RT_SOCKET_IPV6_OPTION_CHECKSUM:
		*linux_option = IPV6_CHECKSUM;
		break;
	case RT_SOCKET_IPV6_OPTION_V6ONLY:
		*linux_option = IPV6_V6ONLY;
		break;
	case RT_SOCKET_IPV6_OPTION_RTHDR:
		*linux_option = IPV6_RTHDR;
		break;
	case RT_SOCKET_IPV6_OPTION_RECVRTHDR:
		*linux_option = IPV6_RECVRTHDR;
		break;
	case RT_SOCKET_IPV6_OPTION_TCLASS:
		*linux_option = IPV6_TCLASS;
		break;
	case RT_SOCKET_IPV6_OPTION_RECVTCLASS:
		*linux_option = IPV6_RECVTCLASS;
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

#endif

/**
 * Actually set the option.<br>
 * Must be called with correct Linux option values.
 */
static rt_s rt_socket_set_option_do(struct rt_socket *socket, enum rt_socket_protocol_level protocol_level, int option, void *value, rt_un value_size)
{
#ifdef RT_DEFINE_LINUX
	int actual_protocol_level;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	/* Returns SOCKET_ERROR (-1) and sets WSAGetLastError in case of issue. */
	if (RT_UNLIKELY(setsockopt((rt_un)socket->io_device.handle, (int)protocol_level, (int)option, value, (int)value_size)))
		goto end;
#else
	if (protocol_level == RT_SOCKET_PROTOCOL_LEVEL_SOCKET) {
		/* SOL_SOCKET is 1 under Linux. */
		/* By the way a consequence is that SOL_ICMP which should be 1 is not defined in Linux headers. */
		actual_protocol_level = SOL_SOCKET;
	} else {
		actual_protocol_level = (int)protocol_level;
	}

	/* On success, zero is returned. On error, -1 is returned, and errno is set appropriately. */
	if (RT_UNLIKELY(setsockopt(socket->io_device.file_descriptor, actual_protocol_level, option, value, (socklen_t)value_size)))
		goto end;
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_socket_set_option(struct rt_socket *socket, enum rt_socket_protocol_level protocol_level, enum rt_socket_option option, void *value, rt_un value_size)
{
#ifdef RT_DEFINE_LINUX
	int actual_option;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!rt_socket_set_option_do(socket, protocol_level, option, value, value_size)))
		goto end;
#else
	if (RT_UNLIKELY(!rt_socket_get_linux_option(option, &actual_option)))
		goto end;

	if (RT_UNLIKELY(!rt_socket_set_option_do(socket, protocol_level, actual_option, value, value_size)))
		goto end;
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_socket_set_ipv6_option(struct rt_socket *socket, enum rt_socket_protocol_level protocol_level, enum rt_socket_ipv6_option ipv6_option, void *value, rt_un value_size)
{
#ifdef RT_DEFINE_LINUX
	int actual_ipv6_option;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!rt_socket_set_option_do(socket, protocol_level, ipv6_option, value, value_size)))
		goto end;
#else
	if (RT_UNLIKELY(!rt_socket_get_linux_ipv6_option(ipv6_option, &actual_ipv6_option)))
		goto end;

	if (RT_UNLIKELY(!rt_socket_set_option_do(socket, protocol_level, actual_ipv6_option, value, value_size)))
		goto end;
#endif

	ret = RT_OK;
end:
	return ret;
}

/**
 * Actually get the option.<br>
 * Must be called with correct Linux option values.
 */
static rt_s rt_socket_get_option_do(struct rt_socket *socket, enum rt_socket_protocol_level protocol_level, int option, void *value, rt_un *value_size)
{
#ifdef RT_DEFINE_WINDOWS
	int local_value_size = (int)*value_size;
#else
	socklen_t local_value_size = *value_size;
	int actual_protocol_level;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	/* Returns SOCKET_ERROR (-1) and sets WSAGetLastError in case of issue. */
	if (RT_UNLIKELY(getsockopt((rt_un)socket->io_device.handle, (int)protocol_level, (int)option, value, &local_value_size)))
		goto end;
	*value_size = local_value_size;
#else
	if (protocol_level == RT_SOCKET_PROTOCOL_LEVEL_SOCKET) {
		/* SOL_SOCKET is 1 under Linux. */
		/* By the way a consequence is that SOL_ICMP which should be 1 is not defined in Linux headers. */
		actual_protocol_level = SOL_SOCKET;
	} else {
		actual_protocol_level = (int)protocol_level;
	}

	/* On success, zero is returned. On error, -1 is returned, and errno is set appropriately. */
	if (RT_UNLIKELY(getsockopt(socket->io_device.file_descriptor, actual_protocol_level, option, value, &local_value_size)))
		goto end;
	*value_size = local_value_size;
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_socket_get_option(struct rt_socket *socket, enum rt_socket_protocol_level protocol_level, enum rt_socket_option option, void *value, rt_un *value_size)
{
#ifdef RT_DEFINE_LINUX
	int actual_option;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!rt_socket_get_option_do(socket, protocol_level, option, value, value_size)))
		goto end;
#else
	if (RT_UNLIKELY(!rt_socket_get_linux_option(option, &actual_option)))
		goto end;

	if (RT_UNLIKELY(!rt_socket_get_option_do(socket, protocol_level, actual_option, value, value_size)))
		goto end;
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_socket_get_ipv6_option(struct rt_socket *socket, enum rt_socket_protocol_level protocol_level, enum rt_socket_ipv6_option ipv6_option, void *value, rt_un *value_size)
{
#ifdef RT_DEFINE_LINUX
	int actual_ipv6_option;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!rt_socket_get_option_do(socket, protocol_level, ipv6_option, value, value_size)))
		goto end;
#else
	if (RT_UNLIKELY(!rt_socket_get_linux_ipv6_option(ipv6_option, &actual_ipv6_option)))
		goto end;

	if (RT_UNLIKELY(!rt_socket_get_option_do(socket, protocol_level, actual_ipv6_option, value, value_size)))
		goto end;
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_socket_connect(struct rt_socket *socket, const rt_char *host_name, rt_un port)
{
	struct rt_address_ipv4 ipv4_address;
	struct rt_address_ipv6 ipv6_address;
	enum rt_address_family address_family = 0;
	struct rt_socket_address_ipv4 ipv4_socket_address;
	struct rt_socket_address_ipv6 ipv6_socket_address;
	struct rt_socket_address *socket_address;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_address_create_from_host_name(host_name, &ipv4_address, &ipv6_address, &address_family)))
		goto end;

	switch (address_family) {
	case RT_ADDRESS_FAMILY_IPV4:
		rt_socket_address_create_ipv4(&ipv4_socket_address, &ipv4_address, port);
		socket_address = (struct rt_socket_address*)&ipv4_socket_address;
		break;
	case RT_ADDRESS_FAMILY_IPV6:
		rt_socket_address_create_ipv6(&ipv6_socket_address, &ipv6_address, port);
		socket_address = (struct rt_socket_address*)&ipv6_socket_address;
		break;
	default:
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto end;
	}

	if (RT_UNLIKELY(!rt_socket_connect_with_socket_address(socket, socket_address)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_socket_connect_with_socket_address(struct rt_socket *socket, struct rt_socket_address *socket_address)
{
	rt_un socket_address_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_socket_address_get_size(socket_address, &socket_address_size)))
		goto end;

#ifdef RT_DEFINE_WINDOWS

	/* If no error occurs, connect returns zero. Otherwise, it returns SOCKET_ERROR, and a specific error code can be retrieved by calling WSAGetLastError. */
	if (RT_UNLIKELY(connect((rt_un)socket->io_device.handle, (struct sockaddr*)socket_address, (int)socket_address_size))) {
		/* Notice that result is always non zero with non-blocking sockets under Windows. */
		/* The normal error should be WSAEWOULDBLOCK. */
		goto end;
	}
#else
	if (connect(socket->io_device.file_descriptor, (struct sockaddr*)socket_address, socket_address_size)) {
		if (RT_UNLIKELY(socket->blocking)) {
			goto end;
		} else {
			/* If the socket is nonblocking and the connection cannot be completed immediately, then there is a normal error EINPROGRESS. */
			if (RT_LIKELY(errno == EINPROGRESS)) {
				/* Replace the EINPROGRESS by an EWOULDBLOCK, to align with Windows. */
				rt_error_set_last(RT_ERROR_SOCKET_WOULD_BLOCK);
			} else {
				/* Real issue. */
				goto end;
			}
		}
	}
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_socket_bind(struct rt_socket *socket, rt_un port)
{
	struct rt_address_ipv4 ipv4_address;
	struct rt_socket_address_ipv4 ipv4_socket_address;
	struct rt_address_ipv6 ipv6_address;
	struct rt_socket_address_ipv6 ipv6_socket_address;
	struct rt_socket_address *socket_address;
	rt_s ret = RT_FAILED;

	switch (socket->address_family)	{
	case RT_ADDRESS_FAMILY_IPV4:
		rt_address_create_ipv4_any(&ipv4_address);
		rt_socket_address_create_ipv4(&ipv4_socket_address, &ipv4_address, port);
		socket_address = (struct rt_socket_address*)&ipv4_socket_address;
		break;
	case RT_ADDRESS_FAMILY_IPV6:
		rt_address_create_ipv6_any(&ipv6_address);
		rt_socket_address_create_ipv6(&ipv6_socket_address, &ipv6_address, port);
		socket_address = (struct rt_socket_address*)&ipv6_socket_address;
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	if (RT_UNLIKELY(!rt_socket_bind_with_socket_address(socket, socket_address)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_socket_bind_with_socket_address(struct rt_socket *socket, struct rt_socket_address *socket_address)
{
	rt_un socket_address_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_socket_address_get_size(socket_address, &socket_address_size)))
		goto end;

#ifdef RT_DEFINE_WINDOWS
	/* On success bind returns zero, -1 and sets last error otherwise. */
	if (RT_UNLIKELY(bind((rt_un)socket->io_device.handle, (struct sockaddr*)socket_address, (int)socket_address_size)))
		goto end;
#else
	/* On success bind returns zero, -1 and sets last error otherwise. */
	if (RT_UNLIKELY(bind(socket->io_device.file_descriptor, (struct sockaddr*)socket_address, socket_address_size)))
		goto end;
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_socket_listen(struct rt_socket *socket)
{
	return rt_socket_listen_with_backlog(socket, SOMAXCONN);
}

rt_s rt_socket_listen_with_backlog(struct rt_socket *socket, rt_un backlog)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	/* On success listen returns zero, -1 and sets last error otherwise. */
	ret = !listen((rt_un)socket->io_device.handle, (int)backlog);
#else
	/* On success listen returns zero, -1 and sets last error otherwise. */
	ret = !listen(socket->io_device.file_descriptor, (int)backlog);
#endif

	return ret;
}

rt_s rt_socket_accept_connection(struct rt_socket *socket, rt_b blocking, struct rt_socket *accepted_socket, struct rt_socket_address *socket_address, rt_n32 *socket_address_size)
{
#ifdef RT_DEFINE_WINDOWS
	rt_h accepted_socket_handle;
	u_long argp;
#else
	rt_n32 accepted_socket_file_descriptor;
	int flags;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	accepted_socket_handle = (rt_h)accept((rt_un)socket->io_device.handle, (struct sockaddr*)socket_address, socket_address_size);
	if (RT_UNLIKELY((rt_un)accepted_socket_handle == INVALID_SOCKET))
		goto end;
	rt_io_device_create_from_handle(&accepted_socket->io_device, accepted_socket_handle);
#else
	accepted_socket_file_descriptor = accept(socket->io_device.file_descriptor, (struct sockaddr*)socket_address, (socklen_t*)socket_address_size);
	if (RT_UNLIKELY(accepted_socket_file_descriptor == -1))
		goto end;
	rt_io_device_create_from_file_descriptor(&accepted_socket->io_device, accepted_socket_file_descriptor);
#endif

	/* Linux can accept IPv4 incoming socket connections on IPv6 socket (IPV6_V6ONLY must not be used). */
	/* But even in this case, the IPv4 address is an IPv4-mapped IPv6 address. */
	accepted_socket->address_family = socket->address_family;

	/* The resulting socket is always a blocking socket. */
	if (blocking) {
		accepted_socket->blocking = RT_TRUE;
	} else {
		accepted_socket->blocking = RT_FALSE;
#ifdef RT_DEFINE_WINDOWS

		/* argp must be true to enable non-blocking mode with FIONBIO command. */
		argp = RT_TRUE;

		/* Returns SOCKET_ERROR (-1) and sets WSAGetLastError in case of issue. */
		if (RT_UNLIKELY(ioctlsocket((rt_un)accepted_socket_handle, FIONBIO, &argp))) {
			rt_socket_free(accepted_socket);
			goto end;
		}
#else
		flags = fcntl(accepted_socket_file_descriptor, F_GETFL, 0);
		if (RT_UNLIKELY(flags == -1)) {
			rt_socket_free(accepted_socket);
			goto end;
		}

		/* Add O_NONBLOCK flag. */
		if (RT_UNLIKELY(fcntl(accepted_socket_file_descriptor, F_SETFL, flags | O_NONBLOCK) == -1)) {
			rt_socket_free(accepted_socket);
			goto end;
		}
#endif
	}

	ret = RT_OK;
end:
	return ret;
}

#ifdef RT_DEFINE_LINUX

/**
 * Translate Windows socket message flags into Linux flags.
 */
static rt_un rt_socket_get_message_flags(enum rt_socket_message_flag flags)
{
	rt_un result;

	result = 0;
	if (flags & RT_SOCKET_MESSAGE_FLAG_OUT_OF_BAND) result |= MSG_OOB;
	if (flags & RT_SOCKET_MESSAGE_FLAG_PEEK) result |= MSG_PEEK;
	if (flags & RT_SOCKET_MESSAGE_FLAG_DO_NOT_ROUTE) result |= MSG_DONTROUTE;
	if (flags & RT_SOCKET_MESSAGE_FLAG_WAIT_ALL) result |= MSG_WAITALL;

	return result;
}

#endif

rt_s rt_socket_send(struct rt_socket *socket, void *data, rt_un data_size, enum rt_socket_message_flag flags, rt_un *bytes_sent)
{
#ifdef RT_DEFINE_LINUX
	rt_un actual_flags;
#endif
	int send_result;
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_LINUX
	actual_flags = rt_socket_get_message_flags(flags);
#endif

#ifdef RT_DEFINE_WINDOWS
	/* Returns SOCKET_ERROR (-1) and sets WSAGetLastError in case of issue. */
	send_result = send((rt_un)socket->io_device.handle, data, (int)data_size, (int)flags);
	if (RT_UNLIKELY(send_result < 0))
		goto end;
#else
	/* Returns -1 in case of issue and sets errno. */
	send_result = send(socket->io_device.file_descriptor, data, data_size, actual_flags);
	if (send_result < 0) {
		if (RT_LIKELY(errno == EAGAIN))
			errno = EWOULDBLOCK;
		goto end;
	}
#endif

	*bytes_sent = send_result;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_socket_receive(struct rt_socket *socket, void *buffer, rt_un buffer_capacity, enum rt_socket_message_flag flags, rt_un *bytes_received)
{
#ifdef RT_DEFINE_LINUX
	rt_un actual_flags;
#endif
	int recv_result;
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_LINUX
	actual_flags = rt_socket_get_message_flags(flags);
#endif

#ifdef RT_DEFINE_WINDOWS
	/* Returns SOCKET_ERROR (-1) and sets WSAGetLastError in case of issue. */
	recv_result = recv((rt_un)socket->io_device.handle, buffer, (int)buffer_capacity, (int)flags);
	if (RT_UNLIKELY(recv_result < 0))
		goto end;
#else
	/* Returns -1 in case of issue and sets errno. */
	recv_result = recv(socket->io_device.file_descriptor, buffer, buffer_capacity, actual_flags);
	if (recv_result < 0) {
		if (RT_UNLIKELY(errno == EAGAIN))
			errno = EWOULDBLOCK;
		goto end;
	}
#endif

	*bytes_received = recv_result;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_socket_receive_all(struct rt_socket *socket, void *buffer, rt_un buffer_capacity, rt_un *bytes_received)
{
	rt_un received;
	rt_un total_bytes_received = 0;
	rt_s ret = RT_FAILED;

	while (RT_TRUE) {
		if (RT_UNLIKELY(!rt_socket_receive(socket, &((rt_char8*)buffer)[total_bytes_received], buffer_capacity - total_bytes_received, 0, &received)))
			goto end;

		if (!received) {
			/* Nothing more to read, return. */
			break;
		}

		total_bytes_received += received;

		if (total_bytes_received == buffer_capacity) {
			/* Buffer is full, return. */
			break;
		}
	}
	*bytes_received = total_bytes_received;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_socket_shutdown(struct rt_socket *socket, enum rt_socket_shutdown_flag flag)
{
	rt_s ret;

	/* On success shutdown returns zero, -1 and sets last error otherwise. */
#ifdef RT_DEFINE_WINDOWS
	ret = !shutdown((rt_un)socket->io_device.handle, (int)flag);
#else
	ret = !shutdown(socket->io_device.file_descriptor, flag);
#endif

	return ret;
}

rt_s rt_socket_free(struct rt_socket *socket)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	/* closesocket returns zero if the operation was successful, call WSASetLastError and returns SOCKET_ERROR otherwise. */
	ret = !closesocket((rt_un)socket->io_device.handle);
#else
	/* close() returns zero on success, and sets errno in case of issue. */
	ret = !close(socket->io_device.file_descriptor);
#endif
	return ret;
}

#ifdef RT_DEFINE_WINDOWS

rt_s rt_socket_cleanup(void)
{
	rt_s ret = RT_FAILED;

	/* WSACleanup returns zero if the operation was successful, calls WSASetLastError and returns SOCKET_ERROR otherwise. */
	if (RT_UNLIKELY(WSACleanup()))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

#else

rt_s rt_socket_cleanup(void)
{
	return RT_OK;
}

#endif

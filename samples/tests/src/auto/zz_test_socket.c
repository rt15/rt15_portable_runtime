#include <rpr.h>

#define ZZ_TEST_SOCKET_PORT_NUMBER 42334

struct zz_test_socket_server_parameter {
	struct rt_event event;
	enum rt_socket_address_family address_family;
};

/**
 * Thread that create a server socket and send "Hello, world!" to the first connecting socket.
 */
static rt_un32 RT_STDCALL zz_test_socket_server_callback(void *parameter)
{
	struct zz_test_socket_server_parameter *server_parameter = (struct zz_test_socket_server_parameter*)parameter;
	struct rt_socket socket;
	rt_b socket_created = RT_FALSE;
	rt_n32 option;
	rt_un option_size = sizeof(rt_n32);
	struct rt_socket accepted_socket;
	rt_b accepted_socket_created = RT_FALSE;
	rt_b shutdown_accepted_socket = RT_FALSE;
	rt_char8 *message;
	rt_un bytes_sent;
	rt_un32 ret;

	if (!rt_socket_create(&socket, server_parameter->address_family, RT_SOCKET_TYPE_STREAM, RT_SOCKET_PROTOCOL_TCP, RT_TRUE, RT_FALSE)) goto error;
	socket_created = RT_TRUE;

	/* Check REUSEADDR default value. */
	if (!rt_socket_get_option(&socket, RT_SOCKET_PROTOCOL_LEVEL_SOCKET, RT_SOCKET_OPTION_REUSEADDR, &option, &option_size)) goto error;
	if (option_size != sizeof(rt_n32)) goto error;
	if (option) goto error;

	/* Update REUSEADDR. */
	if (!rt_socket_set_boolean_option(&socket, RT_SOCKET_PROTOCOL_LEVEL_SOCKET, RT_SOCKET_OPTION_REUSEADDR, RT_TRUE)) goto error;

	/* Check new REUSEADDR value. */
	if (!rt_socket_get_option(&socket, RT_SOCKET_PROTOCOL_LEVEL_SOCKET, RT_SOCKET_OPTION_REUSEADDR, &option, &option_size)) goto error;
	if (option_size != sizeof(rt_n32)) goto error;
	if (!option) goto error;

	if (server_parameter->address_family == RT_SOCKET_ADDRESS_FAMILY_IPV6) {
		if (!rt_socket_set_ipv6_boolean_option(&socket, RT_SOCKET_PROTOCOL_LEVEL_IPV6, RT_SOCKET_IPV6_OPTION_V6ONLY, RT_TRUE)) goto error;
		if (!rt_socket_get_ipv6_option(&socket, RT_SOCKET_PROTOCOL_LEVEL_IPV6, RT_SOCKET_IPV6_OPTION_V6ONLY, &option, &option_size)) goto error;
		if (option_size != sizeof(rt_n32)) goto error;
		if (!option) goto error;
	}

	if (!rt_socket_bind(&socket, ZZ_TEST_SOCKET_PORT_NUMBER)) goto error;
	if (!rt_socket_listen(&socket)) goto error;

	if (!rt_event_signal(&server_parameter->event)) goto error;

	if (!rt_socket_accept_connection(&socket, &accepted_socket, RT_NULL, RT_NULL)) goto error;
	accepted_socket_created = RT_TRUE;
	shutdown_accepted_socket = RT_TRUE;

	message = "Hello";
	if (!rt_socket_send(&accepted_socket, message, rt_char8_get_size(message), 0, &bytes_sent)) goto error;
	if (bytes_sent != 5) goto error;

	/* Uncomment next line to separate responses. */
	/* rt_sleep_sleep(1000); */

	message = ", world!";
	if (!rt_socket_send(&accepted_socket, message, rt_char8_get_size(message) + 1, 0, &bytes_sent)) goto error;
	if (bytes_sent != 9) goto error;

	ret = RT_OK;
free:
	if (shutdown_accepted_socket) {
		shutdown_accepted_socket = RT_FALSE;
		if (!rt_socket_shutdown(&accepted_socket, RT_SOCKET_SHUTDOWN_FLAG_BOTH) && ret)
			goto error;
	}
	if (accepted_socket_created) {
		accepted_socket_created = RT_FALSE;
		if (!rt_socket_free(&accepted_socket) && ret)
			goto error;
	}
	if (socket_created) {
		socket_created = RT_FALSE;
		if (!rt_socket_free(&socket) && ret)
			goto error;
	}
	return ret;

error:
	/* Ensure that main thread will not wait for ever. */
	rt_event_signal(&server_parameter->event);
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_socket_client(enum rt_socket_address_family address_family)
{
	struct rt_socket socket;
	rt_b socket_created = RT_FALSE;
	struct rt_address_ipv4 ipv4_address;
	struct rt_socket_address_ipv4 ipv4_socket_address;
	struct rt_address_ipv6 ipv6_address;
	struct rt_socket_address_ipv6 ipv6_socket_address;
	struct rt_socket_address *socket_address;
	rt_char8 buffer[RT_CHAR_BIG_STRING_SIZE];
	rt_un received;
	rt_b shutdown_socket = RT_FALSE;
	rt_s ret;

	if (!rt_socket_create(&socket, address_family, RT_SOCKET_TYPE_STREAM, RT_SOCKET_PROTOCOL_TCP, RT_TRUE, RT_FALSE)) goto error;
	socket_created = RT_TRUE;

	if (address_family == RT_SOCKET_ADDRESS_FAMILY_IPV4) {
		rt_socket_address_create_ipv4_loopback_address(&ipv4_address);
		rt_socket_address_create_ipv4(&ipv4_socket_address, &ipv4_address, ZZ_TEST_SOCKET_PORT_NUMBER);
		socket_address = (struct rt_socket_address*)&ipv4_socket_address;
	} else {
		rt_socket_address_create_ipv6_loopback_address(&ipv6_address);
		rt_socket_address_create_ipv6(&ipv6_socket_address, &ipv6_address, ZZ_TEST_SOCKET_PORT_NUMBER);
		socket_address = (struct rt_socket_address*)&ipv6_socket_address;
	}
	if (!rt_socket_connect_with_socket_address(&socket, socket_address)) goto error;
	shutdown_socket = RT_TRUE;

	if (!rt_socket_receive_all(&socket, buffer, RT_CHAR_BIG_STRING_SIZE, &received)) goto error;
	if (received != rt_char8_get_size(buffer) + 1) goto error;
	if (!rt_char8_equals(buffer, received - 1, "Hello, world!", 13)) goto error;

	ret = RT_OK;
free:
	if (shutdown_socket) {
		shutdown_socket = RT_FALSE;
		if (!rt_socket_shutdown(&socket, RT_SOCKET_SHUTDOWN_FLAG_BOTH) && ret)
			goto error;
	}
	if (socket_created) {
		socket_created = RT_FALSE;
		if (!rt_socket_free(&socket) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

/**
 * Test socket assuming rt_socket_initialize/rt_socket_cleanup are called.
 */
static rt_s zz_test_socket_do(enum rt_socket_address_family address_family)
{
	struct zz_test_socket_server_parameter server_parameter;
	rt_b event_created = RT_FALSE;
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	rt_s ret;

	server_parameter.address_family = address_family;

	if (!rt_event_create(&server_parameter.event)) goto error;
	event_created = RT_TRUE;

	if (!rt_thread_create(&thread, &zz_test_socket_server_callback, &server_parameter)) goto error;
	thread_created = RT_TRUE;

	if (!rt_event_wait_for(&server_parameter.event)) goto error;

	/* Let some time for the server socket to be ready to accept connections. */
	rt_sleep_sleep(10);

	if (!zz_test_socket_client(address_family)) goto error;

	if (!rt_thread_join_and_check(&thread)) goto error;

	ret = RT_OK;
free:
	if (thread_created) {
		thread_created = RT_FALSE;
		if (!rt_thread_free(&thread) && ret)
			goto error;
	}
	if (event_created) {
		event_created = RT_FALSE;
		if (!rt_event_free(&server_parameter.event) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_socket()
{
	rt_b sockets_initialized;
	rt_s ret;

	sockets_initialized = RT_FALSE;

	if (!rt_socket_initialize()) goto error;
	sockets_initialized = RT_TRUE;

	if (!zz_test_socket_do(RT_SOCKET_ADDRESS_FAMILY_IPV4)) goto error;
	if (!zz_test_socket_do(RT_SOCKET_ADDRESS_FAMILY_IPV6)) goto error;

	ret = RT_OK;
free:
	if (sockets_initialized) {
		sockets_initialized = RT_FALSE;
		if (!rt_socket_cleanup() && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

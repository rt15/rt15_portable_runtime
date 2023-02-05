#include "layer007/rt_select.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"

static void rt_select_update_readiness(struct rt_select_item *items, rt_un items_size, fd_set *fds)
{
	rt_un i;

	if (items) {
		if (fds) {
			for (i = 0; i < items_size; i++) {
				if (FD_ISSET(items[i].file_descriptor, fds))
					items[i].ready = RT_TRUE;
				else
					items[i].ready = RT_FALSE;
			}
		} else {
			for (i = 0; i < items_size; i++)
				items[i].ready = RT_FALSE;
		}
	}
}

rt_s rt_select(struct rt_select_item *read_items,  rt_un read_items_size,
	       struct rt_select_item *write_items, rt_un write_items_size,
	       struct rt_select_item *except_items, rt_un except_items_size,
	       rt_un timeout_milliseconds, rt_un *ready_items_count)
{
	fd_set readfds;
	fd_set writefds;
	fd_set exceptfds;
	fd_set *actual_readfds;
	fd_set *actual_writefds;
	fd_set *actual_exceptfds;
#ifdef RT_DEFINE_WINDOWS
	rt_un file_descriptor;
#else
	rt_n32 file_descriptor;
#endif
	rt_n32 nfds = 0;
	struct timeval timeout;
	struct timeval *actual_timeout;
	rt_n32 select_result;
	rt_un i;
	rt_s ret;

	if (read_items_size  > RT_SELECT_MAX_ITEMS ||
	    write_items_size > RT_SELECT_MAX_ITEMS ||
	    except_items_size > RT_SELECT_MAX_ITEMS) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	if (read_items) {
		FD_ZERO(&readfds);
		for (i = 0; i < read_items_size; i++) {
			file_descriptor = read_items[i].file_descriptor;
			FD_SET(file_descriptor, &readfds);
#ifdef RT_DEFINE_LINUX
			if (file_descriptor > nfds)
				nfds = file_descriptor;
#endif
		}
		actual_readfds = &readfds;
	} else {
		actual_readfds = RT_NULL;
	}

	if (write_items) {
		FD_ZERO(&writefds);
		for (i = 0; i < write_items_size; i++) {
			file_descriptor = write_items[i].file_descriptor;
			FD_SET(file_descriptor, &writefds);
#ifdef RT_DEFINE_LINUX
			if (file_descriptor > nfds)
				nfds = file_descriptor;
#endif
		}
		actual_writefds = &writefds;
	} else {
		actual_writefds = RT_NULL;
	}

	if (except_items) {
		FD_ZERO(&exceptfds);
		for (i = 0; i < except_items_size; i++) {
			file_descriptor = except_items[i].file_descriptor;
			FD_SET(file_descriptor, &exceptfds);
#ifdef RT_DEFINE_LINUX
			if (file_descriptor > nfds)
				nfds = file_descriptor;
#endif
		}
		actual_exceptfds = &exceptfds;
	} else {
		actual_exceptfds = RT_NULL;
	}

	if (timeout_milliseconds) {
		timeout.tv_sec = (long)(timeout_milliseconds / 1000);
		timeout.tv_usec = (timeout_milliseconds % 1000) * 1000;
		actual_timeout = &timeout;
	} else {
		actual_timeout = RT_NULL;
	}

	/* nfds is either 0 or the highest-numbered file descriptor in any of the three sets, plus 1. */
	/* It is ignored under Windows. */
	if (nfds)
		nfds++;

	/* Returns the number of file descriptors in the sets. Can be zero in case of timeout. */
	/* Returns -1 and set errno in case of error. */
	select_result = select(nfds, actual_readfds, actual_writefds, actual_exceptfds, actual_timeout);
	if (select_result < 1)
		goto error;

	if (ready_items_count)
		*ready_items_count = select_result;

	if (select_result) {
		rt_select_update_readiness(read_items, read_items_size, actual_readfds);
		rt_select_update_readiness(write_items, write_items_size, actual_writefds);
		rt_select_update_readiness(except_items, except_items_size, actual_exceptfds);
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_select_wait_for_ready_to_read(struct rt_socket *socket, rt_un timeout_milliseconds)
{
#ifdef RT_DEFINE_WINDOWS
	rt_un file_descriptor = (rt_un)socket->io_device.handle;
#else
	rt_n32 file_descriptor = socket->io_device.file_descriptor;
#endif
	struct rt_select_item read_item;
	struct rt_select_item except_item;
	rt_n32 error;
	rt_un error_size = sizeof(rt_n32);
	rt_s ret;

	read_item.file_descriptor = file_descriptor;
	except_item.file_descriptor = file_descriptor;

	if (!rt_select(&read_item, 1, RT_NULL, 0, &except_item, 1, timeout_milliseconds, RT_NULL))
		goto error;

	if (except_item.ready) {
		if (!rt_socket_get_option(socket, RT_SOCKET_PROTOCOL_LEVEL_SOCKET, RT_SOCKET_OPTION_ERROR, &error, &error_size))
			goto error;

		if (error_size != sizeof(rt_n32)) {
			rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
			goto error;
		}

#ifdef RT_DEFINE_WINDOWS
		SetLastError(error);
#else
		errno = error;
#endif
		goto error;
	}

	if (!read_item.ready) {
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_select_wait_for_ready_to_write(struct rt_socket *socket, rt_un timeout_milliseconds)
{
#ifdef RT_DEFINE_WINDOWS
	rt_un file_descriptor = (rt_un)socket->io_device.handle;
#else
	rt_n32 file_descriptor = socket->io_device.file_descriptor;
#endif
	struct rt_select_item write_item;
	struct rt_select_item except_item;
	rt_n32 error;
	rt_un error_size = sizeof(rt_n32);
	rt_s ret;

	write_item.file_descriptor = file_descriptor;
	except_item.file_descriptor = file_descriptor;

	if (!rt_select(RT_NULL, 0, &write_item, 1, &except_item, 1, timeout_milliseconds, RT_NULL))
		goto error;

	if (except_item.ready) {
		if (!rt_socket_get_option(socket, RT_SOCKET_PROTOCOL_LEVEL_SOCKET, RT_SOCKET_OPTION_ERROR, &error, &error_size))
			goto error;

		if (error_size != sizeof(rt_n32)) {
			rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
			goto error;
		}

#ifdef RT_DEFINE_WINDOWS
		SetLastError(error);
#else
		errno = error;
#endif
		goto error;
	}

	if (!write_item.ready) {
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

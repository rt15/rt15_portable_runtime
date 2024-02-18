#include "layer007/rt_last_error_message.h"

#include "layer002/rt_error.h"
#include "layer002/rt_thread_local_storage.h"
#include "layer003/rt_char.h"
#include "layer003/rt_fast_initialization.h"
#include "layer004/rt_page_heap.h"
#include "layer005/rt_console.h"
#include "layer006/rt_error_message.h"

#define RT_LAST_ERROR_MESSAGE_SIZE 8000

static struct rt_fast_initialization rt_last_error_message_fast_initialization = RT_FAST_INITIALIZATION_STATIC_INIT;

static rt_b rt_last_error_message_fast_initialization_successful = RT_FALSE;

static struct rt_thread_local_storage rt_last_error_message_storage;

/**
 * Make sure the thread local storage is created. 
 */
static rt_s rt_last_error_message_init(void)
{
	rt_s ret;

	if (rt_fast_initialization_is_required(&rt_last_error_message_fast_initialization)) {

		/* No thread has created the thread local storage yet. */
		if (RT_UNLIKELY(!rt_thread_local_storage_create(&rt_last_error_message_storage))) {
			rt_fast_initialization_notify_done(&rt_last_error_message_fast_initialization);
			goto error;
		}

		rt_last_error_message_fast_initialization_successful = RT_TRUE;

		rt_fast_initialization_notify_done(&rt_last_error_message_fast_initialization);
	}

	if (RT_UNLIKELY(!rt_last_error_message_fast_initialization_successful)) {
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

/**
 * Retrieve a pointer to the local storage data allocating the space for this thread if necessary.
 */
static rt_s rt_last_error_message_get_buffer(rt_char **buffer)
{
	rt_char *local_storage;
	rt_s ret;

	if (RT_UNLIKELY(!rt_last_error_message_init()))
		goto error;

	/* Retrieve a pointer to this thread local storage. */
	if (RT_UNLIKELY(!rt_thread_local_storage_get(&rt_last_error_message_storage, (void**)&local_storage)))
		goto error;

	/* Maybe the buffer has not be allocated yet for this thread. */
	if (!local_storage) {
		/* This memory will be freed when the process is terminated. */
		if (RT_UNLIKELY(!rt_page_heap_alloc((void**)&local_storage, RT_LAST_ERROR_MESSAGE_SIZE)))
			goto error;
		
		/* Add a terminating zero in case the message is retrieve before it is set. */
		local_storage[0] = 0;

		if (RT_UNLIKELY(!rt_thread_local_storage_set(&rt_last_error_message_storage, local_storage)))
			goto error;
	}

	*buffer = local_storage;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_last_error_message_set(const rt_char *message)
{
	rt_char *local_storage;
	rt_s ret;

	if (RT_UNLIKELY(!rt_last_error_message_get_buffer(&local_storage)))
		goto error;

	/* If the message is too long, it is truncated. */
	rt_char_copy(message, rt_char_get_size(message), local_storage, RT_LAST_ERROR_MESSAGE_SIZE);

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_last_error_message_set_with_last_error(void)
{
	rt_char *local_storage;
	rt_un local_storage_size;
	rt_s ret;

	if (RT_UNLIKELY(!rt_last_error_message_get_buffer(&local_storage)))
		goto error;

	/* Operating system error message are pretty small and should fit in the buffer. */
	local_storage_size = 0;
	if (RT_UNLIKELY(!rt_error_message_append_last(local_storage, RT_LAST_ERROR_MESSAGE_SIZE, &local_storage_size)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_last_error_message_append(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_char *local_storage;
	rt_un local_storage_size;
	rt_s ret;

	if (RT_UNLIKELY(!rt_last_error_message_get_buffer(&local_storage)))
		goto error;

	local_storage_size = rt_char_get_size(local_storage);
	if (RT_UNLIKELY(!rt_char_append(local_storage, local_storage_size, buffer, buffer_capacity, buffer_size)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_last_error_message_write(const rt_char *prefix)
{
	rt_char buffer[RT_CHAR_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret;

	if (prefix) {
		buffer_size = rt_char_get_size(prefix);
		if (RT_UNLIKELY(!rt_char_copy(prefix, buffer_size, buffer, RT_CHAR_BIG_STRING_SIZE)))
			goto error;
	} else {
		buffer_size = 0;
	}
	if (RT_UNLIKELY(!rt_last_error_message_append(buffer, RT_CHAR_BIG_STRING_SIZE , &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), buffer, RT_CHAR_BIG_STRING_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_console_write_error_with_size(buffer, buffer_size))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_last_error_message_cleanup_thread_buffer(void)
{
	void *local_storage;
	rt_s ret;

	if (rt_fast_initialization_is_done(&rt_last_error_message_fast_initialization)) {

		if (RT_UNLIKELY(!rt_last_error_message_init()))
			goto error;

		/* Retrieve a pointer to this thread local storage. */
		if (RT_UNLIKELY(!rt_thread_local_storage_get(&rt_last_error_message_storage, &local_storage)))
			goto error;

		ret = RT_OK;

		if (local_storage) {
			if (RT_UNLIKELY(!rt_thread_local_storage_set(&rt_last_error_message_storage, RT_NULL)))
				ret = RT_FAILED;

			if (RT_UNLIKELY(!rt_page_heap_free(&local_storage)))
				ret = RT_FAILED;
		}

	} else {
		ret = RT_OK;
	}

free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_last_error_message_cleanup(void)
{
	rt_s ret;

	if (rt_fast_initialization_is_done(&rt_last_error_message_fast_initialization)) {

		if (RT_UNLIKELY(!rt_last_error_message_init()))
			goto error;

		if (RT_UNLIKELY(rt_thread_local_storage_free(&rt_last_error_message_storage)))
			goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

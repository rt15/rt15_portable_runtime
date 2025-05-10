#include <rpr.h>

#define ZZ_SHARED_MEMORY_CHAT_MEMORY_SIZE 12288
#define ZZ_SHARED_MEMORY_CHAT_MEMORY_NAME _R("/zz_shared_memory_chat_memory")
#define ZZ_SHARED_MEMORY_CHAT_MUTEX_NAME _R("/zz_shared_memory_chat_mutex")

struct zz_shared_memory_chat_parameter {
	rt_un32 *size;
	rt_char *text;
	struct rt_shared_mutex *shared_mutex;
	rt_un32 previous_size;
	rt_b quit;
};

/**
 * Print new text from the shared memory, if any.
 * 
 * <p>
 * You must acquire the shared mutex before calling this function.
 * </p> 
 */
static rt_s zz_shared_memory_chat_print(struct zz_shared_memory_chat_parameter *parameter)
{
	rt_un32 *size = parameter->size;
	rt_char *text = parameter->text;
	rt_char *new_text;
	rt_s ret = RT_FAILED;

	if (*size != parameter->previous_size) {
		new_text = &text[parameter->previous_size];

		if (RT_UNLIKELY(!rt_console_set_color(RT_CONSOLE_COLOR_GREEN)))
			goto end;

		if (RT_UNLIKELY(!rt_console_write_str_with_size(new_text, *size - parameter->previous_size)))
			goto end;

		parameter->previous_size = *size;
	}

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!rt_console_reset_color()))
		ret = RT_FAILED;

	return ret;
}

static rt_un32 RT_STDCALL zz_shared_memory_chat_callback(void *parameter)
{
	struct zz_shared_memory_chat_parameter *shared_memory_chat_parameter = (struct zz_shared_memory_chat_parameter*)parameter;
	struct rt_shared_mutex *shared_mutex = shared_memory_chat_parameter->shared_mutex;
	rt_s ret = RT_FAILED;

	while (!shared_memory_chat_parameter->quit) {
		if (RT_UNLIKELY(!rt_shared_mutex_acquire(shared_mutex)))
			goto end;

		if (RT_UNLIKELY(!zz_shared_memory_chat_print(shared_memory_chat_parameter))) {
			rt_shared_mutex_release(shared_mutex);
			goto end;
		}

		if (RT_UNLIKELY(!rt_shared_mutex_release(shared_mutex)))
			goto end;

		rt_sleep_sleep(100);
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_shared_memory_chat(void)
{
	struct rt_shared_memory shared_memory;
	rt_b shared_memory_created = RT_FALSE;
	struct rt_shared_mutex shared_mutex;
	rt_b shared_mutex_created = RT_FALSE;
	rt_un32 *size;
	rt_char *text;
	rt_char *new_text;
	struct zz_shared_memory_chat_parameter parameter;
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_shared_memory_create(&shared_memory, ZZ_SHARED_MEMORY_CHAT_MEMORY_NAME, ZZ_SHARED_MEMORY_CHAT_MEMORY_SIZE, RT_SHARED_MEMORY_RIGHT_READ_WRITE)))
		goto end;
	shared_memory_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_shared_mutex_create(&shared_mutex, ZZ_SHARED_MEMORY_CHAT_MUTEX_NAME)))
		goto end;
	shared_mutex_created = RT_TRUE;

	size = shared_memory.area;
	text = (rt_char *)(size + 1);

	parameter.size = size;
	parameter.text = text;
	parameter.shared_mutex = &shared_mutex;
	parameter.previous_size = 0;
	parameter.quit = RT_FALSE;

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_shared_memory_chat_callback, &parameter)))
		goto end;
	thread_created = RT_TRUE;

	rt_sleep_yield();

	while (RT_TRUE) {
		if (RT_UNLIKELY(!rt_console_read_line(buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
			goto end;

		if (rt_char_equals(buffer, buffer_size, _R("quit"), 4)) {
			parameter.quit = RT_TRUE;
			break;
		}

		if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
			goto end;

		if (RT_UNLIKELY(!rt_shared_mutex_acquire(&shared_mutex)))
			goto end;

		/* Print existing text before adding our line. */
		if (RT_UNLIKELY(!zz_shared_memory_chat_print(&parameter))) {
			rt_shared_mutex_release(&shared_mutex);
			goto end;
		}

		new_text = &text[*size];
		if (RT_UNLIKELY(!rt_char_copy(buffer, buffer_size, new_text, ZZ_SHARED_MEMORY_CHAT_MEMORY_SIZE - sizeof(rt_un32) - *size))) {
			rt_shared_mutex_release(&shared_mutex);
			goto end;
		}
		*size = *size + (rt_un32)buffer_size;
		parameter.previous_size = *size;

		if (RT_UNLIKELY(!rt_shared_mutex_release(&shared_mutex)))
			goto end;
	}

	if (RT_UNLIKELY(!rt_thread_join_and_check(&thread)))
		goto end;

	ret = RT_OK;
end:
	if (thread_created) {
		if (RT_UNLIKELY(!rt_thread_free(&thread)))
			ret = RT_FAILED;
	}
	if (shared_mutex_created) {
		if (RT_UNLIKELY(!rt_shared_mutex_free(&shared_mutex)))
			ret = RT_FAILED;
	}
	if (shared_memory_created) {
		if (RT_UNLIKELY(!rt_shared_memory_free(&shared_memory)))
			ret = RT_FAILED;
	}

	return ret;
}

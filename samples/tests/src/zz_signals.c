#include <rpr.h>

#include "zz_utils.h"

struct zz_signals_thread_callback_parameter {
	rt_un sleep_value;
};

/**
 * Create a file in the tmp folder, with the timestamp in the name, and containing the event name.
 */
static rt_s zz_signals_create_tmp_file(const rt_char8 *event_name)
{
	rt_char buffer[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_char character;
	rt_char file_path[RT_FILE_PATH_SIZE];
	rt_un file_path_size;
	rt_n unix_time;
	struct rt_time_info time_info;
	rt_un i;
	rt_s ret = RT_FAILED;

	/* Write the current time in a buffer, ready to be included in the file name. */
	if (RT_UNLIKELY(!rt_time_get_unix_time(&unix_time))) goto end;
	if (RT_UNLIKELY(!rt_time_info_create_local(&time_info, unix_time))) goto end;
	buffer_size = 0;
	if (RT_UNLIKELY(!zz_append_time_info(&time_info, RT_NULL, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
	for (i = 0; i < buffer_size; i++) {
		character = buffer[i];
		if (character == _R('-') ||
		    character == _R(' ') ||
		    character == _R(':')) {
			buffer[i] = _R('_');
		}
	}

	/* Build the file path. */
	if (RT_UNLIKELY(!zz_get_tmp_dir(file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto end;
	if (RT_UNLIKELY(!rt_file_path_append_separator(file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append(_R("signal_"), 7, file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append(buffer, buffer_size, file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append(_R(".txt"), 4, file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto end;

	if (RT_UNLIKELY(!rt_small_file_write(file_path, RT_SMALL_FILE_MODE_TRUNCATE, event_name, rt_char8_get_size(event_name))))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_un32 RT_STDCALL zz_signals_thread_callback(void *parameter)
{
	struct zz_signals_thread_callback_parameter *signals_thread_callback_parameter = (struct zz_signals_thread_callback_parameter*)parameter;
	rt_b wait_for_signals = RT_TRUE;
	enum rt_signal_event signal_event;
	rt_s ret = RT_FAILED;

	while (wait_for_signals) {

		if (RT_UNLIKELY(!rt_console_write_str(_R("Waiting for signals...\n"))))
			goto end;

		if (RT_UNLIKELY(!rt_signal_wait(&signal_event)))
			goto end;

		switch (signal_event) {
		case RT_SIGNAL_EVENT_CANCEL_WAIT:
			if (RT_UNLIKELY(!rt_console_write_str(_R("RT_SIGNAL_EVENT_CANCEL_WAIT\n"))))
				goto end;
			wait_for_signals = RT_FALSE;
			break;
		case RT_SIGNAL_EVENT_INTERRUPT:
			if (RT_UNLIKELY(!rt_console_write_str(_R("RT_SIGNAL_EVENT_INTERRUPT\n"))))
				goto end;
			break;
		case RT_SIGNAL_EVENT_BREAK:
			if (RT_UNLIKELY(!rt_console_write_str(_R("RT_SIGNAL_EVENT_BREAK\n"))))
				goto end;
			break;
		case RT_SIGNAL_EVENT_CLOSE:
			rt_console_write_str(_R("RT_SIGNAL_EVENT_CLOSE\n"));
			if (RT_UNLIKELY(!zz_signals_create_tmp_file("RT_SIGNAL_EVENT_CLOSE\n")))
				goto end;
			break;
		case RT_SIGNAL_EVENT_TERMINATE:
			rt_console_write_str(_R("RT_SIGNAL_EVENT_TERMINATE\n"));
			if (RT_UNLIKELY(!zz_signals_create_tmp_file("RT_SIGNAL_EVENT_TERMINATE\n")))
				goto end;
			break;
		default:
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto end;
		}
		rt_sleep_sleep((rt_un32)signals_thread_callback_parameter->sleep_value);
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_signals_do(rt_un sleep_value)
{
	struct zz_signals_thread_callback_parameter parameter;
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	parameter.sleep_value = sleep_value;

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_signals_thread_callback, &parameter)))
		goto end;
	thread_created = RT_TRUE;

	rt_sleep_sleep(30000);

	if (RT_UNLIKELY(!rt_console_write_str(_R("End of the main program.\n"))))
		goto end;

	if (RT_UNLIKELY(!rt_signal_cancel_wait()))
		goto end;

	ret = RT_OK;
end:
	if (thread_created) {
		if (RT_UNLIKELY(!rt_thread_join_and_check(&thread)))
			ret = RT_FAILED;
		if (RT_UNLIKELY(!rt_thread_free(&thread)))
			ret = RT_FAILED;
	}

	return ret;
}

rt_s zz_signals(const rt_char *sleep_str)
{
	rt_un sleep_value;
	rt_b watching = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_char_convert_to_un(sleep_str, &sleep_value)))
		goto end;
	sleep_value = sleep_value * 1000;

	if (RT_UNLIKELY(!rt_signal_watch()))
		goto end;
	watching = RT_TRUE;

	if (RT_UNLIKELY(!zz_signals_do(sleep_value)))
		goto end;

	ret = RT_OK;
end:
	if (watching) {
		if (RT_UNLIKELY(!rt_signal_cleanup()))
			ret = RT_FAILED;
	}
	return ret;
}

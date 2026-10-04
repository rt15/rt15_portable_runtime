#include "layer004/rt_signal.h"

#include "layer001/rt_memory.h"
#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer003/rt_pipe.h"

static struct rt_pipe rt_signal_pipe;

#ifdef RT_DEFINE_LINUX

#define RT_SIGNAL_LINUX_SIGNALS_COUNT 4

static const rt_n32 rt_signal_signals[RT_SIGNAL_LINUX_SIGNALS_COUNT] = { SIGINT, SIGQUIT, SIGHUP, SIGTERM };
static struct sigaction rt_signal_action_configuration_backups[RT_SIGNAL_LINUX_SIGNALS_COUNT];

#endif

/**
 * Sends the signal event to the pipe so that it can be consumed by <tt>rt_signal_wait</tt>.
 */
static rt_s rt_signal_send_event(enum rt_signal_event signal_event)
{
	struct rt_output_stream *output_stream = &rt_signal_pipe.output_io_device.output_stream;
	rt_char8 byte;
	rt_s ret = RT_FAILED;

	byte = (rt_char8)signal_event;
	if (RT_UNLIKELY(!output_stream->write(output_stream, &byte, 1)))
		goto end;
	if (RT_UNLIKELY(!output_stream->flush(output_stream)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

#ifdef RT_DEFINE_WINDOWS

static rt_n32 RT_STDCALL rt_signal_handler(rt_un32 ctrl_type)
{
	enum rt_signal_event signal_event;
	rt_n32 ret;

	switch (ctrl_type) {
	case CTRL_C_EVENT:
		signal_event = RT_SIGNAL_EVENT_INTERRUPT;
		ret = TRUE;
		break;
	case CTRL_BREAK_EVENT:
		signal_event = RT_SIGNAL_EVENT_BREAK;
		ret = TRUE;
		break;
	case CTRL_CLOSE_EVENT:
		signal_event = RT_SIGNAL_EVENT_CLOSE;
		ret = TRUE;
		break;
	default:
		/* Initialize signal_event with an arbitrary value. */
		signal_event = RT_SIGNAL_EVENT_CANCEL_WAIT;
		ret = FALSE;
	}

	if (ret) {
		if (RT_UNLIKELY(!rt_signal_send_event(signal_event)))
			ret = FALSE;
	}

	/* In case of CTRL_CLOSE_EVENT, Windows stops the process when the handler returns. */
	if (ret && ctrl_type == CTRL_CLOSE_EVENT) {
		Sleep(INFINITE);
	}

	/* Notify whether we handled the signal. */
	return ret;
}

#else

static void RT_CDECL rt_signal_handler(rt_n32 signo)
{
	struct rt_io_device *output_io_device = &rt_signal_pipe.output_io_device;
	enum rt_signal_event signal_event = RT_SIGNAL_EVENT_CANCEL_WAIT;
	rt_char8 byte;
	rt_n32 errno_backup = errno;
	rt_n written;

	switch (signo) {
	case SIGINT:
		signal_event = RT_SIGNAL_EVENT_INTERRUPT;
		break;
	case SIGQUIT:
		signal_event = RT_SIGNAL_EVENT_BREAK;
		break;
	case SIGHUP:
		signal_event = RT_SIGNAL_EVENT_CLOSE;
		break;
	case SIGTERM:
		signal_event = RT_SIGNAL_EVENT_TERMINATE;
		break;
	}

	if (signal_event != RT_SIGNAL_EVENT_CANCEL_WAIT) {
		byte = (rt_char8)signal_event;
		/* There can be a warning if we don't use the value returned by write. */
		written = write(output_io_device->file_descriptor, &byte, 1);
		(void)written;
	}

	errno = errno_backup;
}

#endif

rt_s rt_signal_watch(void)
{
	rt_b pipe_created = RT_FALSE;
#ifdef RT_DEFINE_LINUX
	rt_n32 output_file_descriptor;
	rt_n32 flags;
	rt_b restore_action_configurations = RT_FALSE;
	struct sigaction action_configuration;
	rt_un i;
#endif
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_pipe_create(&rt_signal_pipe)))
		goto end;
	pipe_created = RT_TRUE;

#ifdef RT_DEFINE_WINDOWS

	/* Returns zero and set last error in case of issue. */
	if (RT_UNLIKELY(!SetConsoleCtrlHandler(&rt_signal_handler, TRUE)))
		goto end;

#else

	/* The handler must never block. If the pipe is full, the signal is dropped. */
	/* fcntl returns -1 and sets errno in case of issue. */
	output_file_descriptor = rt_signal_pipe.output_io_device.file_descriptor;
	flags = fcntl(output_file_descriptor, F_GETFL);
	if (RT_UNLIKELY(flags == -1)) goto end;
	if (RT_UNLIKELY(fcntl(output_file_descriptor, F_SETFL, flags | O_NONBLOCK) == -1)) goto end;

	/* Backup the existing configuration. */
	for (i = 0; i < RT_SIGNAL_LINUX_SIGNALS_COUNT; i++) {
		/* Returns -1 and set errno in case of issue. */
		if (RT_UNLIKELY(sigaction(rt_signal_signals[i], RT_NULL, &rt_signal_action_configuration_backups[i]) == -1))
			goto end;
	}
	restore_action_configurations = RT_TRUE;

	RT_MEMORY_ZERO(&action_configuration, sizeof(struct sigaction));
	action_configuration.sa_handler = &rt_signal_handler;
	/* Block the other watched signals while the handler is executed, so that the handler is not re-entered. */
	/* sigemptyset and sigaddset return -1 and set errno in case of issue. */
	if (RT_UNLIKELY(sigemptyset(&action_configuration.sa_mask) == -1))
		goto end;
	for (i = 0; i < RT_SIGNAL_LINUX_SIGNALS_COUNT; i++) {
		if (RT_UNLIKELY(sigaddset(&action_configuration.sa_mask, rt_signal_signals[i]) == -1))
			goto end;
	}
	action_configuration.sa_flags = SA_RESTART;

	for (i = 0; i < RT_SIGNAL_LINUX_SIGNALS_COUNT; i++) {
		/* Keep ignored signals ignored, for example SIGHUP when the process is started with nohup. */
		if (rt_signal_action_configuration_backups[i].sa_handler == SIG_IGN)
			continue;
		/* Returns -1 and set errno in case of issue. */
		if (RT_UNLIKELY(sigaction(rt_signal_signals[i], &action_configuration, RT_NULL) == -1))
			goto end;
	}

#endif

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!ret)) {

#ifdef RT_DEFINE_LINUX

		if (restore_action_configurations) {
			for (i = 0; i < RT_SIGNAL_LINUX_SIGNALS_COUNT; i++) {
				sigaction(rt_signal_signals[i], &rt_signal_action_configuration_backups[i], RT_NULL);
			}
		}

#endif

		if (pipe_created)
			rt_pipe_free(&rt_signal_pipe);
	}
	return ret;
}

rt_s rt_signal_wait(enum rt_signal_event *signal_event)
{
	rt_char8 byte;
	rt_un bytes_read;
	rt_s ret = RT_FAILED;
	struct rt_input_stream *input_stream = &rt_signal_pipe.input_io_device.input_stream;

	if (RT_UNLIKELY(!input_stream->read(input_stream, &byte, 1, &bytes_read)))
		goto end;

	if (RT_UNLIKELY(bytes_read != 1)) {
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto end;
	}

	*signal_event = (enum rt_signal_event)byte;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_signal_cancel_wait(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_signal_send_event(RT_SIGNAL_EVENT_CANCEL_WAIT)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_signal_cleanup(void)
{
#ifdef RT_DEFINE_LINUX
	rt_un i;
#endif
	rt_s ret = RT_OK;

#ifdef RT_DEFINE_WINDOWS

	/* Returns zero and set last error in case of issue. */
	if (RT_UNLIKELY(!SetConsoleCtrlHandler(&rt_signal_handler, FALSE)))
		ret = RT_FAILED;

#else

	for (i = 0; i < RT_SIGNAL_LINUX_SIGNALS_COUNT; i++) {
		/* Returns -1 and set errno in case of issue. */
		if (RT_UNLIKELY(sigaction(rt_signal_signals[i], &rt_signal_action_configuration_backups[i], RT_NULL) == -1))
			ret = RT_FAILED;
	}

#endif

	if (RT_UNLIKELY(!rt_pipe_free(&rt_signal_pipe)))
		ret = RT_FAILED;

	return ret;
}

#ifndef RT_SIGNAL_H
#define RT_SIGNAL_H

#include "layer000/rt_types.h"

/**
 * @file
 * Allows the application to be notified of signals.
 *
 * <p>
 * When an application should stop, the operating system sends a signal to it:<br>
 * If the user presses Ctrl+C.<br>
 * If the user closes the window.<br>
 * ...
 * </p>
 *
 * <p>
 * Windows implementation details:<br>
 * This works for console applications only, not for GUI applications.<br>
 * <tt>SetConsoleCtrlHandler</tt> is used to register a signal handler.<br>
 * When a signal is sent to the application, Windows creates a new thread and calls the handlers.<br>
 * For signals other than Ctrl+C and Ctrl+Break, the application will be killed after a timeout (typically 5 seconds), so handle the signal quickly.<br>
 * The signals are:
 * </p>
 * <ul>
 *   <li>CTRL_C_EVENT: Ctrl+C.</li>
 *   <li>CTRL_BREAK_EVENT: Ctrl+Break.</li>
 *   <li>CTRL_CLOSE_EVENT: the console window is about to be closed.</li>
 *   <li>CTRL_LOGOFF_EVENT: the user is logging off. Sent for services only, and we don't know which user. Not supported.</li>
 *   <li>CTRL_SHUTDOWN_EVENT: the system is shutting down. Sent for services only. Not supported.</li>
 * </ul>
 *
 * <p>
 * Linux implementation details:<br>
 * <tt>sigaction</tt> is used to install a signal handler function.<br>
 * Linux picks an existing thread of the process to execute the handler.<br>
 * The handler is limited and can only rely on "async-signal-safe functions".<br>
 * The handler must also preserve errno.<br>
 * SA_RESTART is used so that most interrupted system calls are restarted automatically.<br>
 * Signals that are already ignored when <tt>rt_signal_watch</tt> is called stay ignored and are never received.<br>
 * For example SIGHUP is ignored if the process is started with <tt>nohup</tt>.<br>
 * The signals supported by this library are:
 * </p>
 * <ul>
 *   <li>SIGINT: Ctrl+C, interruption.</li>
 *   <li>SIGQUIT: Ctrl+\, heavy interruption, generate a core dump by default.</li>
 *   <li>SIGHUP: Terminal closed.</li>
 *   <li>SIGTERM: Terminate the program.</li>
 * </ul>
 */

enum rt_signal_event {
	/* Sent when <tt>rt_signal_cancel_wait</tt> is called to wake up the thread waiting for signals. */
	RT_SIGNAL_EVENT_CANCEL_WAIT,
	/* Ctrl+C. */
	RT_SIGNAL_EVENT_INTERRUPT,
	/* Ctrl+Break on Windows, Ctrl+\ on Linux. */
	RT_SIGNAL_EVENT_BREAK,
	/* The console is being closed. */
	RT_SIGNAL_EVENT_CLOSE,
	/* The application must stop. Never received under Windows. */
	RT_SIGNAL_EVENT_TERMINATE
};

/**
 * Should be called once early when the process is started.<br>
 * Then create a thread and call <tt>rt_signal_wait</tt>.
 *
 * <p>
 * Ctrl+C and other signals do not stop the application anymore after a call to this function.
 * </p>
 */
RT_API rt_s rt_signal_watch(void);

/**
 * Blocking function waiting for a signal.<br>
 * Returns one signal event per call, in the order of arrival.
 *
 * <p>
 * Only a single thread should be blocked on this function at a given time.
 * </p>
 */
RT_API rt_s rt_signal_wait(enum rt_signal_event *signal_event);

/**
 * Sends <tt>RT_SIGNAL_EVENT_CANCEL_WAIT</tt> to the thread waiting for signals.
 */
RT_API rt_s rt_signal_cancel_wait(void);

/**
 * <p>
 * Must be called once at the end.<br>
 * No thread should be blocked in <tt>rt_signal_wait</tt> when calling this function.
 * </p>
 */
RT_API rt_s rt_signal_cleanup(void);

#endif /* RT_SIGNAL_H */

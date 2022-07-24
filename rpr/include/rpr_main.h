#ifndef RPR_MAIN_H
#define RPR_MAIN_H

#include "layer000/rt_types.h"

/**
 * @file
 * This special header can be used by the consumer to easily write its application entry point.<br>
 * If the CRT is not used, then the compiler expect the application to provide either WinMainCRTStartup or mainCRTStartup.<br>
 * This header provides a unified entry point with argc/argv.<br>
 * It is optional, the rpr library can be used without defining a rpr_main entry point.
 *
 * <p>
 * It is an header as the library cannot export symbols like main/WinMainCRTStartup/mainCRTStartup.<br>
 * It must not be included in the library .c files.
 * </p>
 *
 * <p>
 * Customer application should never call directly ExitProcess/exit to terminate.<br>
 * It should return from the main method instead.
 * </p>
 *
 * <p>
 * Define RT_DEFINE_USE_CRT when your application is using CRT on Windows. CRT is always used on linux.
 * Under Windows, either _CONSOLE or _WINDOWS (For a Windows GUI) must be defined.
 * </p>
 */

/**
 * Declaration that must be used for API consumer entry point.
 *
 * @param argc Size of <tt>argv</tt>.
 * @param argv Program arguments. <tt>argv[0]</tt> is the name of the program if <tt>argv</tt> is greater than zero.
 * @return Process exit code. Should be zero if the execution is a success.
 */
rt_un16 rpr_main(rt_un argc, const rt_char *argv[]);

#ifdef RT_DEFINE_WINDOWS

#if defined(_WINDOWS) && defined(_CONSOLE)
#error Both _WINDOWS and _CONSOLE are defined.
#endif

#if !defined(_WINDOWS) && !defined(_CONSOLE)
#error Either _WINDOWS or _CONSOLE must be defined.
#endif

/**
 * Should not be use outside this file.
 */
void rpr_main_exit_process(rt_un32 result);

/**
 * Should not be use outside this file.
 */
void rpr_main_local_free(rt_char **argv);

/**
 * Should not be use outside this file.
 */
rt_char **rpr_main_command_line_to_argv_w(rt_n32 *argc);

#else

void rpr_main_set_locale();

#endif

#ifdef RT_DEFINE_WINDOWS
#ifdef RT_DEFINE_USE_CRT
int RT_CDECL main(int argc, char* argv[])
#else /* NOT RT_DEFINE_USE_CRT */
#ifdef _WINDOWS
int RT_CDECL WinMainCRTStartup()
#else /* NOT _WINDOWS */
int RT_CDECL mainCRTStartup()
#endif
#endif
{
	rt_char **argv;
	rt_n32 argc;
	rt_un32 ret;

	argv = rpr_main_command_line_to_argv_w(&argc);
	ret = rpr_main((rt_un)argc, (const rt_char**)argv);
	rpr_main_local_free(argv);

#ifndef RT_DEFINE_USE_CRT
	/* It is safer to call ExitProcess instead of simply returning from the entry point (There might be other threads). */
	rpr_main_exit_process(ret);
#endif
	/* Let CRT clean up. */
	return ret;
}
#else
int RT_CDECL main(int argc, char* argv[])
{
	/* Default locale is "C". This call will align locale with system locale. */
	rpr_main_set_locale();
	return rpr_main((rt_un)argc, (const rt_char**)argv);
}
#endif

#endif /* RPR_MAIN_H */

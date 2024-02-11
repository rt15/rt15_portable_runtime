#include <rpr.h>

rt_s zz_manual_test_process()
{
	const rt_char *application_path_and_args[5];
	rt_un32 exit_code;
	rt_s ret;

	application_path_and_args[0] = _R("ping");
	application_path_and_args[1] = _R("localhost");

#ifdef RT_DEFINE_WINDOWS
	application_path_and_args[2] = _R("-n");
#else
	application_path_and_args[2] = _R("-c");
#endif

	application_path_and_args[3] = _R("3");
	application_path_and_args[4] = RT_NULL;

	if (RT_UNLIKELY(!rt_console_write_str(_R("==========================")))) goto error;

#ifdef RT_DEFINE_LINUX
	if (RT_UNLIKELY(!rt_console_write_str_with_size(_R("\n"), 1))) goto error;
#endif

	if (RT_UNLIKELY(!rt_process_spawn_sync(RT_NULL, RT_NULL, application_path_and_args, &exit_code))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("==========================\n")))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("Joined!\n")))) goto error;
	if (RT_UNLIKELY(exit_code)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

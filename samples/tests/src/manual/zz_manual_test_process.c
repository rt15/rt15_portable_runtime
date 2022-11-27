#include <rpr.h>

rt_s zz_manual_test_process()
{
	struct rt_process process;
	rt_b process_created = RT_FALSE;
	const rt_char *application_path_and_args[5];
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

	if (!rt_console_write_string(_R("==========================")))
		goto error;

#ifdef RT_DEFINE_LINUX
	if (!rt_console_write_string_with_size(_R("\n"), 1))
		goto error;
#endif

	if (!rt_process_create(&process, RT_TRUE, RT_NULL, RT_NULL, application_path_and_args))
		goto error;
	process_created = RT_TRUE;

	ret = RT_OK;
free:
	if (process_created) {
		if (!rt_process_join(&process) && ret)
			goto error;

		if (!rt_console_write_string(_R("==========================\n")) && ret)
			goto error;
		if (!rt_console_write_string(_R("Joined!\n")) && ret)
			goto error;

		process_created = RT_FALSE;
		if (!rt_process_free(&process) && ret)
			goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

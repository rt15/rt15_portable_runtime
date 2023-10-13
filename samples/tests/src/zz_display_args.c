#include <rpr.h>

rt_s zz_display_args(rt_un argc, const rt_char *argv[])
{
	rt_un i;
	rt_s ret;

	for (i = 0; i < argc; i++) {
		if (RT_UNLIKELY(!rt_console_write_string(argv[i]))) goto error;
		if (RT_UNLIKELY(!rt_console_write_string(_R("\n")))) goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

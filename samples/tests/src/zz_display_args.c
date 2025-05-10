#include <rpr.h>

rt_s zz_display_args(rt_un argc, const rt_char *argv[])
{
	rt_un i;
	rt_s ret = RT_FAILED;

	for (i = 0; i < argc; i++) {
		if (RT_UNLIKELY(!rt_console_write_str(argv[i]))) goto end;
		if (RT_UNLIKELY(!rt_console_write_str(_R("\n")))) goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

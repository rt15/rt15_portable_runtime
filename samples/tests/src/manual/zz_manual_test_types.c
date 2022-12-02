#include <rpr.h>

rt_s zz_manual_test_types_display_signedness()
{
	rt_char rt_char_variable = -1;
	rt_s ret;

#ifdef RT_DEFINE_GCC
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
	if (rt_char_variable < 0) {
		if (!rt_console_write_string(_R("rt_char is signed.\n")))
			goto error;
	} else {
		if (!rt_console_write_string(_R("rt_char is unsigned.\n")))
			goto error;
	}
#ifdef RT_DEFINE_GCC
#pragma GCC diagnostic pop
#endif

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_manual_test_types()
{
	rt_s ret;

	if (!zz_manual_test_types_display_signedness()) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

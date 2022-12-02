#include <rpr.h>

static rt_s zz_manual_test_types_display_flags()
{
	rt_s ret;

	if (!rt_console_write_string(_R("## Flags:\n"))) goto error;
#ifdef RT_DEFINE_DEBUG
	if (!rt_console_write_string(_R("  Debug build.\n"))) goto error;
#else
	if (!rt_console_write_string(_R("  Release build.\n"))) goto error;
#endif

#ifdef RT_DEFINE_32
	if (!rt_console_write_string(_R("  Architecture = 32 bits\n"))) goto error;
#else
	if (!rt_console_write_string(_R("  Architecture = 64 bits\n"))) goto error;
#endif

#ifdef RT_DEFINE_WINDOWS
	if (!rt_console_write_string(_R("  Operating system = Windows\n"))) goto error;
#else
	if (!rt_console_write_string(_R("  Operating system = Linux\n"))) goto error;
#endif

#ifdef RT_DEFINE_VC
	if (!rt_console_write_string(_R("  Compiler = Visual C++\n"))) goto error;
#else
	if (!rt_console_write_string(_R("  Compiler = GCC\n"))) goto error;
#endif

#ifdef RT_DEFINE_USE_CRT
	if (!rt_console_write_string(_R("  Use CRT = true\n"))) goto error;
#else
	if (!rt_console_write_string(_R("  Use CRT = false\n"))) goto error;
#endif
	if (!rt_console_write_string_with_size(_R("\n"), 1)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_manual_test_types_display_signedness()
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

	if (!zz_manual_test_types_display_flags()) goto error;
	if (!zz_manual_test_types_display_signedness()) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
#include <rpr.h>

static rt_s zz_manual_test_types_display_flags(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_console_write_str(_R("## Flags:\n")))) goto end;
#ifdef RT_DEFINE_DEBUG
	if (RT_UNLIKELY(!rt_console_write_str(_R("  Debug build.\n")))) goto end;
#else
	if (RT_UNLIKELY(!rt_console_write_str(_R("  Release build.\n")))) goto end;
#endif

#ifdef RT_DEFINE_32
	if (RT_UNLIKELY(!rt_console_write_str(_R("  Architecture = 32 bits\n")))) goto end;
#else
	if (RT_UNLIKELY(!rt_console_write_str(_R("  Architecture = 64 bits\n")))) goto end;
#endif

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!rt_console_write_str(_R("  Operating system = Windows\n")))) goto end;
#else
	if (RT_UNLIKELY(!rt_console_write_str(_R("  Operating system = Linux\n")))) goto end;
#endif

#ifdef RT_DEFINE_VC
	if (RT_UNLIKELY(!rt_console_write_str(_R("  Compiler = Visual C++\n")))) goto end;
#else
	if (RT_UNLIKELY(!rt_console_write_str(_R("  Compiler = GCC\n")))) goto end;
#endif

#ifdef RT_DEFINE_USE_CRT
	if (RT_UNLIKELY(!rt_console_write_str(_R("  Use CRT = true\n")))) goto end;
#else
	if (RT_UNLIKELY(!rt_console_write_str(_R("  Use CRT = false\n")))) goto end;
#endif
	if (RT_UNLIKELY(!rt_console_write_str_with_size(_R("\n"), 1))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_manual_test_types_display_signedness(void)
{
	rt_char rt_char_variable = -1;
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_GCC
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
	if (rt_char_variable < 0) {
		if (RT_UNLIKELY(!rt_console_write_str(_R("rt_char is signed.\n"))))
			goto end;
	} else {
		if (RT_UNLIKELY(!rt_console_write_str(_R("rt_char is unsigned.\n"))))
			goto end;
	}
#ifdef RT_DEFINE_GCC
#pragma GCC diagnostic pop
#endif

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_manual_test_types_display_page_size(void)
{
	rt_un page_size;
	rt_char buffer[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret = RT_FAILED;

	page_size = rt_virtual_memory_get_page_size();
	buffer_size = 12;
	if (RT_UNLIKELY(!rt_char_copy(_R("Page size = "), buffer_size, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE))) goto end;
	if (RT_UNLIKELY(!rt_char_append_un(page_size, 10, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size))) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_manual_test_types(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_manual_test_types_display_flags())) goto end;
	if (RT_UNLIKELY(!zz_manual_test_types_display_signedness())) goto end;
	if (RT_UNLIKELY(!zz_manual_test_types_display_page_size())) goto end;

	ret = RT_OK;
end:
	return ret;
}

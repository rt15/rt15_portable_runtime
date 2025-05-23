#include <rpr.h>

static rt_uchar8 zz_iso_88591[3] = { 0xE9, 0xE8, 0x00 };
static rt_uchar8 zz_utf8[5] = { 0xC3, 0xA9, 0xC3, 0xA8, 0x00 };

static rt_s zz_manual_test_write_str(const rt_char *prefix, const rt_char8 *str, enum rt_encoding encoding)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_console_write_str(prefix))) goto end;
	if (RT_UNLIKELY(!rt_console_write_str(_R(": O")))) goto end;
	if (RT_UNLIKELY(!rt_console8_write_str(str, encoding))) goto end;
	if (RT_UNLIKELY(!rt_console_write_str(_R("O\n")))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_manual_test_console8_do(const rt_char *encoding_name, const rt_char8 *str, enum rt_encoding encoding)
{
	rt_char8 buffer[RT_CHAR8_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_manual_test_write_str(encoding_name, str, encoding))) goto end;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Reading ")))) goto end;
	if (RT_UNLIKELY(!rt_console_write_str(encoding_name))) goto end;
	if (RT_UNLIKELY(!rt_console_write_str(_R(":")))) goto end;

	if (RT_UNLIKELY(!rt_console8_read_line(encoding, buffer, RT_CHAR8_BIG_STRING_SIZE, &buffer_size))) goto end;

	if (RT_UNLIKELY(!zz_manual_test_write_str(_R("Read"), buffer, encoding))) goto end;

	ret = RT_OK;
end:
	return ret;
}


rt_s zz_manual_test_console8(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_manual_test_console8_do(_R("UTF-8"), (rt_char8*)zz_utf8, RT_ENCODING_UTF_8))) goto end;
	if (RT_UNLIKELY(!zz_manual_test_console8_do(_R("ISO-8859-1"), (rt_char8*)zz_iso_88591, RT_ENCODING_ISO_8859_1))) goto end;

	ret = RT_OK;
end:
	return ret;
}

#include <rpr.h>

static rt_s zz_test_encoding_get_info_do(enum rt_encoding encoding, const rt_char *encoding_enum)
{
	struct rt_encoding_info encoding_info;
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret;

	if (!rt_encoding_get_info(encoding, &encoding_info))
		goto error;

	buffer_size = rt_char_get_size(encoding_enum);
	if (!rt_char_copy(encoding_enum, buffer_size, buffer, RT_CHAR_HALF_BIG_STRING_SIZE)) goto error;
	if (!rt_char_append(_R(": "), 2, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)) goto error;
	if (!rt_char_append(encoding_info.name, rt_char_get_size(encoding_info.name), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)) goto error;
	if (!rt_char_append_char(_R('('), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)) goto error;
	if (!rt_char_append_n(encoding_info.code_page, 10, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)) goto error;
	if (!rt_char_append(_R(") \""), 3, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)) goto error;
	if (!rt_char_append(encoding_info.label, rt_char_get_size(encoding_info.label), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)) goto error;
	if (!rt_char_append(_R("\" "), 2, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)) goto error;
	if (!rt_char_append_n(encoding_info.code_unit_size, 10, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)) goto error;
	if (!rt_char_append_char(_R(','), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)) goto error;
	if (!rt_char_append_n(encoding_info.max_code_point_size, 10, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)) goto error;
	if (!rt_char_append_char(_R('\n'), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)) goto error;

	if (!rt_console_write_string_with_size(buffer, buffer_size))
		goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_encoding_get_info()
{
	rt_s ret;

	if (!zz_test_encoding_get_info_do(RT_ENCODING_SYSTEM_DEFAULT,	_R("RT_ENCODING_SYSTEM_DEFAULT")))	goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM037,		_R("RT_ENCODING_IBM037")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM437,		_R("RT_ENCODING_IBM437")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM500,		_R("RT_ENCODING_IBM500")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_ASMO_708,		_R("RT_ENCODING_ASMO_708")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM737,		_R("RT_ENCODING_IBM737")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM775,		_R("RT_ENCODING_IBM775")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM850,		_R("RT_ENCODING_IBM850")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM852,		_R("RT_ENCODING_IBM852")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM855,		_R("RT_ENCODING_IBM855")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM857,		_R("RT_ENCODING_IBM857")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM860,		_R("RT_ENCODING_IBM860")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM861,		_R("RT_ENCODING_IBM861")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM863,		_R("RT_ENCODING_IBM863")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM864,		_R("RT_ENCODING_IBM864")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM865,		_R("RT_ENCODING_IBM865")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_CP866,		_R("RT_ENCODING_CP866")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM869,		_R("RT_ENCODING_IBM869")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM870,		_R("RT_ENCODING_IBM870")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_WINDOWS_874,	_R("RT_ENCODING_WINDOWS_874")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_CP875,		_R("RT_ENCODING_CP875")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_SHIFT_JIS,	_R("RT_ENCODING_SHIFT_JIS")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_GB2312,		_R("RT_ENCODING_GB2312")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_CP949,		_R("RT_ENCODING_CP949")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_BIG5,		_R("RT_ENCODING_BIG5")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM1026,		_R("RT_ENCODING_IBM1026")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM1047,		_R("RT_ENCODING_IBM1047")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_UTF_16,		_R("RT_ENCODING_UTF_16")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_UTF_16LE,		_R("RT_ENCODING_UTF_16LE")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_UTF_16BE,		_R("RT_ENCODING_UTF_16BE")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_WINDOWS_1250,	_R("RT_ENCODING_WINDOWS_1250")))	goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_WINDOWS_1251,	_R("RT_ENCODING_WINDOWS_1251")))	goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_WINDOWS_1252,	_R("RT_ENCODING_WINDOWS_1252")))	goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_WINDOWS_1253,	_R("RT_ENCODING_WINDOWS_1253")))	goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_WINDOWS_1254,	_R("RT_ENCODING_WINDOWS_1254")))	goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_WINDOWS_1255,	_R("RT_ENCODING_WINDOWS_1255")))	goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_WINDOWS_1256,	_R("RT_ENCODING_WINDOWS_1256")))	goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_WINDOWS_1257,	_R("RT_ENCODING_WINDOWS_1257")))	goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_WINDOWS_1258,	_R("RT_ENCODING_WINDOWS_1258")))	goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_CP1361,		_R("RT_ENCODING_CP1361")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_MAC,		_R("RT_ENCODING_MAC")))			goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_UTF_32,		_R("RT_ENCODING_UTF_32")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_UTF_32LE,		_R("RT_ENCODING_UTF_32LE")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_UTF_32BE,		_R("RT_ENCODING_UTF_32BE")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_US_ASCII,		_R("RT_ENCODING_US_ASCII")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_T_61,		_R("RT_ENCODING_T_61")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_ISO_6937,		_R("RT_ENCODING_ISO_6937")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM273,		_R("RT_ENCODING_IBM273")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM277,		_R("RT_ENCODING_IBM277")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM278,		_R("RT_ENCODING_IBM278")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM280,		_R("RT_ENCODING_IBM280")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM284,		_R("RT_ENCODING_IBM284")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM285,		_R("RT_ENCODING_IBM285")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM297,		_R("RT_ENCODING_IBM297")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM420,		_R("RT_ENCODING_IBM420")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM423,		_R("RT_ENCODING_IBM423")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM424,		_R("RT_ENCODING_IBM424")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_KOI8_R,		_R("RT_ENCODING_KOI8_R")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM871,		_R("RT_ENCODING_IBM871")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM880,		_R("RT_ENCODING_IBM880")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_IBM905,		_R("RT_ENCODING_IBM905")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_EUC_JP,		_R("RT_ENCODING_EUC_JP")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_KOI8_U,		_R("RT_ENCODING_KOI8_U")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_ISO_8859_1,	_R("RT_ENCODING_ISO_8859_1")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_ISO_8859_2,	_R("RT_ENCODING_ISO_8859_2")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_ISO_8859_3,	_R("RT_ENCODING_ISO_8859_3")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_ISO_8859_4,	_R("RT_ENCODING_ISO_8859_4")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_ISO_8859_5,	_R("RT_ENCODING_ISO_8859_5")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_ISO_8859_6,	_R("RT_ENCODING_ISO_8859_6")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_ISO_8859_7,	_R("RT_ENCODING_ISO_8859_7")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_ISO_8859_8,	_R("RT_ENCODING_ISO_8859_8")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_ISO_8859_9,	_R("RT_ENCODING_ISO_8859_9")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_ISO_8859_13,	_R("RT_ENCODING_ISO_8859_13")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_ISO_8859_15,	_R("RT_ENCODING_ISO_8859_15")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_ISO_2022_JP,	_R("RT_ENCODING_ISO_2022_JP")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_CS_ISO_2022_JP,	_R("RT_ENCODING_CS_ISO_2022_JP")))	goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_ISO_2022_KR,	_R("RT_ENCODING_ISO_2022_KR")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_EUC_KR,		_R("RT_ENCODING_EUC_KR")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_HZ_GB2312,	_R("RT_ENCODING_HZ_GB2312")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_GB18030,		_R("RT_ENCODING_GB18030")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_UTF_7,		_R("RT_ENCODING_UTF_7")))		goto error;
	if (!zz_test_encoding_get_info_do(RT_ENCODING_UTF_8,		_R("RT_ENCODING_UTF_8")))		goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_manual_test_encoding()
{
	rt_s ret;

	if (!zz_test_encoding_get_info()) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

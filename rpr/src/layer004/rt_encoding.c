#include "layer004/rt_encoding.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer003/rt_char.h"
#include "layer003/rt_fast_initialization.h"

static const rt_un32 rt_encoding_code_pages[RT_ENCODING_ENCODINGS_COUNT] = {
	0,      /* System ANSI code page (CP_ACP).	*/
	37,     /* RT_ENCODING_IBM037.			*/
	437,    /* RT_ENCODING_IBM437.			*/
	500,    /* RT_ENCODING_IBM500.			*/
	708,    /* RT_ENCODING_ASMO_708.		*/
	737,    /* RT_ENCODING_IBM737.			*/
	775,    /* RT_ENCODING_IBM775.			*/
	850,    /* RT_ENCODING_IBM850.			*/
	852,    /* RT_ENCODING_IBM852.			*/
	855,    /* RT_ENCODING_IBM855.			*/
	857,    /* RT_ENCODING_IBM857.			*/
	860,    /* RT_ENCODING_IBM860.			*/
	861,    /* RT_ENCODING_IBM861.			*/
	863,    /* RT_ENCODING_IBM863.			*/
	864,    /* RT_ENCODING_IBM864.			*/
	865,    /* RT_ENCODING_IBM865.			*/
	866,    /* RT_ENCODING_CP866.			*/
	869,    /* RT_ENCODING_IBM869.			*/
	870,    /* RT_ENCODING_IBM870.			*/
	874,    /* RT_ENCODING_WINDOWS_874.		*/
	875,    /* RT_ENCODING_CP875.			*/
	932,    /* RT_ENCODING_SHIFT_JIS.		*/
	936,    /* RT_ENCODING_GB2312.			*/
	949,    /* RT_ENCODING_CP949.			*/
	950,    /* RT_ENCODING_BIG5.			*/
	1026,   /* RT_ENCODING_IBM1026.			*/
	1047,   /* RT_ENCODING_IBM1047.			*/
	1200,   /* RT_ENCODING_UTF_16.			*/
	1200,   /* RT_ENCODING_UTF_16LE.		*/
	1201,   /* RT_ENCODING_UTF_16BE.		*/
	1250,   /* RT_ENCODING_WINDOWS_1250.		*/
	1251,   /* RT_ENCODING_WINDOWS_1251.		*/
	1252,   /* RT_ENCODING_WINDOWS_1252.		*/
	1253,   /* RT_ENCODING_WINDOWS_1253.		*/
	1254,   /* RT_ENCODING_WINDOWS_1254.		*/
	1255,   /* RT_ENCODING_WINDOWS_1255.		*/
	1256,   /* RT_ENCODING_WINDOWS_1256.		*/
	1257,   /* RT_ENCODING_WINDOWS_1257.		*/
	1258,   /* RT_ENCODING_WINDOWS_1258.		*/
	1361,   /* RT_ENCODING_CP1361.			*/
	10000,  /* RT_ENCODING_MAC.			*/
	12000,  /* RT_ENCODING_UTF_32.			*/
	12000,  /* RT_ENCODING_UTF_32LE.		*/
	12001,  /* RT_ENCODING_UTF_32BE.		*/
	20127,  /* RT_ENCODING_US_ASCII.		*/
	20261,  /* RT_ENCODING_T_61.			*/
	20269,  /* RT_ENCODING_ISO_6937.		*/
	20273,  /* RT_ENCODING_IBM273.			*/
	20277,  /* RT_ENCODING_IBM277.			*/
	20278,  /* RT_ENCODING_IBM278.			*/
	20280,  /* RT_ENCODING_IBM280.			*/
	20284,  /* RT_ENCODING_IBM284.			*/
	20285,  /* RT_ENCODING_IBM285.			*/
	20290,  /* RT_ENCODING_IBM290.			*/
	20297,  /* RT_ENCODING_IBM297.			*/
	20420,  /* RT_ENCODING_IBM420.			*/
	20423,  /* RT_ENCODING_IBM423.			*/
	20424,  /* RT_ENCODING_IBM424.			*/
	20866,  /* RT_ENCODING_KOI8_R.			*/
	20871,  /* RT_ENCODING_IBM871.			*/
	20880,  /* RT_ENCODING_IBM880.			*/
	20905,  /* RT_ENCODING_IBM905.			*/
	21866,  /* RT_ENCODING_KOI8_U.			*/
	28591,  /* RT_ENCODING_ISO_8859_1.		*/
	28592,  /* RT_ENCODING_ISO_8859_2.		*/
	28593,  /* RT_ENCODING_ISO_8859_3.		*/
	28594,  /* RT_ENCODING_ISO_8859_4.		*/
	28595,  /* RT_ENCODING_ISO_8859_5.		*/
	28596,  /* RT_ENCODING_ISO_8859_6.		*/
	28597,  /* RT_ENCODING_ISO_8859_7.		*/
	28598,  /* RT_ENCODING_ISO_8859_8.		*/
	28599,  /* RT_ENCODING_ISO_8859_9.		*/
	28603,  /* RT_ENCODING_ISO_8859_13.		*/
	28605,  /* RT_ENCODING_ISO_8859_15.		*/
	50220,  /* RT_ENCODING_ISO_2022_JP.		*/
	50221,  /* RT_ENCODING_CS_ISO_2022_JP.		*/
	50225,  /* RT_ENCODING_ISO_2022_KR.		*/
	51932,  /* RT_ENCODING_EUC_JP.			*/
	51936,  /* RT_ENCODING_EUC_CN.			*/
	51949,  /* RT_ENCODING_EUC_KR.			*/
	52936,  /* RT_ENCODING_HZ_GB2312.		*/
	54936,  /* RT_ENCODING_GB18030.			*/
	65000,  /* RT_ENCODING_UTF_7.			*/
	65001   /* RT_ENCODING_UTF_8.			*/
};

static const rt_char *const rt_encoding_code_names[RT_ENCODING_ENCODINGS_COUNT] = {
	_R(""),			/* Keep original encoding.	*/
	_R("IBM037"),		/* RT_ENCODING_IBM037.		*/
	_R("IBM437"),		/* RT_ENCODING_IBM437.		*/
	_R("IBM500"),		/* RT_ENCODING_IBM500.		*/
	_R("ASMO-708"),		/* RT_ENCODING_ASMO_708.	*/
	_R("CP737"),		/* RT_ENCODING_IBM737.		*/
	_R("IBM775"),		/* RT_ENCODING_IBM775.		*/
	_R("IBM850"),		/* RT_ENCODING_IBM850.		*/
	_R("IBM852"),		/* RT_ENCODING_IBM852.		*/
	_R("IBM855"),		/* RT_ENCODING_IBM855.		*/
	_R("IBM857"),		/* RT_ENCODING_IBM857.		*/
	_R("IBM860"),		/* RT_ENCODING_IBM860.		*/
	_R("IBM861"),		/* RT_ENCODING_IBM861.		*/
	_R("IBM863"),		/* RT_ENCODING_IBM863.		*/
	_R("IBM864"),		/* RT_ENCODING_IBM864.		*/
	_R("IBM865"),		/* RT_ENCODING_IBM865.		*/
	_R("CP866"),		/* RT_ENCODING_CP866.		*/
	_R("IBM869"),		/* RT_ENCODING_IBM869.		*/
	_R("IBM870"),		/* RT_ENCODING_IBM870.		*/
	_R("WINDOWS-874"),	/* RT_ENCODING_WINDOWS_874.	*/
	_R("CP875"),		/* RT_ENCODING_CP875.		*/
	_R("SHIFT-JIS"),	/* RT_ENCODING_SHIFT_JIS.	*/
	_R("GB2312"),		/* RT_ENCODING_GB2312.		*/
	_R("MSCP949"),		/* RT_ENCODING_CP949.		*/
	_R("BIG5"),		/* RT_ENCODING_BIG5.		*/
	_R("IBM1026"),		/* RT_ENCODING_IBM1026.		*/
	_R("IBM1047"),		/* RT_ENCODING_IBM1047.		*/
	_R("UTF-16"),		/* RT_ENCODING_UTF_16.		*/
	_R("UTF-16LE"),		/* RT_ENCODING_UTF_16LE.	*/
	_R("UTF-16BE"),		/* RT_ENCODING_UTF_16BE.	*/
	_R("WINDOWS-1250"),	/* RT_ENCODING_WINDOWS_1250.	*/
	_R("WINDOWS-1251"),	/* RT_ENCODING_WINDOWS_1251.	*/
	_R("WINDOWS-1252"),	/* RT_ENCODING_WINDOWS_1252.	*/
	_R("WINDOWS-1253"),	/* RT_ENCODING_WINDOWS_1253.	*/
	_R("WINDOWS-1254"),	/* RT_ENCODING_WINDOWS_1254.	*/
	_R("WINDOWS-1255"),	/* RT_ENCODING_WINDOWS_1255.	*/
	_R("WINDOWS-1256"),	/* RT_ENCODING_WINDOWS_1256.	*/
	_R("WINDOWS-1257"),	/* RT_ENCODING_WINDOWS_1257.	*/
	_R("WINDOWS-1258"),	/* RT_ENCODING_WINDOWS_1258.	*/
	_R("MSCP1361"),		/* RT_ENCODING_CP1361.		*/
	_R("MAC"),		/* RT_ENCODING_MAC.		*/
	_R("UTF-32"),		/* RT_ENCODING_UTF_32.		*/
	_R("UTF-32LE"),		/* RT_ENCODING_UTF_32LE.	*/
	_R("UTF-32BE"),		/* RT_ENCODING_UTF_32BE.	*/
	_R("ANSI_X3.4-1968"),	/* RT_ENCODING_US_ASCII.	*/
	_R("T.61"),		/* RT_ENCODING_T_61.		*/
	_R("ISO6937"),		/* RT_ENCODING_ISO_6937.	*/
	_R("IBM273"),		/* RT_ENCODING_IBM273.		*/
	_R("IBM277"),		/* RT_ENCODING_IBM277.		*/
	_R("IBM278"),		/* RT_ENCODING_IBM278.		*/
	_R("IBM280"),		/* RT_ENCODING_IBM280.		*/
	_R("IBM284"),		/* RT_ENCODING_IBM284.		*/
	_R("IBM285"),		/* RT_ENCODING_IBM285.		*/
	_R("IBM290"),		/* RT_ENCODING_IBM290.		*/
	_R("IBM297"),		/* RT_ENCODING_IBM297.		*/
	_R("IBM420"),		/* RT_ENCODING_IBM420.		*/
	_R("IBM423"),		/* RT_ENCODING_IBM423.		*/
	_R("IBM424"),		/* RT_ENCODING_IBM424.		*/
	_R("KOI8-R"),		/* RT_ENCODING_KOI8_R.		*/
	_R("IBM871"),		/* RT_ENCODING_IBM871.		*/
	_R("IBM880"),		/* RT_ENCODING_IBM880.		*/
	_R("IBM905"),		/* RT_ENCODING_IBM905.		*/
	_R("KOI8-U"),		/* RT_ENCODING_KOI8_U.		*/
	_R("ISO-8859-1"),	/* RT_ENCODING_ISO_8859_1.	*/
	_R("ISO-8859-2"),	/* RT_ENCODING_ISO_8859_2.	*/
	_R("ISO-8859-3"),	/* RT_ENCODING_ISO_8859_3.	*/
	_R("ISO-8859-4"),	/* RT_ENCODING_ISO_8859_4.	*/
	_R("ISO-8859-5"),	/* RT_ENCODING_ISO_8859_5.	*/
	_R("ISO-8859-6"),	/* RT_ENCODING_ISO_8859_6.	*/
	_R("ISO-8859-7"),	/* RT_ENCODING_ISO_8859_7.	*/
	_R("ISO-8859-8"),	/* RT_ENCODING_ISO_8859_8.	*/
	_R("ISO-8859-9"),	/* RT_ENCODING_ISO_8859_9.	*/
	_R("ISO-8859-13"),	/* RT_ENCODING_ISO_8859_13.	*/
	_R("ISO-8859-15"),	/* RT_ENCODING_ISO_8859_15.	*/
	_R("ISO-2022-JP"),	/* RT_ENCODING_ISO_2022_JP.	*/
	_R("CSISO2022JP"),	/* RT_ENCODING_CS_ISO_2022_JP.	*/
	_R("ISO-2022-KR"),	/* RT_ENCODING_ISO_2022_KR.	*/
	_R("EUC-JP"),		/* RT_ENCODING_EUC_JP.		*/
	_R("EUC-CN"),		/* RT_ENCODING_EUC_CN.		*/
	_R("EUC-KR"),		/* RT_ENCODING_EUC_KR.		*/
	_R("GB2312"),		/* RT_ENCODING_HZ_GB2312.	*/
	_R("GB18030"),		/* RT_ENCODING_GB18030.		*/
	_R("UTF-7"),		/* RT_ENCODING_UTF_7.		*/
	_R("UTF-8")		/* RT_ENCODING_UTF_8.		*/
};

static const rt_un rt_encoding_max_char_sizes[RT_ENCODING_ENCODINGS_COUNT] = {
	0, /* RT_ENCODING_SYSTEM_DEFAULT.	*/
	1, /* RT_ENCODING_IBM037.		*/
	1, /* RT_ENCODING_IBM437.		*/
	1, /* RT_ENCODING_IBM500.		*/
	1, /* RT_ENCODING_ASMO_708.		*/
	1, /* RT_ENCODING_IBM737.		*/
	1, /* RT_ENCODING_IBM775.		*/
	1, /* RT_ENCODING_IBM850.		*/
	1, /* RT_ENCODING_IBM852.		*/
	1, /* RT_ENCODING_IBM855.		*/
	1, /* RT_ENCODING_IBM857.		*/
	1, /* RT_ENCODING_IBM860.		*/
	1, /* RT_ENCODING_IBM861.		*/
	1, /* RT_ENCODING_IBM863.		*/
	1, /* RT_ENCODING_IBM864.		*/
	1, /* RT_ENCODING_IBM865.		*/
	1, /* RT_ENCODING_CP866.		*/
	1, /* RT_ENCODING_IBM869.		*/
	1, /* RT_ENCODING_IBM870.		*/
	1, /* RT_ENCODING_WINDOWS_874.		*/
	1, /* RT_ENCODING_CP875.		*/
	2, /* RT_ENCODING_SHIFT_JIS.		*/
	2, /* RT_ENCODING_GB2312.		*/
	2, /* RT_ENCODING_CP949.		*/
	2, /* RT_ENCODING_BIG5.			*/
	1, /* RT_ENCODING_IBM1026.		*/
	1, /* RT_ENCODING_IBM1047.		*/
	4, /* RT_ENCODING_UTF_16.		*/
	4, /* RT_ENCODING_UTF_16LE.		*/
	4, /* RT_ENCODING_UTF_16BE.		*/
	1, /* RT_ENCODING_WINDOWS_1250.		*/
	1, /* RT_ENCODING_WINDOWS_1251.		*/
	1, /* RT_ENCODING_WINDOWS_1252.		*/
	1, /* RT_ENCODING_WINDOWS_1253.		*/
	1, /* RT_ENCODING_WINDOWS_1254.		*/
	1, /* RT_ENCODING_WINDOWS_1255.		*/
	1, /* RT_ENCODING_WINDOWS_1256.		*/
	1, /* RT_ENCODING_WINDOWS_1257.		*/
	1, /* RT_ENCODING_WINDOWS_1258.		*/
	2, /* RT_ENCODING_CP1361.		*/
	1, /* RT_ENCODING_MAC.			*/
	4, /* RT_ENCODING_UTF_32.		*/
	4, /* RT_ENCODING_UTF_32LE.		*/
	4, /* RT_ENCODING_UTF_32BE.		*/
	1, /* RT_ENCODING_US_ASCII.		*/
	2, /* RT_ENCODING_T_61.			*/
	1, /* RT_ENCODING_ISO_6937.		*/
	1, /* RT_ENCODING_IBM273.		*/
	1, /* RT_ENCODING_IBM277.		*/
	1, /* RT_ENCODING_IBM278.		*/
	1, /* RT_ENCODING_IBM280.		*/
	1, /* RT_ENCODING_IBM284.		*/
	1, /* RT_ENCODING_IBM285.		*/
	1, /* RT_ENCODING_IBM290.		*/
	1, /* RT_ENCODING_IBM297.		*/
	1, /* RT_ENCODING_IBM420.		*/
	1, /* RT_ENCODING_IBM423.		*/
	1, /* RT_ENCODING_IBM424.		*/
	1, /* RT_ENCODING_KOI8_R.		*/
	1, /* RT_ENCODING_IBM871.		*/
	1, /* RT_ENCODING_IBM880.		*/
	1, /* RT_ENCODING_IBM905.		*/
	1, /* RT_ENCODING_KOI8_U.		*/
	1, /* RT_ENCODING_ISO_8859_1.		*/
	1, /* RT_ENCODING_ISO_8859_2.		*/
	1, /* RT_ENCODING_ISO_8859_3.		*/
	1, /* RT_ENCODING_ISO_8859_4.		*/
	1, /* RT_ENCODING_ISO_8859_5.		*/
	1, /* RT_ENCODING_ISO_8859_6.		*/
	1, /* RT_ENCODING_ISO_8859_7.		*/
	1, /* RT_ENCODING_ISO_8859_8.		*/
	1, /* RT_ENCODING_ISO_8859_9.		*/
	1, /* RT_ENCODING_ISO_8859_13.		*/
	1, /* RT_ENCODING_ISO_8859_15.		*/
	5, /* RT_ENCODING_ISO_2022_JP.		*/
	5, /* RT_ENCODING_CS_ISO_2022_JP.	*/
	5, /* RT_ENCODING_ISO_2022_KR.		*/
	3, /* RT_ENCODING_EUC_JP.		*/
	2, /* RT_ENCODING_EUC_CN.		*/
	2, /* RT_ENCODING_EUC_KR.		*/
	5, /* RT_ENCODING_HZ_GB2312.		*/
	4, /* RT_ENCODING_GB18030.		*/
	5, /* RT_ENCODING_UTF_7.		*/
	4  /* RT_ENCODING_UTF_8.		*/
};

#ifdef RT_DEFINE_LINUX

static struct rt_fast_initialization rt_encoding_system_initialization = RT_FAST_INITIALIZATION_STATIC_INIT;

static rt_char rt_encoding_system_buffer[64];

static rt_char *rt_encoding_system;

#endif

#ifdef RT_DEFINE_LINUX
/**
 * Retrieve the Linux system encoding.<br>
 * Call setlocale(LC_ALL, "") to avoid "C" locale.
 *
 * Return RT_NULL in case of error.
 */
static rt_s rt_encoding_get_linux_system(rt_char **encoding)
{
	rt_char *encoding_name;
	rt_un encoding_name_size;

	if (rt_fast_initialization_is_required(&rt_encoding_system_initialization)) {
		rt_encoding_system = RT_NULL;

		encoding_name = nl_langinfo(CODESET);
		/* There are very few chances that nl_langinfo failed. */
		if (encoding_name) {
			encoding_name_size = rt_char_get_size(encoding_name);
			/* There are very few chances that encoding_name is empty. */
			if (encoding_name_size) {
				/* The pointer nl_langinfo is not thread safe so we make a copy quickly. */
				if (rt_char_copy(encoding_name, encoding_name_size, rt_encoding_system_buffer, 64))
					rt_encoding_system = rt_encoding_system_buffer;
			}
		}

		rt_fast_initialization_notify_done(&rt_encoding_system_initialization);
	}
	if (!rt_encoding_system) {
		/* The initialization has failed now or in the past. */
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
	}
	*encoding = rt_encoding_system;
	return (rt_encoding_system != RT_NULL);
}

#endif

rt_s rt_encoding_get_system(enum rt_encoding *encoding)
{
#ifdef RT_DEFINE_WINDOWS
	UINT code_page;
#else
	rt_char* linux_system_encoding;
	rt_un linux_system_encoding_size;
	const rt_char* current_encoding;
	rt_s ret;
#endif
	rt_un result = RT_TYPE_MAX_UN;
	rt_un i;

#ifdef RT_DEFINE_WINDOWS
	code_page = GetACP();
	for (i = 1; i < RT_ENCODING_ENCODINGS_COUNT; i++) {
		if (rt_encoding_code_pages[i] == code_page) {
			result = i;
			break;
		}
	}
	*encoding = result;
	return (result != RT_TYPE_MAX_UN);
#else
	if (!rt_encoding_get_linux_system(&linux_system_encoding))
		goto error;
	linux_system_encoding_size = rt_char_get_size(linux_system_encoding);

	for (i = 1; i < RT_ENCODING_ENCODINGS_COUNT; i++) {
		current_encoding = rt_encoding_code_names[i];
		if (rt_char_equals(linux_system_encoding, linux_system_encoding_size, current_encoding, rt_char_get_size(current_encoding))) {
			result = i;
			break;
		}
	}

	*encoding = result;
	ret = (result != RT_TYPE_MAX_UN);
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
#endif
}

rt_s rt_encoding_get_info(enum rt_encoding encoding, struct rt_encoding_info *encoding_info)
{
#ifdef RT_DEFINE_WINDOWS
	CPINFOEX cp_info_ex;
#endif
	enum rt_encoding actual_encoding;
	rt_s ret;

	if (encoding == RT_ENCODING_SYSTEM_DEFAULT) {
		if (!rt_encoding_get_system(&actual_encoding))
			goto error;
	} else if (encoding < RT_ENCODING_ENCODINGS_COUNT) {
		actual_encoding = encoding;
	} else {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	encoding_info->code_page = rt_encoding_code_pages[actual_encoding];
	encoding_info->name = rt_encoding_code_names[actual_encoding];
	encoding_info->max_char_size = rt_encoding_max_char_sizes[actual_encoding];

#ifdef RT_DEFINE_WINDOWS
	switch (actual_encoding) {
	case RT_ENCODING_UTF_16:
		if (!rt_char_copy(_R("1200 (UTF-16)"), 13, encoding_info->label, RT_ENCODING_LABEL_SIZE))
			goto error;
		break;
	case RT_ENCODING_UTF_16LE:
		if (!rt_char_copy(_R("1200 (UTF-16 little-endian)"), 27, encoding_info->label, RT_ENCODING_LABEL_SIZE))
			goto error;
		break;
	case RT_ENCODING_UTF_16BE:
		if (!rt_char_copy(_R("1201 (UTF-16 big-endian)"), 24, encoding_info->label, RT_ENCODING_LABEL_SIZE))
			goto error;
		break;
	case RT_ENCODING_UTF_32:
		if (!rt_char_copy(_R("12000 (UTF-32)"), 14, encoding_info->label, RT_ENCODING_LABEL_SIZE))
			goto error;
		break;
	case RT_ENCODING_UTF_32LE:
		if (!rt_char_copy(_R("12000 (UTF-32 little-endian)"), 28, encoding_info->label, RT_ENCODING_LABEL_SIZE))
			goto error;
		break;
	case RT_ENCODING_UTF_32BE:
		if (!rt_char_copy(_R("12001 (UTF-32 big-endian)"), 25, encoding_info->label, RT_ENCODING_LABEL_SIZE))
			goto error;
		break;
	case RT_ENCODING_EUC_JP:
		if (!rt_char_copy(_R("51932 (EUC-japanese)"), 20, encoding_info->label, RT_ENCODING_LABEL_SIZE))
			goto error;
		break;
	case RT_ENCODING_EUC_CN:
		if (!rt_char_copy(_R("51936 (EUC-chinese)"), 19, encoding_info->label, RT_ENCODING_LABEL_SIZE))
			goto error;
		break;
	default:
		/* Returns 0 and set last error in case of issue. */
		if (!GetCPInfoEx(encoding_info->code_page, 0, &cp_info_ex))
			goto error;

		/* Copy the label. */
		if (!rt_char_copy(cp_info_ex.CodePageName, rt_char_get_size(cp_info_ex.CodePageName), encoding_info->label, RT_ENCODING_LABEL_SIZE))
			goto error;
	}

#else
	/* Use the name as label. */
	if (!rt_char_copy(encoding_info->name, rt_char_get_size(encoding_info->name), encoding_info->label, RT_ENCODING_LABEL_SIZE))
		goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

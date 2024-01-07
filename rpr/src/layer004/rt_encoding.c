#include "layer004/rt_encoding.h"

#include "layer001/rt_memory.h"
#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer003/rt_char.h"
#include "layer003/rt_fast_initialization.h"

/**
 * @file
 *
 * <p>
 * It would be pretty hard to implement a input/output encoding streams.<br>
 * For example an issue is that multiple UTF-8 characters (actually code points) can be equivalent to one character in the other encoding.<br>
 * For example, "é" can be encoded as 0xC3, 0xA9, one code point and two bytes.<br>
 * But it can also be encoded as e + acute accent, 0x65, 0xCC, 0x81, two code points, the first of one byte, the second of two bytes.<br>
 * This can cause issue while encoding from UTF-16 or UTF-8 to ISO-8859-1 because the 'e' can be sent to the output stream alone and the accent later on.<br>
 * But it may not that much of an issue because it is already how WideCharToMultiByte and MultiByteToWideChar work today.<br>
 * They do not correctly handle this case of further accent. And iconv is even worst: it fails in case of decomposed characters.
 * </p>
 *
 * <p>
 * Unlike iconv, WideCharToMultiByte/MultiByteToWideChar are not detecting if the input ends with an incomplete character.<br>
 * As a result, the input must be cut before the incomplete character.<br>
 * IsDBCSLeadByteEx is a function that would help to do so, but it supports only some DBCS (Double Byte Char String) encodings, so not UTF-8 and some others.
 * IsDBCSLeadByteEx returns TRUE if a second byte is expected after the given one.<br>
 * IsDBCSLeadByteEx should work on encodings for which LeadByte field of CPINFOEX structure returned by GetCPInfoEx is set:<br>
 * RT_ENCODING_SHIFT_JIS<br>
 * RT_ENCODING_GB2312<br>
 * RT_ENCODING_CP949<br>
 * RT_ENCODING_BIG5<br>
 * RT_ENCODING_CP1361<br>
 * RT_ENCODING_T_61<br>
 * RT_ENCODING_EUC_JP<br>
 * RT_ENCODING_EUC_KR<br>
 * Notice that as explained in the documentation, all these encodings have a maximum of two bytes per character.
 * </p>
 *
 * <p>
 * Following encodings are not supported by IsDBCSLeadByteEx:<br>
 * RT_ENCODING_ISO_2022_JP<br>
 * RT_ENCODING_CS_ISO_2022_JP<br>
 * RT_ENCODING_ISO_2022_KR<br>
 * RT_ENCODING_HZ_GB2312<br>
 * RT_ENCODING_GB18030<br>
 * RT_ENCODING_UTF_7<br>
 * IsDBCSLeadByteEx should returns FALSE regardless of the given character.<br>
 * A custom implementation would have to be made for UTF-8/16/32, also taking into account the BOM.
 * </p>
 */

static const rt_un16 rt_encoding_code_pages[RT_ENCODING_ENCODINGS_COUNT] = {
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
	20297,  /* RT_ENCODING_IBM297.			*/
	20420,  /* RT_ENCODING_IBM420.			*/
	20423,  /* RT_ENCODING_IBM423.			*/
	20424,  /* RT_ENCODING_IBM424.			*/
	20866,  /* RT_ENCODING_KOI8_R.			*/
	20871,  /* RT_ENCODING_IBM871.			*/
	20880,  /* RT_ENCODING_IBM880.			*/
	20905,  /* RT_ENCODING_IBM905.			*/
	20932,  /* RT_ENCODING_EUC_JP.			*/
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
	_R("IBM297"),		/* RT_ENCODING_IBM297.		*/
	_R("IBM420"),		/* RT_ENCODING_IBM420.		*/
	_R("IBM423"),		/* RT_ENCODING_IBM423.		*/
	_R("IBM424"),		/* RT_ENCODING_IBM424.		*/
	_R("KOI8-R"),		/* RT_ENCODING_KOI8_R.		*/
	_R("IBM871"),		/* RT_ENCODING_IBM871.		*/
	_R("IBM880"),		/* RT_ENCODING_IBM880.		*/
	_R("IBM905"),		/* RT_ENCODING_IBM905.		*/
	_R("EUC-JP"),		/* RT_ENCODING_EUC_JP.		*/
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
	_R("EUC-KR"),		/* RT_ENCODING_EUC_KR.		*/
	_R("GB2312"),		/* RT_ENCODING_HZ_GB2312.	*/
	_R("GB18030"),		/* RT_ENCODING_GB18030.		*/
	_R("UTF-7"),		/* RT_ENCODING_UTF_7.		*/
	_R("UTF-8")		/* RT_ENCODING_UTF_8.		*/
};

static const rt_uchar8 rt_encoding_code_unit_sizes[RT_ENCODING_ENCODINGS_COUNT] = {
	1, /* RT_ENCODING_SYSTEM_DEFAULT.	*/
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
	1, /* RT_ENCODING_SHIFT_JIS.		*/
	1, /* RT_ENCODING_GB2312.		*/
	1, /* RT_ENCODING_CP949.		*/
	1, /* RT_ENCODING_BIG5.			*/
	1, /* RT_ENCODING_IBM1026.		*/
	1, /* RT_ENCODING_IBM1047.		*/
	2, /* RT_ENCODING_UTF_16.		*/
	2, /* RT_ENCODING_UTF_16LE.		*/
	2, /* RT_ENCODING_UTF_16BE.		*/
	1, /* RT_ENCODING_WINDOWS_1250.		*/
	1, /* RT_ENCODING_WINDOWS_1251.		*/
	1, /* RT_ENCODING_WINDOWS_1252.		*/
	1, /* RT_ENCODING_WINDOWS_1253.		*/
	1, /* RT_ENCODING_WINDOWS_1254.		*/
	1, /* RT_ENCODING_WINDOWS_1255.		*/
	1, /* RT_ENCODING_WINDOWS_1256.		*/
	1, /* RT_ENCODING_WINDOWS_1257.		*/
	1, /* RT_ENCODING_WINDOWS_1258.		*/
	1, /* RT_ENCODING_CP1361.		*/
	1, /* RT_ENCODING_MAC.			*/
	4, /* RT_ENCODING_UTF_32.		*/
	4, /* RT_ENCODING_UTF_32LE.		*/
	4, /* RT_ENCODING_UTF_32BE.		*/
	1, /* RT_ENCODING_US_ASCII.		*/
	1, /* RT_ENCODING_T_61.			*/
	1, /* RT_ENCODING_ISO_6937.		*/
	1, /* RT_ENCODING_IBM273.		*/
	1, /* RT_ENCODING_IBM277.		*/
	1, /* RT_ENCODING_IBM278.		*/
	1, /* RT_ENCODING_IBM280.		*/
	1, /* RT_ENCODING_IBM284.		*/
	1, /* RT_ENCODING_IBM285.		*/
	1, /* RT_ENCODING_IBM297.		*/
	1, /* RT_ENCODING_IBM420.		*/
	1, /* RT_ENCODING_IBM423.		*/
	1, /* RT_ENCODING_IBM424.		*/
	1, /* RT_ENCODING_KOI8_R.		*/
	1, /* RT_ENCODING_IBM871.		*/
	1, /* RT_ENCODING_IBM880.		*/
	1, /* RT_ENCODING_IBM905.		*/
	1, /* RT_ENCODING_EUC_JP.		*/
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
	1, /* RT_ENCODING_ISO_2022_JP.		*/
	1, /* RT_ENCODING_CS_ISO_2022_JP.	*/
	1, /* RT_ENCODING_ISO_2022_KR.		*/
	1, /* RT_ENCODING_EUC_KR.		*/
	1, /* RT_ENCODING_HZ_GB2312.		*/
	1, /* RT_ENCODING_GB18030.		*/
	1, /* RT_ENCODING_UTF_7.		*/
	1  /* RT_ENCODING_UTF_8.		*/
};

static const rt_uchar8 rt_encoding_max_code_point_sizes[RT_ENCODING_ENCODINGS_COUNT] = {
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
	1, /* RT_ENCODING_IBM297.		*/
	1, /* RT_ENCODING_IBM420.		*/
	1, /* RT_ENCODING_IBM423.		*/
	1, /* RT_ENCODING_IBM424.		*/
	1, /* RT_ENCODING_KOI8_R.		*/
	1, /* RT_ENCODING_IBM871.		*/
	1, /* RT_ENCODING_IBM880.		*/
	1, /* RT_ENCODING_IBM905.		*/
	2, /* RT_ENCODING_EUC_JP.		*/
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
	2, /* RT_ENCODING_EUC_KR.		*/
	5, /* RT_ENCODING_HZ_GB2312.		*/
	4, /* RT_ENCODING_GB18030.		*/
	5, /* RT_ENCODING_UTF_7.		*/
	4  /* RT_ENCODING_UTF_8.		*/
};

static struct rt_fast_initialization rt_encoding_system_initialization = RT_FAST_INITIALIZATION_STATIC_INIT;

static enum rt_encoding rt_encoding_system = 0;

#ifdef RT_DEFINE_LINUX

/**
 * Retrieve the Linux system encoding.<br>
 * Make sure to call setlocale(LC_ALL, "") before this function to avoid "C" locale.
 */
static rt_s rt_encoding_get_linux_system(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_char *encoding_name;
	rt_un encoding_name_size;
	rt_s ret;

	encoding_name = nl_langinfo(CODESET);
	/* There are very few chances that nl_langinfo failed. */
	/* It may not set errno. */
	if (RT_LIKELY(encoding_name)) {
		encoding_name_size = rt_char_get_size(encoding_name);
		/* There are very few chances that encoding_name is empty. */
		if (RT_LIKELY(encoding_name_size)) {
			/* The pointer nl_langinfo is not thread safe so we make a copy really quick. */
			if (RT_UNLIKELY(!rt_char_copy(encoding_name, encoding_name_size, buffer, buffer_capacity)))
				goto error;
			*buffer_size = encoding_name_size;
		} else {
			rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
			goto error;
		}
	} else {
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto error;
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

#endif

rt_s rt_encoding_get_system(enum rt_encoding *encoding)
{
#ifdef RT_DEFINE_WINDOWS
	UINT code_page;
#else
	rt_char system_encoding_name[64];
	rt_un system_encoding_name_size;
	const rt_char* current_encoding;
#endif
	rt_un i;
	rt_s ret;

	if (rt_fast_initialization_is_required(&rt_encoding_system_initialization)) {

#ifdef RT_DEFINE_WINDOWS
		code_page = GetACP();
		for (i = 1; i < RT_ENCODING_ENCODINGS_COUNT; i++) {
			if (rt_encoding_code_pages[i] == code_page) {
				rt_encoding_system = i;
				break;
			}
		}
#else
		/* Retrieve the encoding name as a string. */
		if (RT_UNLIKELY(!rt_encoding_get_linux_system(system_encoding_name, 64, &system_encoding_name_size))) {
			rt_fast_initialization_notify_done(&rt_encoding_system_initialization);
			goto error;
		}

		for (i = 1; i < RT_ENCODING_ENCODINGS_COUNT; i++) {
			current_encoding = rt_encoding_code_names[i];
			if (rt_char_equals(system_encoding_name, system_encoding_name_size, current_encoding, rt_char_get_size(current_encoding))) {
				rt_encoding_system = i;
				break;
			}
		}
#endif

		rt_fast_initialization_notify_done(&rt_encoding_system_initialization);
	}

	*encoding = rt_encoding_system;

	/* At this point rt_encoding_system is either zero (its initial value) or the correct encoding. */
	if (RT_UNLIKELY(!rt_encoding_system)) {
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

/**
 * Returns actual encoding if <tt>encoding</tt> is <tt>RT_ENCODING_SYSTEM_DEFAULT</tt>.
 */
static rt_s rt_encoding_get_actual(enum rt_encoding encoding, enum rt_encoding *actual_encoding)
{
	rt_s ret;

	if (encoding == RT_ENCODING_SYSTEM_DEFAULT) {
		if (RT_UNLIKELY(!rt_encoding_get_system(actual_encoding)))
			goto error;
	} else if (encoding < RT_ENCODING_ENCODINGS_COUNT) {
		*actual_encoding = encoding;
	} else {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_encoding_get_info(enum rt_encoding encoding, struct rt_encoding_info *encoding_info)
{
#ifdef RT_DEFINE_WINDOWS
	CPINFOEX cp_info_ex;
#endif
	enum rt_encoding actual_encoding;
	rt_s ret;

	if (RT_UNLIKELY(!rt_encoding_get_actual(encoding, &actual_encoding)))
		goto error;

	encoding_info->code_page = rt_encoding_code_pages[actual_encoding];
	encoding_info->name = rt_encoding_code_names[actual_encoding];
	encoding_info->code_unit_size = rt_encoding_code_unit_sizes[actual_encoding];
	encoding_info->max_code_point_size = rt_encoding_max_code_point_sizes[actual_encoding];

#ifdef RT_DEFINE_WINDOWS
	switch (actual_encoding) {
	case RT_ENCODING_UTF_16:
		if (RT_UNLIKELY(!rt_char_copy(_R("1200 (UTF-16)"), 13, encoding_info->label, RT_ENCODING_LABEL_SIZE)))
			goto error;
		break;
	case RT_ENCODING_UTF_16LE:
		if (RT_UNLIKELY(!rt_char_copy(_R("1200 (UTF-16 little-endian)"), 27, encoding_info->label, RT_ENCODING_LABEL_SIZE)))
			goto error;
		break;
	case RT_ENCODING_UTF_16BE:
		if (RT_UNLIKELY(!rt_char_copy(_R("1201 (UTF-16 big-endian)"), 24, encoding_info->label, RT_ENCODING_LABEL_SIZE)))
			goto error;
		break;
	case RT_ENCODING_UTF_32:
		if (RT_UNLIKELY(!rt_char_copy(_R("12000 (UTF-32)"), 14, encoding_info->label, RT_ENCODING_LABEL_SIZE)))
			goto error;
		break;
	case RT_ENCODING_UTF_32LE:
		if (RT_UNLIKELY(!rt_char_copy(_R("12000 (UTF-32 little-endian)"), 28, encoding_info->label, RT_ENCODING_LABEL_SIZE)))
			goto error;
		break;
	case RT_ENCODING_UTF_32BE:
		if (RT_UNLIKELY(!rt_char_copy(_R("12001 (UTF-32 big-endian)"), 25, encoding_info->label, RT_ENCODING_LABEL_SIZE)))
			goto error;
		break;
	default:
		/* Returns 0 and set last error in case of issue. */
		if (RT_UNLIKELY(!GetCPInfoEx(encoding_info->code_page, 0, &cp_info_ex)))
			goto error;

		/* Copy the label. */
		if (RT_UNLIKELY(!rt_char_copy(cp_info_ex.CodePageName, rt_char_get_size(cp_info_ex.CodePageName), encoding_info->label, RT_ENCODING_LABEL_SIZE)))
			goto error;
	}

#else
	/* Use the name as label. */
	if (RT_UNLIKELY(!rt_char_copy(encoding_info->name, rt_char_get_size(encoding_info->name), encoding_info->label, RT_ENCODING_LABEL_SIZE)))
		goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_un rt_encoding_get_size(const rt_char8 *data, rt_un code_unit_size)
{
	const rt_char8 *chars;
	const rt_un16 *wide_chars;
	const rt_un32 *very_wide_chars;
	rt_un result;

	if (code_unit_size == 2) {
		wide_chars = (rt_un16*)data;
		while (*wide_chars++);
		result = (rt_un)(wide_chars - (rt_un16*)data - 1);
	} else if (code_unit_size == 4) {
		very_wide_chars = (rt_un32*)data;
		while (*very_wide_chars++);
		result = (rt_un)(very_wide_chars - (rt_un32*)data - 1);
	} else {
		chars = data;
		while (*chars++);
		result = (rt_un)(chars - data - 1);
	}
	return result;
}

#ifdef RT_DEFINE_WINDOWS

/**
 * Simple affectation, "1 2" -> "0 0 1 2".
 */
static void rt_encoding_copy_16_le_to_32_le(const rt_un16 *source, rt_un32 *destination, rt_un size)
{
	rt_un i;

	for (i = 0; i < size; i++)
		destination[i] = source[i];
}

/**
 * Affectation then 32 bits swap, "1 2" -> "2 1 0 0".
 */
static void rt_encoding_copy_16_le_to_32_be(const rt_un16 *source, rt_un32 *destination, rt_un size)
{
	rt_un i;

	for (i = 0; i < size; i++)
		destination[i] = RT_MEMORY_SWAP_BYTES32((rt_un32)source[i]);
}

/**
 * Swap then affectation, "2 1" -> "0 0 1 2".
 */
static void rt_encoding_copy_16_be_to_32_le(const rt_un16 *source, rt_un32 *destination, rt_un size)
{
	rt_un i;

	for (i = 0; i < size; i++)
		destination[i] = RT_MEMORY_SWAP_BYTES16(source[i]);
}

/**
 * Affect to high bytes, "2 1" -> "2 1 0 0".
 */
static void rt_encoding_copy_16_be_to_32_be(const rt_un16 *source, rt_un32 *destination, rt_un size)
{
	rt_un i;

	for (i = 0; i < size; i++)
		((rt_un16*)(&destination[i]))[1] = source[i];
}

/**
 * Simple affectation, "0 0 1 2" -> "1 2".
 */
static void rt_encoding_copy_32_le_to_16_le(const rt_un32 *source, rt_un16 *destination, rt_un size)
{
	rt_un i;

	for (i = 0; i < size; i++)
		destination[i] = (rt_un16)source[i];
}

/**
 * Affectation then swap, "0 0 1 2" -> "2 1".
 */
static void rt_encoding_copy_32_le_to_16_be(const rt_un32 *source, rt_un16 *destination, rt_un size)
{
	rt_un i;

	for (i = 0; i < size; i++)
		destination[i] = RT_MEMORY_SWAP_BYTES16((rt_un16)source[i]);
}

/**
 * Swap then affectation, "2 1 0 0" -> "1 2".
 */
static void rt_encoding_copy_32_be_to_16_le(const rt_un32 *source, rt_un16 *destination, rt_un size)
{
	rt_un i;

	for (i = 0; i < size; i++)
		destination[i] = (rt_un16)RT_MEMORY_SWAP_BYTES32(source[i]);
}

/**
 * Affect from high bytes, "2 1 0 0" -> "2 1".
 */
static void rt_encoding_copy_32_be_to_16_be(const rt_un32 *source, rt_un16 *destination, rt_un size)
{
	rt_un i;

	for (i = 0; i < size; i++)
		destination[i] = ((rt_un16*)&source[i])[1];
}

static rt_s rt_encoding_encode_using_windows(const rt_char *input, rt_un input_size, enum rt_encoding output_encoding, rt_char8 *buffer, rt_un buffer_capacity, void **heap_buffer, rt_un *heap_buffer_capacity, rt_char8 **output, rt_un *output_size, struct rt_heap *heap)
{
	UINT code_page;
	rt_b use_heap = RT_FALSE;
	int actual_buffer_capacity;
	rt_un heap_buffer_size;
	int length;
	rt_s ret;

	/* If the default encoding is used, then the code page is CP_ACP (0) which is supported by WideCharToMultiByte. */
	code_page = rt_encoding_code_pages[output_encoding];

	if (buffer) {

		/* WideCharToMultiByte will not write the terminating zero, so we must save space for it. */
		actual_buffer_capacity = (int)buffer_capacity - 1;

		length = WideCharToMultiByte(code_page, 0, input, (int)input_size, buffer, actual_buffer_capacity, RT_NULL, RT_NULL);
		/* In case of issue, WideCharToMultiByte returns zero and set last error. */
		if (length) {
			*output_size = length;
			/* WideCharToMultiByte does not write a zero terminating character. */
			/* WideCharToMultiByte only supports encodings with 1 zero terminating character (not UTF-16 and not UTF-32). */
			buffer[length] = 0;

			/* Provided buffer was good enough. */
			*output = buffer;
		} else {
			if (RT_LIKELY(GetLastError() == ERROR_INSUFFICIENT_BUFFER)) {
				/* The provided buffer was not enough, use the heap. */
				use_heap = RT_TRUE;
			} else {
				goto error;
			}
		}
	} else {
		/* No buffer has been provided, use the heap. */
		use_heap = RT_TRUE;
	}

	if (use_heap) {

		/* The provided buffer was not enough, use the heap if available. */
		if (RT_LIKELY(heap)) {
			/* Compute the required buffer size. */
			length = WideCharToMultiByte(code_page, 0, input, (int)input_size, RT_NULL, 0, RT_NULL, RT_NULL);
			if (RT_UNLIKELY(!length)) {
				goto error;
			} else {

				/* We need space for the terminating zero. */
				heap_buffer_size = length + 1;

				if (RT_LIKELY(rt_heap_alloc_if_needed(RT_NULL, 0, heap_buffer, heap_buffer_capacity, (void**)output, heap_buffer_size, heap))) {

					length = WideCharToMultiByte(code_page, 0, input, (int)input_size, *output, length, RT_NULL, RT_NULL);
					if (RT_UNLIKELY(!length))
						goto error;
					/* Output size is the size of the buffer minus the terminating zero. */
					*output_size = heap_buffer_size - 1;

					/* WideCharToMultiByte does not write a zero terminating character. */
					/* WideCharToMultiByte only supports encodings with 1 zero terminating character (not UTF-16 and not UTF-32). */
					(*output)[length] = 0;

				} else {
					/* Allocation failed. */
					goto error;
				}
			}
		} else {
			/* Buffer was too small and no heap has been provided. */
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s rt_encoding_decode_using_windows(const rt_char8 *input, rt_un input_size, enum rt_encoding input_encoding, rt_char *buffer, rt_un buffer_capacity, void **heap_buffer, rt_un *heap_buffer_capacity, rt_char **output, rt_un *output_size, struct rt_heap *heap)
{
	UINT code_page;
	rt_b use_heap = RT_FALSE;
	int actual_buffer_capacity;
	rt_un heap_buffer_size;
	int length;
	rt_s ret;

	/* If the default encoding is used, then the code page is CP_ACP (0) which is supported by MultiByteToWideChar. */
	code_page = rt_encoding_code_pages[input_encoding];

	if (buffer) {

		/* MultiByteToWideChar will not write the terminating zero, so we must save space for it. */
		actual_buffer_capacity = (int)buffer_capacity - 1;

		length = MultiByteToWideChar(code_page, 0, input, (int)input_size, buffer, actual_buffer_capacity);
		/* In case of issue, MultiByteToWideChar returns zero and set last error. */
		if (length) {

			*output_size = length;
			/* MultiByteToWideChar does not write a zero terminating character. */
			buffer[length] = 0;

			/* Provided buffer was good enough. */
			*output = buffer;
		} else {
			if (RT_LIKELY(GetLastError() == ERROR_INSUFFICIENT_BUFFER)) {
				/* The provided buffer was not enough, use the heap. */
				use_heap = RT_TRUE;
			} else {
				goto error;
			}
		}
	} else {
		/* No buffer has been provided, use the heap. */
		use_heap = RT_TRUE;
	}

	if (use_heap) {

		/* The provided buffer was not enough, use the heap if available. */
		if (RT_LIKELY(heap)) {
			/* Compute the required buffer size. */
			length = MultiByteToWideChar(code_page, 0, input, (int)input_size, RT_NULL, 0);
			if (RT_UNLIKELY(!length)) {
				goto error;
			} else {

				/* We need space for the terminating zero. */
				heap_buffer_size = length + 1;

				if (RT_LIKELY(rt_heap_alloc_if_needed(RT_NULL, 0, heap_buffer, heap_buffer_capacity, (void**)output, heap_buffer_size * sizeof(rt_char), heap))) {

					length = MultiByteToWideChar(code_page, 0, input, (int)input_size, *output, length);
					if (RT_UNLIKELY(!length))
						goto error;
					/* Output size is the size of the buffer minus the terminating zero. */
					*output_size = heap_buffer_size - 1;

					/* MultiByteToWideChar does not write a zero terminating character. */
					(*output)[length] = 0;

				} else {
					/* Allocation failed. */
					goto error;
				}
			}
		} else {
			/* Buffer was too small and no heap has been provided. */
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

/**
 * Windows WideCharToMultiByte/MultiByteToWideChar does not manage UTF-16/UTF-32 conversions.<br>
 * This is done by this function.
 *
 * @param input Input without BOM. Must not be considered zero terminated.
 * @param input_size Input size in characters.
 * @param input_encoding Input encoding (with endianness).
 * @param input_code_unit_size Size of input characters.
 * @param output_encoding Output encoding (with optional endianess).
 * @param output_code_unit_size Size of output characters.
 * @param buffer_capacity Size of the buffer, in bytes.
 * @param output_size Receive the output size, in bytes, including the BOM if any.
 */
static rt_un rt_encoding_encode_or_decode_unicode_without_bom(const rt_char8 *input, rt_un input_size, enum rt_encoding input_encoding, rt_un input_code_unit_size, enum rt_encoding output_encoding, rt_un output_code_unit_size, rt_char8 *buffer, rt_un buffer_capacity, void **heap_buffer, rt_un *heap_buffer_capacity, rt_char8 **output, rt_un *output_size, struct rt_heap *heap)
{
	rt_un local_output_size;
	rt_char8 *output_payload; /* Pointer on output after optional BOM. */
	rt_un i;
	rt_s ret;

	if ((output_encoding == RT_ENCODING_UTF_16) ||
	    (output_encoding == RT_ENCODING_UTF_32)) {
		/* There is no more BOM in input, but we need space for one in output. */
		local_output_size = input_size + 1;
	} else {
		local_output_size = input_size;
	}

	/* Allocate resulting buffer, adding one for zero terminating character. */
	if (RT_UNLIKELY(!rt_heap_alloc_if_needed(buffer, buffer_capacity, heap_buffer, heap_buffer_capacity, (void**)output, (local_output_size + 1) * output_code_unit_size, heap)))
		goto error;

	/* Add little endian BOM if needed and set output payload pointer. */
	if (output_encoding == RT_ENCODING_UTF_16) {
		/* Always assume little endian on Windows. */
		(*output)[0] = (rt_char8)0xFF;
		(*output)[1] = (rt_char8)0xFE;
		output_payload = &(*output)[2];
	} else if (output_encoding == RT_ENCODING_UTF_32) {
		/* Always assume little endian on Windows. */
		(*output)[0] = (rt_char8)0xFF;
		(*output)[1] = (rt_char8)0xFE;
		(*output)[2] = (rt_char8)0x00;
		(*output)[3] = (rt_char8)0x00;
		output_payload = &(*output)[4];
	} else {
		output_payload = *output;
	}

	switch (input_encoding) {
	case RT_ENCODING_UTF_16LE:
		switch (output_encoding) {
		case RT_ENCODING_UTF_16:
		case RT_ENCODING_UTF_16LE:
			/* No transformation. */
			RT_MEMORY_COPY(input, output_payload, input_size * input_code_unit_size);
			break;
		case RT_ENCODING_UTF_16BE:
			/* Swap 16. */
			rt_memory_swap_bytes16((rt_un16*)input, (rt_un16*)output_payload, input_size);
			break;
		case RT_ENCODING_UTF_32:
		case RT_ENCODING_UTF_32LE:
			rt_encoding_copy_16_le_to_32_le((rt_un16*)input, (rt_un32*)output_payload, input_size);
			break;
		case RT_ENCODING_UTF_32BE:
			rt_encoding_copy_16_le_to_32_be((rt_un16*)input, (rt_un32*)output_payload, input_size);
			break;
		default:
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		}
		break;
	case RT_ENCODING_UTF_16BE:
		switch (output_encoding) {
		case RT_ENCODING_UTF_16:
		case RT_ENCODING_UTF_16LE:
			/* Swap 16. */
			rt_memory_swap_bytes16((rt_un16*)input, (rt_un16*)output_payload, input_size);
			break;
		case RT_ENCODING_UTF_16BE:
			/* No transformation. */
			RT_MEMORY_COPY(input, output_payload, input_size * input_code_unit_size);
			break;
		case RT_ENCODING_UTF_32:
		case RT_ENCODING_UTF_32LE:
			rt_encoding_copy_16_be_to_32_le((rt_un16*)input, (rt_un32*)output_payload, input_size);
			break;
		case RT_ENCODING_UTF_32BE:
			rt_encoding_copy_16_be_to_32_be((rt_un16*)input, (rt_un32*)output_payload, input_size);
			break;
		default:
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		}
		break;
	case RT_ENCODING_UTF_32LE:
		switch (output_encoding) {
		case RT_ENCODING_UTF_16:
		case RT_ENCODING_UTF_16LE:
			rt_encoding_copy_32_le_to_16_le((rt_un32*)input, (rt_un16*)output_payload, input_size);
			break;
		case RT_ENCODING_UTF_16BE:
			rt_encoding_copy_32_le_to_16_be((rt_un32*)input, (rt_un16*)output_payload, input_size);
			break;
		case RT_ENCODING_UTF_32:
		case RT_ENCODING_UTF_32LE:
			/* No transformation. */
			RT_MEMORY_COPY(input, output_payload, input_size * input_code_unit_size);
			break;
		case RT_ENCODING_UTF_32BE:
			/* Swap 32. */
			rt_memory_swap_bytes32((rt_un32*)input, (rt_un32*)output_payload, input_size);
			break;
		default:
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		}
		break;
	case RT_ENCODING_UTF_32BE:
		switch (output_encoding) {
		case RT_ENCODING_UTF_16:
		case RT_ENCODING_UTF_16LE:
			rt_encoding_copy_32_be_to_16_le((rt_un32*)input, (rt_un16*)output_payload, input_size);
			break;
		case RT_ENCODING_UTF_16BE:
			rt_encoding_copy_32_be_to_16_be((rt_un32*)input, (rt_un16*)output_payload, input_size);
			break;
		case RT_ENCODING_UTF_32:
		case RT_ENCODING_UTF_32LE:
			/* Swap 32. */
			rt_memory_swap_bytes32((rt_un32*)input, (rt_un32*)output_payload, input_size);
			break;
		case RT_ENCODING_UTF_32BE:
			/* No transformation. */
			RT_MEMORY_COPY(input, output_payload, input_size * input_code_unit_size);
			break;
		default:
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		}
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	/* Returned output size is the size in bytes without counting the zero terminating bytes. */
	local_output_size = local_output_size * output_code_unit_size;
	*output_size = local_output_size;

	/* Add terminating zero bytes. */
	for (i = 0; i < output_code_unit_size; i++)
		(*output)[local_output_size + i] = 0;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

/**
 * Windows WideCharToMultiByte/MultiByteToWideChar does not manage UTF-16/UTF-32 conversions.<br>
 * This is done by this function that manages input BOM (if needed) and call <tt>rt_encoding_encode_or_decode_unicode_without_bom</tt>.
 *
 * @param input Input with or without BOM. Must not be considered zero terminated.
 * @param input_size Input size in characters.
 * @param input_encoding Input encoding (with optional endianness).
 * @param input_code_unit_size Size of input characters.
 * @param output_encoding Output encoding (with optional endianess).
 * @param output_code_unit_size Size of output characters.
 * @param buffer_capacity Size of the buffer, in bytes.
 * @param output_size Receive the output size, in bytes, including the BOM if any.
 */
static rt_un rt_encoding_encode_or_decode_unicode(const rt_char8 *input, rt_un input_size, enum rt_encoding input_encoding, rt_un input_code_unit_size, enum rt_encoding output_encoding, rt_un output_code_unit_size, rt_char8 *buffer, rt_un buffer_capacity, void **heap_buffer, rt_un *heap_buffer_capacity, rt_char8 **output, rt_un *output_size, struct rt_heap *heap)
{
	const rt_char8 *input_payload; /* Pointer on input after optional BOM. */
	rt_un input_payload_size;
	rt_un input_payload_encoding; /* Input encoding with endianness. */
	rt_s ret;

	if (input_encoding == RT_ENCODING_UTF_16) {
		/* We already checked that size is > 0. */
		if (((rt_uchar8)input[0] == 0xFF) && ((rt_uchar8)input[1] == 0xFE)) {
			input_payload_encoding = RT_ENCODING_UTF_16LE;
		} else if (((rt_uchar8)input[0] == 0xFE) && ((rt_uchar8)input[1] == 0xFF)) {
			input_payload_encoding = RT_ENCODING_UTF_16BE;
		} else {
			/* Invalid BOM. */
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		}

		/* Skip BOM. */
		input_payload = &input[2];

		/* Remove BOM from size. */
		input_payload_size = input_size - 1;
	} else if (input_encoding == RT_ENCODING_UTF_32) {
		/* We already checked that size is > 0. */
		if (((rt_uchar8)input[0] == 0xFF) && ((rt_uchar8)input[1] == 0xFE) && ((rt_uchar8)input[2] == 0x00) && ((rt_uchar8)input[3] == 0x00)) {
			input_payload_encoding = RT_ENCODING_UTF_32LE;
		} else if (((rt_uchar8)input[0] == 0x00) && ((rt_uchar8)input[1] == 0x00) && ((rt_uchar8)input[2] == 0xFE) && ((rt_uchar8)input[3] == 0xFF)) {
			input_payload_encoding = RT_ENCODING_UTF_32BE;
		} else {
			/* Invalid BOM. */
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		}

		/* Skip BOM. */
		input_payload = &input[4];

		/* Remove BOM from size. */
		input_payload_size = input_size - 1;
	} else {
		/* No BOM for given encoding. */
		input_payload_encoding = input_encoding;
		input_payload = input;
		input_payload_size = input_size;
	}

	if (RT_UNLIKELY(!rt_encoding_encode_or_decode_unicode_without_bom(input_payload, input_payload_size, input_payload_encoding, input_code_unit_size, output_encoding, output_code_unit_size, buffer, buffer_capacity, heap_buffer, heap_buffer_capacity, output, output_size, heap)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

#else

/**
 * Call Linux iconv to perform the encoding/decoding.
 *
 * @param input_size Input size, in bytes.
 * @param buffer_capacity Buffer capacity, in bytes.
 * @param heap_buffer_capacity Heap buffer capacity, in bytes.
 * @param output_size Output size, in bytes.
 */
static rt_s rt_encoding_iconv_with_descriptor(const rt_char8 *input, rt_un input_size, iconv_t conversion_descriptor, enum rt_encoding output_encoding, rt_char8 *buffer, rt_un buffer_capacity, void **heap_buffer, rt_un *heap_buffer_capacity, rt_char8 **output, rt_un *output_size, struct rt_heap *heap)
{
	rt_char8 *iconv_input = (rt_char8*)input;
	size_t iconv_input_size = input_size;
	rt_char8 *iconv_buffer = buffer;
	rt_char8 *previous_iconv_buffer;
	rt_un iconv_buffer_capacity;
	rt_un output_code_unit_size;
	rt_b flush = RT_FALSE;
	size_t result;
	rt_b use_heap;
	rt_un i;
	rt_s ret;

	output_code_unit_size = rt_encoding_code_unit_sizes[output_encoding];

	/* Makes sure that the buffer is enough for the terminating zero. */
	if (buffer_capacity >= output_code_unit_size) {

		/* Save some room for the terminating zero. */
		iconv_buffer_capacity = buffer_capacity - output_code_unit_size;

buffer_flush:
		if (flush) {
			result = iconv(conversion_descriptor, RT_NULL, RT_NULL, &iconv_buffer, &iconv_buffer_capacity);
		} else {
			result = iconv(conversion_descriptor, &iconv_input, &iconv_input_size, &iconv_buffer, &iconv_buffer_capacity);
		}
		if (result == (size_t)-1) {
			/* Something went wrong. */
			/* Output buffer too small. errno == E2BIG. */
			/* Invalid character sequence. errno == EILSEQ. */
			/* Incomplete character sequence. errno == EINVAL. */
			if (RT_LIKELY(errno == E2BIG)) {
				/* Buffer was too small. */
				use_heap = RT_TRUE;

				/* Make sure a first heap buffer is ready. */
				if (RT_UNLIKELY(!rt_heap_alloc_if_needed(RT_NULL, 0, heap_buffer, heap_buffer_capacity, (void**)output, buffer_capacity * 2, heap)))
					goto error;

				/* Copy from the stack buffer to the heap buffer as we will resume the encoding in there. */
				RT_MEMORY_COPY(buffer, *heap_buffer, buffer_capacity);

				/* Adjust iconv_buffer and iconv_buffer_capacity for next iconv call. */
				*output_size = iconv_buffer - buffer;
				iconv_buffer = &((rt_char8*)*heap_buffer)[*output_size];
				iconv_buffer_capacity = *heap_buffer_capacity - *output_size - output_code_unit_size;

			} else {
				/* Invalid/incomplete character sequence. */
				goto error;
			}
		} else {
			if (flush) {
				/* Conversion was successful and the buffer was enough. */
				use_heap = RT_FALSE;

				*output = buffer;
				*output_size = iconv_buffer - buffer;
			} else {
				flush = RT_TRUE;
				goto buffer_flush;
			}
		}
	} else {
		/* The buffer was way too small. */
		use_heap = RT_TRUE;

		/* Allocate an heap buffer twice as big as the input. */
		if (RT_UNLIKELY(!rt_heap_alloc_if_needed(RT_NULL, 0, heap_buffer, heap_buffer_capacity, (void**)output, input_size * 2, heap)))
			goto error;

		/* Adjust iconv_buffer and iconv_buffer_capacity for next iconv call. */
		iconv_buffer = *heap_buffer;
		iconv_buffer_capacity = *heap_buffer_capacity - output_code_unit_size;

		*output_size = 0;
	}

	/* If buffer was not enough. */
	if (use_heap) {
		if (RT_LIKELY(heap)) {
			while (RT_TRUE) {
heap_buffer_flush:
				previous_iconv_buffer = iconv_buffer;
				if (flush) {
					result = iconv(conversion_descriptor, RT_NULL, RT_NULL, &iconv_buffer, &iconv_buffer_capacity);
				} else {
					result = iconv(conversion_descriptor, &iconv_input, &iconv_input_size, &iconv_buffer, &iconv_buffer_capacity);
				}
				if (result == (size_t)-1) {
					/* Something went wrong. */
					/* Output buffer too small. errno == E2BIG. */
					/* Invalid character sequence. errno == EILSEQ. */
					/* Incomplete character sequence. errno == EINVAL. */
					if (RT_UNLIKELY(errno != E2BIG)) {
						/* Invalid/incomplete character sequence. */
						goto error;
					}
				} else {
					*output_size += iconv_buffer - previous_iconv_buffer;
					if (flush) {
						/* Point on reallocated area. */
						*output = *heap_buffer;

						/* Conversion was successful. Exit the infinite loop. */
						break;
					} else {
						flush = RT_TRUE;
						goto heap_buffer_flush;
					}
				}

				*output_size += iconv_buffer - previous_iconv_buffer;

				/* Allocate a bigger heap buffer then the loop will try again. */
				if (RT_UNLIKELY(!heap->realloc(heap, heap_buffer, *heap_buffer_capacity * 2)))
					goto error;

				/* Adjust iconv_buffer and iconv_buffer_capacity for next iconv call. */
				iconv_buffer = &((rt_char8*)*heap_buffer)[*output_size];
				iconv_buffer_capacity += *heap_buffer_capacity;

				/* Specify new heap buffer size. */
				*heap_buffer_capacity *= 2;
			}
		} else {
			/* Buffer was too small and no heap has been provided. */
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}
	}

	/* Add the terminating zero. We know that we have enough space at this point. */
	for (i = 0; i < output_code_unit_size; i++)
		(*output)[*output_size + i] = 0;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

/**
 * Build and iconv conversion descriptor from <tt>input_encoding</tt> and <tt>output_encoding</tt> then call <tt>rt_encoding_iconv_with_descriptor</tt>.
 */
static rt_s rt_encoding_iconv_with_encoding(const rt_char8 *input, rt_un input_size, enum rt_encoding input_encoding, enum rt_encoding output_encoding, rt_char8 *buffer, rt_un buffer_capacity, void **heap_buffer, rt_un *heap_buffer_capacity, rt_char8 **output, rt_un *output_size, struct rt_heap *heap)
{
	rt_char system_encoding_name[64];
	rt_un system_encoding_name_size;
	const rt_char8 *input_encoding_name;
	const rt_char8 *output_encoding_name;
	rt_b conversion_descriptor_open = RT_FALSE;
	iconv_t conversion_descriptor;
	rt_s ret;

	/* Retrieve the encoding name as a string. */
	/* Either input_encoding or output_encoding is RT_ENCODING_SYSTEM_DEFAULT so we will use the result of this function call. */
	if (RT_UNLIKELY(!rt_encoding_get_linux_system(system_encoding_name, 64, &system_encoding_name_size)))
		goto error;

	if (input_encoding == RT_ENCODING_SYSTEM_DEFAULT) {
		input_encoding_name = system_encoding_name;
	} else {
		input_encoding_name = rt_encoding_code_names[input_encoding];
	}
	if (output_encoding == RT_ENCODING_SYSTEM_DEFAULT) {
		output_encoding_name = system_encoding_name;
	} else {
		output_encoding_name = rt_encoding_code_names[output_encoding];
	}

	/* In case of error, iconv_open sets errno and returns (iconv_t)-1. */
	conversion_descriptor = iconv_open(output_encoding_name, input_encoding_name);
	if (RT_UNLIKELY(conversion_descriptor == (iconv_t)-1)) {
		goto error;
	}
	conversion_descriptor_open = RT_TRUE;

	if (RT_UNLIKELY(!rt_encoding_iconv_with_descriptor(input, input_size, conversion_descriptor, output_encoding, buffer, buffer_capacity, heap_buffer, heap_buffer_capacity, output, output_size, heap)))
		goto error;

	ret = RT_OK;
free:
	if (conversion_descriptor_open) {
		 /* In case of error, iconv_close sets errno and returns -1. */
		if (RT_UNLIKELY((iconv_close(conversion_descriptor) == -1) && ret)) {
			conversion_descriptor_open = RT_FALSE;
			goto error;
		}
		conversion_descriptor_open = RT_FALSE;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
#endif

rt_s rt_encoding_encode(const rt_char *input, rt_un input_size, enum rt_encoding output_encoding, rt_char8 *buffer, rt_un buffer_capacity, void **heap_buffer, rt_un *heap_buffer_capacity, rt_char8 **output, rt_un *output_size, struct rt_heap *heap)
{
#ifdef RT_DEFINE_WINDOWS
	rt_un output_code_unit_size;
#endif
	rt_un empty_size;
	rt_s ret;

	if (input_size) {

#ifdef RT_DEFINE_WINDOWS
		if ((output_encoding == RT_ENCODING_UTF_16)   ||
		    (output_encoding == RT_ENCODING_UTF_16LE) ||
		    (output_encoding == RT_ENCODING_UTF_16BE) ||
		    (output_encoding == RT_ENCODING_UTF_32)   ||
		    (output_encoding == RT_ENCODING_UTF_32LE) ||
		    (output_encoding == RT_ENCODING_UTF_32BE)) {

				output_code_unit_size = rt_encoding_code_unit_sizes[output_encoding];

				if (RT_UNLIKELY(!rt_encoding_encode_or_decode_unicode((rt_char8*)input, input_size, RT_ENCODING_UTF_16LE, sizeof(rt_char), output_encoding, output_code_unit_size, buffer, buffer_capacity, heap_buffer, heap_buffer_capacity, output, output_size, heap)))
					goto error;
		} else {
			if (RT_UNLIKELY(!rt_encoding_encode_using_windows(input, input_size, output_encoding, buffer, buffer_capacity, heap_buffer, heap_buffer_capacity, output, output_size, heap)))
				goto error;
		}

#else

		if (RT_UNLIKELY(!rt_encoding_iconv_with_encoding(input, input_size, RT_ENCODING_SYSTEM_DEFAULT, output_encoding, buffer, buffer_capacity, heap_buffer, heap_buffer_capacity, output, output_size, heap)))
			goto error;

#endif

	} else {
		if (output_encoding == RT_ENCODING_UTF_16)
			empty_size = 4;
		else if (output_encoding == RT_ENCODING_UTF_32)
			empty_size = 8;
		else if (output_encoding == RT_ENCODING_UTF_16LE ||
			 output_encoding == RT_ENCODING_UTF_16BE)
			empty_size = 2;
		else if (output_encoding == RT_ENCODING_UTF_32LE ||
			 output_encoding == RT_ENCODING_UTF_32BE)
			empty_size = 4;
		else
			empty_size = 1;

		/* Empty input, just write the terminating zero. */
		if (RT_UNLIKELY(!rt_heap_alloc_if_needed(buffer, buffer_capacity, heap_buffer, heap_buffer_capacity, (void**)output, empty_size, heap)))
			goto error;

		if (output_encoding == RT_ENCODING_UTF_16) {
			(*output)[0] = (rt_char8)0xFF;
			(*output)[1] = (rt_char8)0xFE;
			(*output)[2] = (rt_char8)0x00;
			(*output)[3] = (rt_char8)0x00;
			*output_size = 2;
		} else if (output_encoding == RT_ENCODING_UTF_32) {
			(*output)[0] = (rt_char8)0xFF;
			(*output)[1] = (rt_char8)0xFE;
			(*output)[2] = (rt_char8)0x00;
			(*output)[3] = (rt_char8)0x00;
			(*output)[4] = (rt_char8)0x00;
			(*output)[5] = (rt_char8)0x00;
			(*output)[6] = (rt_char8)0x00;
			(*output)[7] = (rt_char8)0x00;
			*output_size = 4;
		} else if (output_encoding == RT_ENCODING_UTF_16LE ||
			   output_encoding == RT_ENCODING_UTF_16BE) {
			(*output)[0] = 0;
			(*output)[1] = 0;
			*output_size = 0;
		} else if (output_encoding == RT_ENCODING_UTF_32LE ||
			   output_encoding == RT_ENCODING_UTF_32BE) {
			(*output)[0] = 0;
			(*output)[1] = 0;
			(*output)[2] = 0;
			(*output)[3] = 0;
			*output_size = 0;
		} else {
			(*output)[0] = 0;
			*output_size = 0;
		}
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_encoding_decode(const rt_char8 *input, rt_un input_size, enum rt_encoding input_encoding, rt_char *buffer, rt_un buffer_capacity, void **heap_buffer, rt_un *heap_buffer_capacity, rt_char **output, rt_un *output_size, struct rt_heap *heap)
{
#ifdef RT_DEFINE_WINDOWS
	rt_un input_code_unit_size;
	rt_un actual_input_size;
#endif
	rt_s ret;

	if (input_size) {

#ifdef RT_DEFINE_WINDOWS
		if ((input_encoding == RT_ENCODING_UTF_16)   ||
		    (input_encoding == RT_ENCODING_UTF_16LE) ||
		    (input_encoding == RT_ENCODING_UTF_16BE) ||
		    (input_encoding == RT_ENCODING_UTF_32)   ||
		    (input_encoding == RT_ENCODING_UTF_32LE) ||
		    (input_encoding == RT_ENCODING_UTF_32BE)) {

			input_code_unit_size = rt_encoding_code_unit_sizes[input_encoding];

			actual_input_size = input_size / input_code_unit_size;

			if (RT_UNLIKELY(!rt_encoding_encode_or_decode_unicode(input, actual_input_size, input_encoding, input_code_unit_size, RT_ENCODING_UTF_16LE, sizeof(rt_char), (rt_char8*)buffer, buffer_capacity * sizeof(rt_char), heap_buffer, heap_buffer_capacity, (rt_char8**)output, output_size, heap)))
				goto error;

			(*output_size) /= sizeof(rt_char);

		} else {
			if (RT_UNLIKELY(!rt_encoding_decode_using_windows(input, input_size, input_encoding, buffer, buffer_capacity, heap_buffer, heap_buffer_capacity, output, output_size, heap)))
				goto error;
		}

#else

		if (RT_UNLIKELY(!rt_encoding_iconv_with_encoding(input, input_size, input_encoding, RT_ENCODING_SYSTEM_DEFAULT, buffer, buffer_capacity, heap_buffer, heap_buffer_capacity, output, output_size, heap)))
			goto error;

#endif
	} else {
		/* Empty input, just write the terminating zero. */
		if (RT_UNLIKELY(!rt_heap_alloc_if_needed(buffer, buffer_capacity * sizeof(rt_char), heap_buffer, heap_buffer_capacity, (void**)output, sizeof(rt_char), heap)))
			goto error;

		(*output)[0] = 0;
		*output_size = 0;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

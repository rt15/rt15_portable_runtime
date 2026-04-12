#include "layer005/rt_unicode_code_point.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer004/rt_encoding.h"

rt_s rt_unicode_code_point_encode(rt_un32 code_point, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
#ifdef RT_DEFINE_WINDOWS
	return rt_unicode_code_point_encode_to_utf16(code_point, buffer, buffer_capacity, buffer_size);
#else
	enum rt_encoding system_encoding;
	rt_char *output;
	rt_un output_size;
	rt_s ret = RT_FAILED;

	/* rt_encoding_get_system returns RT_ENCODING_SYSTEM_DEFAULT in case of error. */
	rt_encoding_get_system(&system_encoding);
	if (system_encoding == RT_ENCODING_UTF_8) {
		if (RT_UNLIKELY(!rt_unicode_code_point_encode_to_utf8(code_point, buffer, buffer_capacity, buffer_size)))
			goto end;
	} else {
		if (RT_UNLIKELY(!rt_encoding_decode((rt_char8*)&code_point, 4, RT_ENCODING_UTF_32LE, &buffer[*buffer_size], buffer_capacity - *buffer_size, RT_NULL, RT_NULL, &output, &output_size, RT_NULL)))
			goto end;
		*buffer_size += output_size;
	}

	ret = RT_OK;
end:
	return ret;
#endif
}

rt_s rt_unicode_code_point_encode_to_utf8(rt_un32 code_point, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_s ret = RT_FAILED;

	if (code_point <= 0x7F) {
		if (RT_UNLIKELY(buffer_capacity - *buffer_size < 2)) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto end;
		}

		buffer[*buffer_size] = (rt_uchar8)code_point;
		buffer[*buffer_size + 1] = 0;

		*buffer_size += 1;

	} else if (code_point <= 0x7FF) {
		if (RT_UNLIKELY(buffer_capacity - *buffer_size < 3)) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto end;
		}

		buffer[*buffer_size] = (rt_uchar8)((code_point >> 6) | 0xC0);
		buffer[*buffer_size + 1] = (rt_uchar8)((code_point & 0x3F) | 0x80);
		buffer[*buffer_size + 2] = 0;

		*buffer_size += 2;

	} else if (code_point <= 0xFFFF) {
		if (RT_UNLIKELY(buffer_capacity - *buffer_size < 4)) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto end;
		}

		buffer[*buffer_size] = (rt_uchar8)((code_point >> 12) | 0xE0);
		buffer[*buffer_size + 1] = (rt_uchar8)(((code_point >> 6) & 0x3F) | 0x80);
		buffer[*buffer_size + 2] = (rt_uchar8)((code_point & 0x3F) | 0x80);
		buffer[*buffer_size + 3] = 0;

		*buffer_size += 3;

	} else if (code_point <= 0x10FFFF) {
		if (RT_UNLIKELY(buffer_capacity - *buffer_size < 5)) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto end;
		}

		buffer[*buffer_size] = (rt_uchar8)((code_point >> 18) | 0xF0);
		buffer[*buffer_size + 1] = (rt_uchar8)(((code_point >> 12) & 0x3F) | 0x80);
		buffer[*buffer_size + 2] = (rt_uchar8)(((code_point >> 6) & 0x3F) | 0x80);
		buffer[*buffer_size + 3] = (rt_uchar8)((code_point & 0x3F) | 0x80);
		buffer[*buffer_size + 4] = 0;

		*buffer_size += 4;

	} else {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_unicode_code_point_encode_to_utf16(rt_un32 code_point, rt_un16 *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_s ret = RT_FAILED;

	if (code_point <= 0xFFFF) {
		if (RT_UNLIKELY(buffer_capacity - *buffer_size < 2)) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto end;
		}
		buffer[*buffer_size] = (rt_un16)code_point;
		buffer[*buffer_size + 1] = 0;

		*buffer_size += 1;
	} else {
		if (RT_UNLIKELY(buffer_capacity - *buffer_size < 3)) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto end;
		}
		code_point -= 0x10000;
		/* First 10 bits in the lead surrogate. */
		buffer[*buffer_size] = (rt_un16)(0xD800 + (code_point >> 10));
		/* Last 10 bits in the trail surrogate. */
		buffer[*buffer_size + 1] = (rt_un16)(0xDC00 + (code_point & 0x3FF));
		buffer[*buffer_size + 2] = 0;

		*buffer_size += 2;
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_unicode_code_point_decode(const rt_char *str, rt_un str_size, rt_un32 *code_point, rt_un *characters_read)
{
#ifdef RT_DEFINE_WINDOWS
	return rt_unicode_code_point_decode_from_utf16(str, str_size, code_point, characters_read);
#else
	enum rt_encoding system_encoding;
	rt_char system_encoding_name[64];
	rt_un system_encoding_name_size;
	const rt_char8 *input_encoding_name;
	const rt_char8 *output_encoding_name;
	iconv_t conversion_descriptor;
	rt_b conversion_descriptor_open = RT_FALSE;
	size_t result;
	rt_char8 *iconv_input = str;
	size_t iconv_input_size = str_size;
	rt_char8 *iconv_output = (rt_char8*)code_point;
	size_t iconv_output_size = 4;
	rt_s ret = RT_FAILED;

	/* rt_encoding_get_system returns RT_ENCODING_SYSTEM_DEFAULT in case of error. */
	rt_encoding_get_system(&system_encoding);

	if (system_encoding == RT_ENCODING_UTF_8) {
		if (RT_UNLIKELY(!rt_unicode_code_point_decode_from_utf8((rt_char8*)str, str_size, code_point, characters_read)))
			goto end;
	} else {
		system_encoding_name_size = 0;
		if (RT_UNLIKELY(!rt_encoding_get_system_name(system_encoding_name, 64, &system_encoding_name_size)))
			goto end;

		input_encoding_name = system_encoding_name;
		output_encoding_name = "UTF-32LE";

		/* In case of error, iconv_open sets errno and returns (iconv_t)-1. */
		conversion_descriptor = iconv_open(output_encoding_name, input_encoding_name);
		if (RT_UNLIKELY(conversion_descriptor == (iconv_t)-1)) {
			goto end;
		}
		conversion_descriptor_open = RT_TRUE;

		/* iconv returns (size_t)-1 and set errno in case of error. */
		result = iconv(conversion_descriptor, &iconv_input, &iconv_input_size, &iconv_output, &iconv_output_size);
		if (RT_UNLIKELY(result == (size_t)-1))
			goto end;

		/* The output buffer should have been entirely filled. */
		if (RT_UNLIKELY(iconv_output_size != 0)) {
			rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
			goto end;
		}

		*characters_read = str_size - iconv_input_size;
	}

	ret = RT_OK;
end:
	if (conversion_descriptor_open) {
		 /* In case of error, iconv_close sets errno and returns -1. */
		if (RT_UNLIKELY((iconv_close(conversion_descriptor) == -1)))
			ret = RT_FAILED;
	}

	return ret;
#endif
}

rt_s rt_unicode_code_point_decode_from_utf8(const rt_char8 *str, rt_un str_size, rt_un32 *code_point, rt_un *characters_read)
{
	rt_uchar8 first_byte;
	rt_s ret = RT_FAILED;

	if (!str_size) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	first_byte = (rt_uchar8)str[0];
	if (first_byte <= 0x7F) {
		/* ASCII. */
		*code_point = first_byte;
		*characters_read = 1;
	} else if (first_byte <= 0xDF) {
		/* 2 bytes. */
		if (RT_UNLIKELY(str_size < 2)) {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto end;
		}
		*code_point = ((first_byte & 0x1F) << 6) | (str[1] & 0x3F);
		*characters_read = 2;
	} else if (first_byte <= 0xEF) {
		/* 3 bytes. */
		if (RT_UNLIKELY(str_size < 3)) {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto end;
		}
		*code_point = ((first_byte & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F);
		*characters_read = 3;
	} else if (first_byte <= 0xF7) {
		/* 4 bytes. */
		if (RT_UNLIKELY(str_size < 4)) {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto end;
		}
		*code_point = ((first_byte & 0x07) << 18) | ((str[1] & 0x3F) << 12) | ((str[2] & 0x3F) << 6) | (str[3] & 0x3F);
		*characters_read = 4;
	} else {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_unicode_code_point_decode_from_utf16(const rt_un16 *str, rt_un str_size, rt_un32 *code_point, rt_un *characters_read)
{
	rt_un16 first_word;
	rt_s ret = RT_FAILED;

	if (!str_size) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	first_word = str[0];
	if (first_word < 0xD800 || first_word > 0xDFFF) {
		/* Not a surrogate, just return the code point. */
		*code_point = first_word;
		*characters_read = 1;
	} else if (first_word <= 0xDBFF) {
		/* Lead surrogate. */
		if (RT_UNLIKELY(str_size < 2)) {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto end;
		}
		if (RT_UNLIKELY(str[1] < 0xDC00 || str[1] > 0xDFFF)) {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto end;
		}
		*code_point = ((first_word - 0xD800) << 10) + (str[1] - 0xDC00) + 0x10000;
		*characters_read = 2;
	} else {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

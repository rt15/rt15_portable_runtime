#include "layer005/rt_unicode_code_point.h"

#include "layer002/rt_error.h"
#include "layer004/rt_encoding.h"

rt_s rt_unicode_code_point_encode(rt_un32 code_point, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
#ifdef RT_DEFINE_WINDOWS
	return rt_unicode_code_point_encode_to_utf16(code_point, buffer, buffer_capacity, buffer_size);
#else
	enum rt_encoding system_encoding;
	rt_char *output;
	rt_s ret;

	/* rt_encoding_get_system returns RT_ENCODING_SYSTEM_DEFAULT in case of error. */
	rt_encoding_get_system(&system_encoding);
	if (system_encoding == RT_ENCODING_UTF_8) {
		if (RT_UNLIKELY(!rt_unicode_code_point_encode_to_utf8(code_point, buffer, buffer_capacity, buffer_size)))
			goto error;
	} else {
		if (RT_UNLIKELY(!rt_encoding_decode((rt_char8*)&code_point, 4, RT_ENCODING_UTF_32LE, buffer, buffer_capacity, RT_NULL, RT_NULL, &output, buffer_size, RT_NULL)))
			goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
#endif
}

rt_s rt_unicode_code_point_encode_to_utf8(rt_un32 code_point, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_s ret;

	if (code_point <= 0x7F) {
		if (RT_UNLIKELY(buffer_capacity < 2)) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}

		buffer[0] = (rt_uchar8)code_point;
		buffer[1] = 0;

		*buffer_size = 1;

	} else if (code_point <= 0x7FF) {
		if (RT_UNLIKELY(buffer_capacity < 3)) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}

		buffer[0] = (rt_uchar8)((code_point >> 6) | 0xC0);
		buffer[1] = (rt_uchar8)((code_point & 0x3F) | 0x80);
		buffer[2] = 0;

		*buffer_size = 2;

	} else if (code_point <= 0xFFFF) {
		if (RT_UNLIKELY(buffer_capacity < 4)) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}

		buffer[0] = (rt_uchar8)((code_point >> 12) | 0xE0);
		buffer[1] = (rt_uchar8)(((code_point >> 6) & 0x3F) | 0x80);
		buffer[2] = (rt_uchar8)((code_point & 0x3F) | 0x80);
		buffer[3] = 0;

		*buffer_size = 3;

	} else if (code_point <= 0x10FFFF) {
		if (RT_UNLIKELY(buffer_capacity < 5)) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}

		buffer[0] = (rt_uchar8)((code_point >> 18) | 0xF0);
		buffer[1] = (rt_uchar8)(((code_point >> 12) & 0x3F) | 0x80);
		buffer[2] = (rt_uchar8)(((code_point >> 6) & 0x3F) | 0x80);
		buffer[3] = (rt_uchar8)((code_point & 0x3F) | 0x80);
		buffer[4] = 0;

		*buffer_size = 4;

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

rt_s rt_unicode_code_point_encode_to_utf16(rt_un32 code_point, rt_un16 *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_s ret;

	if (code_point <= 0xFFFF) {
		if (RT_UNLIKELY(buffer_capacity < 2)) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}
		buffer[0] = (rt_un16)code_point;
		buffer[1] = 0;

		*buffer_size = 1;
	} else {
		if (RT_UNLIKELY(buffer_capacity < 3)) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}
		code_point -= 0x10000;
		/* First 10 bits in the lead surrogate. */
		buffer[0] = (rt_un16)(0xD800 + (code_point >> 10));
		/* Last 10 bits in the trail surrogate. */
		buffer[1] = (rt_un16)(0xDC00 + (code_point & 0x3FF));
		buffer[2] = 0;

		*buffer_size = 2;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

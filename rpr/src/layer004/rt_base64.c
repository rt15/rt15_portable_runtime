#include "layer004/rt_base64.h"

#include "layer001/rt_memory.h"
#include "layer002/rt_error.h"
#include "layer003/rt_char8.h"

/*
https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64
*/

#define RT_BASE64_WHITESPACE 64
#define RT_BASE64_EQUALS 65
#define RT_BASE64_INVALID 66

static const rt_char8 rt_base64_characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const rt_uchar8 rt_base64_decode_characters[] = {
	66,66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
	66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
	54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
	29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
	66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
	66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
	66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
	66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
	66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
	66,66,66,66,66,66
};

rt_s rt_base64_encode(const rt_char8 *data, rt_un data_size, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	const rt_uchar8 *unsigned_data = (rt_uchar8*)data;
	rt_un32 number;
	rt_un index_in_buffer = 0;
	rt_uchar8 six_bits0;
	rt_uchar8 six_bits1;
	rt_uchar8 six_bits2;
	rt_uchar8 six_bits3;
	rt_un i;
	rt_s ret;

	/* Increment over the length of the data, three characters at a time. */
	for (i = 0; i < data_size; i += 3) {
		/* These three 8-bit characters become one 24-bit number. */
		number = ((rt_un32)unsigned_data[i]) << 16;

		/* Is there a second character available? */
		if ((i + 1) < data_size) {
			number += ((rt_un32)unsigned_data[i + 1]) << 8;
		}
		/* Is there a third character available? */
		if ((i + 2) < data_size) {
			number += unsigned_data[i + 2];
		}

		/* This 24-bit number gets separated into four 6-bit numbers. */
		six_bits0 = (rt_uchar8)(number >> 18) & 63;
		six_bits1 = (rt_uchar8)(number >> 12) & 63;
		six_bits2 = (rt_uchar8)(number >> 6) & 63;
		six_bits3 = (rt_uchar8)number & 63;

		/* If we have one byte available (8 bits), then its encoding is spread out over two characters (2 * 6 bits). */
		if (index_in_buffer >= buffer_capacity) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}
		buffer[index_in_buffer++] = rt_base64_characters[six_bits0];
		if (index_in_buffer >= buffer_capacity) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}
		buffer[index_in_buffer++] = rt_base64_characters[six_bits1];

		/* If we have only two bytes available (16 bits), then their encoding is spread out over three characters (3 * 6 bits) . */
		if ((i + 1) < data_size) {
			if (index_in_buffer >= buffer_capacity) {
				rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
				goto error;
			}
			buffer[index_in_buffer++] = rt_base64_characters[six_bits2];
		}

		/* If we have all three bytes available (24 bits), then their encoding is spread out over four characters (4 * 6 bits). */
		if ((i + 2) < data_size) {
			if (index_in_buffer >= buffer_capacity) {
				rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
				goto error;
			}
			buffer[index_in_buffer++] = rt_base64_characters[six_bits3];
		}
	}

	/* Add padding that is required if we did not have a multiple of 4 number of characters in output. */
	while (RT_MEMORY_MODULO_POWER_OF_TWO(index_in_buffer, 4)) {
		if (index_in_buffer >= buffer_capacity) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}
		buffer[index_in_buffer++] = '=';
	}

	/* Zero terminating character. */
	if (index_in_buffer >= buffer_capacity) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto error;
	}
	buffer[index_in_buffer] = 0;

	ret = RT_OK;
free:
	*buffer_size = index_in_buffer;
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_base64_decode(const rt_char8 *base64, rt_un base64_size, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	const rt_char8 *end = base64 + base64_size;
	rt_un32 temp_buffer = 0;
	rt_un output_size = 0;
	rt_uchar8 character;
	rt_un i = 0;
	rt_s ret;

	while (base64 < end) {
		character = rt_base64_decode_characters[(rt_uchar8)*base64++];

		switch (character) {
		case RT_BASE64_WHITESPACE:
			/* Skip whitespace. */
			continue;
		case RT_BASE64_INVALID:
			/* Invalid input. */
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		case RT_BASE64_EQUALS:
			/* Pad character, end of data. */
			base64 = end;
			continue;
		default:
			temp_buffer = temp_buffer << 6 | character;
			 /* Increment the number of iterations. */
			i++;
			/* If the buffer is full, split it into bytes */
			if (i == 4) {
				if ((output_size += 3) > buffer_capacity) {
					rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
					goto error;
				}
				*(buffer++) = (rt_uchar8)((temp_buffer >> 16) & 255);
				*(buffer++) = (rt_uchar8)((temp_buffer >> 8) & 255);
				*(buffer++) = (rt_uchar8)(temp_buffer & 255);
				temp_buffer = 0;
				i = 0;
			}
		}
	}

	if (i == 3) {

		if ((output_size += 2) > buffer_capacity) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}
		*(buffer++) = (rt_uchar8)((temp_buffer >> 10) & 255);
		*(buffer++) = (rt_uchar8)((temp_buffer >> 2) & 255);

	} else if (i == 2) {

		if (++output_size > buffer_capacity) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}
		*(buffer++) = (rt_uchar8)((temp_buffer >> 4) & 255);
	}

	/* Zero terminating character. */
	if (output_size + 1 > buffer_capacity) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto error;
	}
	*(buffer++) = 0;

	ret = RT_OK;
free:
	*buffer_size = output_size;
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

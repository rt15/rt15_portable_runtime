#include "layer005/rt_deduce_encoding.h"

#include "layer002/rt_io_device.h"

rt_s rt_deduce_encoding(rt_char8 *data, rt_un data_size, enum rt_encoding *encoding, rt_un *bom_size)
{
	rt_uchar8 *unsigned_data = (rt_uchar8*)data;
	rt_b ascii;
	rt_b utf_8;
	rt_uchar8 current_char;
	rt_un i;

	/* UTF-32 BOM? */
	if (data_size >= 4) {
		if (unsigned_data[0] == 0xFF &&
		    unsigned_data[1] == 0xFE &&
		    unsigned_data[2] == 0x00 &&
		    unsigned_data[3] == 0x00) {
			*encoding = RT_ENCODING_UTF_32LE;
			*bom_size = 4;
			goto end;
		}
		if (unsigned_data[0] == 0x00 &&
		    unsigned_data[1] == 0x00 &&
		    unsigned_data[2] == 0xFE &&
		    unsigned_data[3] == 0xFF) {
			*encoding = RT_ENCODING_UTF_32BE;
			*bom_size = 4;
			goto end;
		}
	}

	/* UTF-8 BOM? */
	if (data_size >= 3 &&
	    unsigned_data[0] == 0xEF &&
	    unsigned_data[1] == 0xBB &&
	    unsigned_data[2] == 0xBF) {
		*encoding = RT_ENCODING_UTF_8;
		*bom_size = 3;
		goto end;
	}

	/* UTF-16 BOM? */
	if (data_size >= 2) {
		if (unsigned_data[0] == 0xFF &&
		    unsigned_data[1] == 0xFE) {
			*encoding = RT_ENCODING_UTF_16LE;
			*bom_size = 2;
			goto end;
		}
		if (unsigned_data[0] == 0xFE &&
		    unsigned_data[1] == 0xFF) {
			*encoding = RT_ENCODING_UTF_16BE;
			*bom_size = 2;
			goto end;
		}
	}

	/* No BOM found. */
	*bom_size = 0;

	/* UTF-16 without BOM. */
	if (data_size >= 4) {
		if (unsigned_data[0] &&
		   !unsigned_data[1] &&
		    unsigned_data[2] &&
		   !unsigned_data[3]) {
			*encoding = RT_ENCODING_UTF_16LE;
			goto end;
		}
		if (!unsigned_data[0] &&
		     unsigned_data[1] &&
		    !unsigned_data[2] &&
		     unsigned_data[3]) {
			*encoding = RT_ENCODING_UTF_16BE;
			goto end;
		}
	}

	/* UTF-32 without BOM. */
	if (data_size >= 8) {
		if (unsigned_data[0] &&
		   !unsigned_data[1] &&
		   !unsigned_data[2] &&
		   !unsigned_data[3] &&
		    unsigned_data[4] &&
		   !unsigned_data[5] &&
		   !unsigned_data[6] &&
		   !unsigned_data[7]) {
			*encoding = RT_ENCODING_UTF_32LE;
			goto end;
		}
		if (!unsigned_data[0] &&
		    !unsigned_data[1] &&
		    !unsigned_data[2] &&
		     unsigned_data[3] &&
		    !unsigned_data[4] &&
		    !unsigned_data[5] &&
		    !unsigned_data[6] &&
		     unsigned_data[7]) {
			*encoding = RT_ENCODING_UTF_32BE;
			goto end;
		}
	}

	ascii = RT_TRUE;
	utf_8 = RT_TRUE;
	for (i = 0; i < data_size; i++) {

		current_char = unsigned_data[i];

		/* ASCII. */
		if (current_char < 0x80)
			continue;

		/* At least one character is not ASCII. */
		ascii = RT_FALSE;

		/* The range [1000 0000 (0x80), 1100 0000 (0xC0)[ is not valid for UTF-8. */
		if (current_char < 0xC0) {
			utf_8 = RT_FALSE;
			break;

		} else if (current_char < 0xE0) {
			/* The range [1100 0000 (0xC0), 1110 0000 (0xE0)[ is for 2 bytes characters. */
			/* Following byte must look like 10XX XXXX. */
			if ((i + 1 >= data_size) || ((unsigned_data[i + 1] & 0xC0) != 0x80)) {
				utf_8 = RT_FALSE;
				break;
			}
			i += 1;

		} else if (current_char < 0xF0) {
			/* The range [1110 0000 (0xE0), 1111 0000 (0xF0)[ is for 3 bytes characters. */
			/* Following bytes must look like 10XX XXXX. */
			if ((i + 2 >= data_size) || ((unsigned_data[i + 1] & 0xC0) != 0x80) ||
			                            ((unsigned_data[i + 2] & 0xC0) != 0x80)) {
				utf_8 = RT_FALSE;
				break;
			}
			i += 2;

		} else if (current_char < 0xF8) {
			/* The range [1111 0000 (0xF0), 1111 1000 (0xF8)[ is for 4 bytes characters. */
			/* Following bytes must look like 10XX XXXX. */
			if ((i + 3 >= data_size) || ((unsigned_data[i + 1] & 0xC0) != 0x80) ||
			                            ((unsigned_data[i + 2] & 0xC0) != 0x80) ||
			                            ((unsigned_data[i + 3] & 0xC0) != 0x80)) {
				utf_8 = RT_FALSE;
				break;
			}
			i += 3;

		} else {
			/* More or equals to 1111 1000 (0xF8) is not UTF-8. */
			utf_8 = RT_FALSE;
			break;
		}
	}

	if (ascii) {
		*encoding = RT_ENCODING_US_ASCII;
		goto end;
	}

	if (utf_8) {
		*encoding = RT_ENCODING_UTF_8;
		goto end;
	}

#ifdef RT_DEFINE_WINDOWS
	*encoding = RT_ENCODING_SYSTEM_DEFAULT;
#else
	rt_encoding_get_system(encoding);
	if (*encoding == RT_ENCODING_UTF_8)
		*encoding = RT_ENCODING_ISO_8859_15;
#endif

end:
	return RT_OK;
}

rt_s rt_deduce_encoding_with_file(struct rt_file *file, rt_char8 *buffer, rt_un buffer_capacity, enum rt_encoding *encoding, rt_un *bom_size)
{
	rt_un64 old_position;
	rt_un bytes_read;
	rt_s ret;

	/* Backup the current position */
	if (RT_UNLIKELY(!rt_file_get_pointer(file, &old_position))) goto error;

	/* Go to the beginning of the file. */
	if (RT_UNLIKELY(!rt_file_set_pointer(file, 0, RT_FILE_POSITION_BEGIN))) goto error;

	/* Read the beginning of the file. */
	if (RT_UNLIKELY(!rt_io_device_read(&file->io_device.input_stream, buffer, buffer_capacity, &bytes_read))) goto error;

	if (RT_UNLIKELY(!rt_deduce_encoding(buffer, bytes_read, encoding, bom_size))) goto error;

	/* Go back to original position. */
	if (RT_UNLIKELY(!rt_file_set_pointer(file, old_position, RT_FILE_POSITION_BEGIN))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

#include <rpr.h>

#include "zz_utils.h"

#ifdef RT_DEFINE_WINDOWS
static  rt_un16 zz_utf8_3[3] = { 0x1158, 0x1158, 0x00 };
static  rt_un16 zz_utf8_4[5] = { 0xD800, 0xDF38, 0xD800, 0xDF38, 0x00 };
#else
static  rt_uchar8 zz_utf8_3[7] = { 0xE1, 0x85, 0x98, 0xE1, 0x85, 0x98, 0x00 };
static  rt_uchar8 zz_utf8_4[9] = { 0xF0, 0x90, 0x8C, 0xB8, 0xF0, 0x90, 0x8C, 0xB8, 0x00 };
#endif

static rt_s zz_test_encoding_deducer_do(const rt_char *encoding_dir, const rt_char *file_name, enum rt_encoding expected_encoding, rt_un expected_bom_size, rt_char *expected)
{
	rt_char file_path[RT_FILE_PATH_SIZE];
	rt_un file_path_size = rt_char_get_size(encoding_dir);
	rt_char8 file_buffer[64];
	rt_char8 *file_output;
	rt_un file_output_size;
	enum rt_encoding encoding;
	rt_un bom_size;
	rt_char buffer[64];
	rt_char *output;
	rt_un output_size;
	rt_s ret;

	if (!rt_char_copy(encoding_dir, file_path_size, file_path, RT_FILE_PATH_SIZE)) goto error;
	if (!rt_char_append(file_name, rt_char_get_size(file_name), file_path, RT_FILE_PATH_SIZE, &file_path_size)) goto error;

	if (!rt_small_file_read(file_path, file_buffer, 64, RT_NULL, RT_NULL, &file_output, &file_output_size, RT_NULL)) goto error;

	if (!rt_encoding_deducer_deduce(file_output, file_output_size, &encoding, &bom_size)) goto error;
	if (encoding != expected_encoding) goto error;
	if (bom_size != expected_bom_size) goto error;

	if (!rt_encoding_decode(&file_output[bom_size], file_output_size - bom_size, encoding, buffer, 64, RT_NULL, RT_NULL, &output, &output_size, RT_NULL)) goto error;
	if (rt_char_get_size(output) != output_size) goto error;
	if (!rt_char_equals(output, output_size, expected, rt_char_get_size(expected))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_encoding_deducer()
{
	rt_char encoding_dir[RT_FILE_PATH_SIZE];
	rt_un encoding_dir_size;
	rt_s ret;

	if (!zz_get_test_resources_dir(encoding_dir, RT_FILE_PATH_SIZE, &encoding_dir_size)) goto error;
	if (!rt_file_path_append_separator(encoding_dir, RT_FILE_PATH_SIZE, &encoding_dir_size)) goto error;
	if (!rt_char_append(_R("encoding"), 8, encoding_dir, RT_FILE_PATH_SIZE, &encoding_dir_size)) goto error;
	if (!rt_file_path_append_separator(encoding_dir, RT_FILE_PATH_SIZE, &encoding_dir_size)) goto error;

	if (!zz_test_encoding_deducer_do(encoding_dir, _R("ascii.dat"), RT_ENCODING_US_ASCII, 0, _R("ea"))) goto error;

#ifdef RT_DEFINE_WINDOWS
	if (!zz_test_encoding_deducer_do(encoding_dir, _R("latin1.dat"), RT_ENCODING_SYSTEM_DEFAULT, 0, _R("ιθ"))) goto error;
#else
	if (!zz_test_encoding_deducer_do(encoding_dir, _R("latin1.dat"), RT_ENCODING_ISO_8859_15, 0, _R("ιθ"))) goto error;
#endif

	if (!zz_test_encoding_deducer_do(encoding_dir, _R("utf_8_with_bom.dat"),    RT_ENCODING_UTF_8,    3, _R("ιθ"))) goto error;
	if (!zz_test_encoding_deducer_do(encoding_dir, _R("utf_16LE_with_bom.dat"), RT_ENCODING_UTF_16LE, 2, _R("ιθ"))) goto error;
	if (!zz_test_encoding_deducer_do(encoding_dir, _R("utf_16BE_with_bom.dat"), RT_ENCODING_UTF_16BE, 2, _R("ιθ"))) goto error;
	if (!zz_test_encoding_deducer_do(encoding_dir, _R("utf_32LE_with_bom.dat"), RT_ENCODING_UTF_32LE, 4, _R("ιθ"))) goto error;
	if (!zz_test_encoding_deducer_do(encoding_dir, _R("utf_32BE_with_bom.dat"), RT_ENCODING_UTF_32BE, 4, _R("ιθ"))) goto error;

	if (!zz_test_encoding_deducer_do(encoding_dir, _R("utf_8_2.dat"),  RT_ENCODING_UTF_8,    0, _R("ιθ")))            goto error;
	if (!zz_test_encoding_deducer_do(encoding_dir, _R("utf_8_3.dat"),  RT_ENCODING_UTF_8,    0, (rt_char*)zz_utf8_3)) goto error;
	if (!zz_test_encoding_deducer_do(encoding_dir, _R("utf_8_4.dat"),  RT_ENCODING_UTF_8,    0, (rt_char*)zz_utf8_4)) goto error;
	if (!zz_test_encoding_deducer_do(encoding_dir, _R("utf_16LE.dat"), RT_ENCODING_UTF_16LE, 0, _R("ιθ")))            goto error;
	if (!zz_test_encoding_deducer_do(encoding_dir, _R("utf_16BE.dat"), RT_ENCODING_UTF_16BE, 0, _R("ιθ")))            goto error;
	if (!zz_test_encoding_deducer_do(encoding_dir, _R("utf_32LE.dat"), RT_ENCODING_UTF_32LE, 0, _R("ιθ")))            goto error;
	if (!zz_test_encoding_deducer_do(encoding_dir, _R("utf_32BE.dat"), RT_ENCODING_UTF_32BE, 0, _R("ιθ")))            goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

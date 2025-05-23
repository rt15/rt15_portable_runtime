#include <rpr.h>

#include "zz_utils.h"

#define ZZ_EXPECTED_LINES_COUNT 11

enum zz_test_read_lines_test {
	ZZ_TEST_READ_LINES_TEST_SIMPLE,
	ZZ_TEST_READ_LINES_TEST_CR,
	ZZ_TEST_READ_LINES_TEST_CRLF,
	ZZ_TEST_READ_LINES_TEST_CR_CHAR
};

struct zz_test_read_lines_context {
	enum zz_test_read_lines_test test;
	rt_un lines_count;
	rt_un expected_lines_count;
};

static rt_char8 *zz_expected_lines[ZZ_EXPECTED_LINES_COUNT] = {
	"firt_line",
	"second_line",
	"oneCR",
	"twoCR",
	"",
	"twoLF",
	"",
	"CRLF",
	"LFCR",
	"",
	"last_line"
};

static enum rt_eol zz_expected_eols[ZZ_EXPECTED_LINES_COUNT] = {
	RT_EOL_LF,
	RT_EOL_LF,
	RT_EOL_CR,
	RT_EOL_CR,
	RT_EOL_CR,
	RT_EOL_LF,
	RT_EOL_LF,
	RT_EOL_CRLF,
	RT_EOL_LF,
	RT_EOL_CR,
	RT_EOL_NONE
};

static rt_s zz_test_read_lines_callback(const rt_char8 *line, rt_un line_size, enum rt_eol eol, void *context)
{
	struct zz_test_read_lines_context *test_read_lines_context = (struct zz_test_read_lines_context*)context;
	rt_un lines_count = test_read_lines_context->lines_count;
	rt_char8 *expected_line;
	enum rt_eol expected_eol;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(lines_count >= test_read_lines_context->expected_lines_count))
		goto end;

	switch (test_read_lines_context->test) {
	case ZZ_TEST_READ_LINES_TEST_SIMPLE:
		expected_line = zz_expected_lines[lines_count];
		expected_eol = zz_expected_eols[lines_count];
		break;

	case ZZ_TEST_READ_LINES_TEST_CR:
		expected_line = "123456789012345";
		expected_eol = RT_EOL_CR;
		break;
	case ZZ_TEST_READ_LINES_TEST_CRLF:
		if (lines_count == 0) {
			expected_line = "1234";
			expected_eol = RT_EOL_CR;
		} else {
			expected_line = "6789012345";
			expected_eol = RT_EOL_CRLF;
		}
		break;
	case ZZ_TEST_READ_LINES_TEST_CR_CHAR:
		if (lines_count == 0) {
			expected_line = "1234";
			expected_eol = RT_EOL_CR;
		} else if (lines_count == 1) {
			expected_line = "6789012345";
			expected_eol = RT_EOL_CR;
		} else {
			expected_line = "a";
			expected_eol = RT_EOL_NONE;
		}
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	if (RT_UNLIKELY(!rt_char8_equals(line, line_size, expected_line, rt_char8_get_size(expected_line))))
		goto end;

	if (RT_UNLIKELY(eol != expected_eol))
		goto end;

	test_read_lines_context->lines_count++;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_read_lines_do(const rt_char *test_resources_dir, const rt_char *file_name, enum zz_test_read_lines_test test, rt_un expected_lines_count)
{
	rt_char file_path[RT_FILE_PATH_SIZE];
	rt_un file_path_size;
	struct rt_file file;
	rt_b file_created = RT_FALSE;
	rt_char8 buffer[16];
	struct zz_test_read_lines_context context;
	rt_s ret = RT_FAILED;

	file_path_size = rt_char_get_size(test_resources_dir);
	if (RT_UNLIKELY(!rt_char_copy(test_resources_dir, file_path_size, file_path, RT_FILE_PATH_SIZE))) goto end;
	if (RT_UNLIKELY(!rt_file_path_append_separator(file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append(file_name, rt_char_get_size(file_name), file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto end;

	if (RT_UNLIKELY(!rt_file_create(&file, file_path, RT_FILE_MODE_READ))) goto end;
	file_created = RT_TRUE;

	context.test = test;
	context.lines_count = 0;
	context.expected_lines_count = expected_lines_count;
	if (RT_UNLIKELY(!rt_read_lines(&file.io_device.input_stream, buffer, 16, &zz_test_read_lines_callback, &context)))
		goto end;

	if (RT_UNLIKELY(context.lines_count != expected_lines_count))
		goto end;

	ret = RT_OK;
end:
	if (file_created) {
		if (RT_UNLIKELY(!rt_io_device_free(&file.io_device)))
			ret = RT_FAILED;
	}

	return ret;
}

static rt_s zz_test_read_lines_create_source_file(const rt_char *source_file_path)
{
	struct rt_file file;
	rt_b file_created = RT_FALSE;
	struct rt_output_stream *output_stream;
	rt_char8 buffer[RT_CHAR8_BIG_STRING_SIZE];
	rt_un i, j;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_file_create(&file, source_file_path, RT_FILE_MODE_TRUNCATE))) goto end;
	file_created = RT_TRUE;

	output_stream = &file.io_device.output_stream;

	for (j = 0; j < 24; j++) {
		if (RT_UNLIKELY(!rt_random_get_bytes(buffer, RT_CHAR8_BIG_STRING_SIZE)))
			goto end;
		for (i = 0; i < RT_CHAR8_BIG_STRING_SIZE; i++) {
			if ((rt_uchar8)buffer[i] < 127) {
				buffer[i] = 'a';
			} else if ((rt_uchar8)buffer[i] < 187) {
				buffer[i] = '\n';
			} else {
				buffer[i] = '\r';
			}
		}
		if (RT_UNLIKELY(!output_stream->write(output_stream, buffer, RT_CHAR8_BIG_STRING_SIZE)))
			goto end;
	}

	ret = RT_OK;
end:
	if (file_created) {
		if (RT_UNLIKELY(!rt_io_device_free(&file.io_device)))
			ret = RT_FAILED;
	}

	return ret;
}

static rt_s zz_test_read_lines_copy_file_callback(const rt_char8 *line, rt_un line_size, enum rt_eol eol, void *context)
{
	struct rt_output_stream *output_stream = (struct rt_output_stream*)context;
	rt_char8 *end_of_line;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!output_stream->write(output_stream, line, line_size)))
		goto end;

	switch (eol) {
	case RT_EOL_NONE:
		end_of_line = RT_NULL;
		break;
	case RT_EOL_LF:
		end_of_line = "\n";
		break;
	case RT_EOL_CRLF:
		end_of_line = "\r\n";
		break;
	case RT_EOL_CR:
		end_of_line = "\r";
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	if (end_of_line) {
		if (RT_UNLIKELY(!output_stream->write(output_stream, end_of_line, rt_char8_get_size(end_of_line))))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_read_lines_create_destination_file(const rt_char *source_file_path, const rt_char *destination_file_path)
{
	struct rt_file source_file;
	rt_b source_file_created = RT_FALSE;
	struct rt_input_stream *input_stream;
	struct rt_file destination_file;
	rt_b destination_file_created = RT_FALSE;
	struct rt_buffered_output_stream buffered_output_stream;
	rt_char8 read_buffer[24];
	rt_char8 write_buffer[4];
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_file_create(&source_file, source_file_path, RT_FILE_MODE_READ))) goto end;
	source_file_created = RT_TRUE;

	input_stream = &source_file.io_device.input_stream;

	if (RT_UNLIKELY(!rt_file_create(&destination_file, destination_file_path, RT_FILE_MODE_TRUNCATE))) goto end;
	destination_file_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_buffered_output_stream_create(&buffered_output_stream, &destination_file.io_device.output_stream, write_buffer, 4))) goto end;

	if (RT_UNLIKELY(!rt_read_lines(input_stream, read_buffer, 24, &zz_test_read_lines_copy_file_callback, &buffered_output_stream))) goto end;

	if (RT_UNLIKELY(!buffered_output_stream.output_stream.flush(&buffered_output_stream.output_stream))) goto end;

	ret = RT_OK;
end:
	if (destination_file_created) {
		if (RT_UNLIKELY(!rt_io_device_free(&destination_file.io_device)))
			ret = RT_FAILED;
	}
	if (source_file_created) {
		if (RT_UNLIKELY(!rt_io_device_free(&source_file.io_device)))
			ret = RT_FAILED;
	}

	return ret;
}

static rt_s zz_test_read_lines_copy_file_do(const rt_char *source_file_path, const rt_char *destination_file_path)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_read_lines_create_source_file(source_file_path))) goto end;
	if (RT_UNLIKELY(!zz_test_read_lines_create_destination_file(source_file_path, destination_file_path))) goto end;
	if (RT_UNLIKELY(!zz_check_same_files_content(source_file_path, destination_file_path))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_read_lines_copy_file(const rt_char* tmp_dir, rt_un tmp_dir_size)
{
	rt_char source_file_path[RT_FILE_PATH_SIZE];
	rt_un source_file_path_size = tmp_dir_size;
	rt_char destination_file_path[RT_FILE_PATH_SIZE];
	rt_un destination_file_path_size = tmp_dir_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_char_copy(tmp_dir, source_file_path_size, source_file_path, RT_FILE_PATH_SIZE))) goto end;
	if (RT_UNLIKELY(!rt_file_path_append_separator(source_file_path, RT_FILE_PATH_SIZE, &source_file_path_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append(_R("source.dat"), 10, source_file_path, RT_FILE_PATH_SIZE, &source_file_path_size))) goto end;

	if (RT_UNLIKELY(!rt_char_copy(tmp_dir, destination_file_path_size, destination_file_path, RT_FILE_PATH_SIZE))) goto end;
	if (RT_UNLIKELY(!rt_file_path_append_separator(destination_file_path, RT_FILE_PATH_SIZE, &destination_file_path_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append(_R("destination.dat"), 15, destination_file_path, RT_FILE_PATH_SIZE, &destination_file_path_size))) goto end;

	if (RT_UNLIKELY(!zz_test_read_lines_copy_file_do(source_file_path, destination_file_path))) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_read_lines(void)
{
	rt_char dir[RT_FILE_PATH_SIZE];
	rt_un dir_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_get_test_resources_dir(dir, RT_FILE_PATH_SIZE, &dir_size))) goto end;

	if (RT_UNLIKELY(!zz_test_read_lines_do(dir, _R("lines.dat"), ZZ_TEST_READ_LINES_TEST_SIMPLE, ZZ_EXPECTED_LINES_COUNT))) goto end;
	if (RT_UNLIKELY(!zz_test_read_lines_do(dir, _R("lines_cr.dat"), ZZ_TEST_READ_LINES_TEST_CR, 1))) goto end;
	if (RT_UNLIKELY(!zz_test_read_lines_do(dir, _R("lines_crlf.dat"), ZZ_TEST_READ_LINES_TEST_CRLF, 2))) goto end;
	if (RT_UNLIKELY(!zz_test_read_lines_do(dir, _R("lines_cr_char.dat"), ZZ_TEST_READ_LINES_TEST_CR_CHAR, 3))) goto end;

	if (RT_UNLIKELY(!zz_get_tmp_dir(dir, RT_FILE_PATH_SIZE, &dir_size))) goto end;

	if (RT_UNLIKELY(!zz_test_read_lines_copy_file(dir, dir_size))) goto end;

	ret = RT_OK;
end:
	return ret;
}

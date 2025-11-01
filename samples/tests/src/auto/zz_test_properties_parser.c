#include <rpr.h>

#define ZZ_TEST_PROPERTIES_PARSER_EXPECTED_PARTS_COUNT 24

static const rt_char *const zz_test_properties_parser_expected_parts[ZZ_TEST_PROPERTIES_PARSER_EXPECTED_PARTS_COUNT] = {
	_R("foo\\\\"),
	_R("="),
	_R("bar\\\\"),
	_R("\n  "),
	_R("key"),
	_R("  "),
	_R(":"),
	_R("  "),
	_R("value with space  "),
	_R("\n\n"),
	_R("other_key\\:"),
	_R(" "),
	_R("other_value\\\n  #second\\tline"),
	_R("\n  "),
	_R("#comment"),
	_R("\n"),
	_R("!comment too"),
	_R("\n"),
	_R("whitespaceStart"),
	_R("="),
	_R("\\ "),
	_R("\n"),
	_R("key_without_value"),
	_R("")
};

static const enum rt_properties_parser_part_type zz_test_properties_parser_expected_part_types[ZZ_TEST_PROPERTIES_PARSER_EXPECTED_PARTS_COUNT] = {
	RT_PROPERTIES_PARSER_PART_TYPE_KEY,
	RT_PROPERTIES_PARSER_PART_TYPE_SEPARATOR,
	RT_PROPERTIES_PARSER_PART_TYPE_VALUE,
	RT_PROPERTIES_PARSER_PART_TYPE_BLANKS,
	RT_PROPERTIES_PARSER_PART_TYPE_KEY,
	RT_PROPERTIES_PARSER_PART_TYPE_BLANKS,
	RT_PROPERTIES_PARSER_PART_TYPE_SEPARATOR,
	RT_PROPERTIES_PARSER_PART_TYPE_BLANKS,
	RT_PROPERTIES_PARSER_PART_TYPE_VALUE,
	RT_PROPERTIES_PARSER_PART_TYPE_BLANKS,
	RT_PROPERTIES_PARSER_PART_TYPE_KEY,
	RT_PROPERTIES_PARSER_PART_TYPE_BLANKS,
	RT_PROPERTIES_PARSER_PART_TYPE_VALUE,
	RT_PROPERTIES_PARSER_PART_TYPE_BLANKS,
	RT_PROPERTIES_PARSER_PART_TYPE_COMMENT,
	RT_PROPERTIES_PARSER_PART_TYPE_BLANKS,
	RT_PROPERTIES_PARSER_PART_TYPE_COMMENT,
	RT_PROPERTIES_PARSER_PART_TYPE_BLANKS,
	RT_PROPERTIES_PARSER_PART_TYPE_KEY,
	RT_PROPERTIES_PARSER_PART_TYPE_SEPARATOR,
	RT_PROPERTIES_PARSER_PART_TYPE_VALUE,
	RT_PROPERTIES_PARSER_PART_TYPE_BLANKS,
	RT_PROPERTIES_PARSER_PART_TYPE_KEY,
	RT_PROPERTIES_PARSER_PART_TYPE_VALUE
};

static rt_s zz_test_properties_parser_callback(enum rt_properties_parser_part_type part_type, const rt_char *str, rt_un index, rt_un size, void *context)
{
	rt_un *part_index = (rt_un*)context;
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_char *header;
	const rt_char *expected_part;
	enum rt_properties_parser_part_type expected_part_type;
	rt_s ret = RT_FAILED;

	switch (part_type) {
		case RT_PROPERTIES_PARSER_PART_TYPE_KEY:
			header = _R("Key: \"");
			break;
		case RT_PROPERTIES_PARSER_PART_TYPE_SEPARATOR:
			header = _R("Separator: \"");
			break;
		case RT_PROPERTIES_PARSER_PART_TYPE_VALUE:
			header = _R("Value: \"");
			break;
		case RT_PROPERTIES_PARSER_PART_TYPE_BLANKS:
			header = _R("Blanks: \"");
			break;
		case RT_PROPERTIES_PARSER_PART_TYPE_COMMENT:
			header = _R("Comment: \"");
			break;
		default:
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto end;
	}

	buffer_size = rt_char_get_size(header);
	if (RT_UNLIKELY(!rt_char_copy(header, buffer_size, buffer, RT_CHAR_HALF_BIG_STRING_SIZE))) goto end;
	if (RT_UNLIKELY(!rt_char_append(&str[index], size, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append(_R("\"\n"), 2, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;

	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size))) goto end;

	expected_part_type = zz_test_properties_parser_expected_part_types[*part_index];
	if (RT_UNLIKELY(part_type != expected_part_type))
		goto end;

	expected_part = zz_test_properties_parser_expected_parts[*part_index];
	if (RT_UNLIKELY(!rt_char_equals(&str[index], size, expected_part, rt_char_get_size(expected_part))))
		goto end;

	(*part_index)++;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_properties_parser(void)
{
	rt_un part_index = 0;
	rt_char *str = _R("foo\\\\=bar\\\\\n  key  :  value with space  \n\nother_key\\: other_value\\\n  #second\\tline\n  #comment\n!comment too\nwhitespaceStart=\\ \nkey_without_value");
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_properties_parser_parse(str, rt_char_get_size(str), &zz_test_properties_parser_callback, &part_index))) goto end;

	ret = RT_OK;
end:
	return ret;
}

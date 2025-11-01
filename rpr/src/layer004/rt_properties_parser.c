#include "layer004/rt_properties_parser.h"

#include "layer003/rt_char.h"

static rt_s rt_properties_parser_parse_blanks(const rt_char *str, rt_un str_size, rt_un *index, rt_properties_parser_callback_t callback, void *context)
{
	rt_un start_index = *index;
	rt_s ret = RT_FAILED;

	while (*index < str_size && RT_CHAR_IS_BLANK(str[*index])) {
		(*index)++;
	}

	if (start_index != *index) {
		if (RT_UNLIKELY(!callback(RT_PROPERTIES_PARSER_PART_TYPE_BLANKS, str, start_index, *index - start_index, context)))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s rt_properties_parser_parse_key(const rt_char *str, rt_un str_size, rt_un *index, rt_properties_parser_callback_t callback, void *context)
{
	rt_un start_index = *index;
	rt_b was_backslash = RT_FALSE;
	rt_char current_char;
	rt_s ret = RT_FAILED;

	while (*index < str_size) {
		current_char = str[*index];
		if (!was_backslash) {
			if (current_char == _R('=') || current_char == _R(':') || RT_CHAR_IS_BLANK(current_char))
				break;
		}
		if (current_char == _R('\\')) {
			if (was_backslash) {
				was_backslash = RT_FALSE;
			} else {
				was_backslash = RT_TRUE;
			}
		} else {
			was_backslash = RT_FALSE;
		}
		(*index)++;
	}

	if (RT_UNLIKELY(!callback(RT_PROPERTIES_PARSER_PART_TYPE_KEY, str, start_index, *index - start_index, context)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s rt_properties_parser_parse_separator(const rt_char *str, rt_un str_size, rt_un *index, rt_properties_parser_callback_t callback, void *context)
{
	rt_un start_index = *index;
	rt_char current_char;
	rt_s ret = RT_FAILED;

	if (*index < str_size) {
		current_char = str[*index];
		if (current_char == _R('=') || current_char == _R(':')) {
			/* Consume the separator character. */
			(*index)++;
		}
	}

	if (start_index != *index) {
		if (RT_UNLIKELY(!callback(RT_PROPERTIES_PARSER_PART_TYPE_SEPARATOR, str, start_index, *index - start_index, context)))
			goto end;
	}
	
	ret = RT_OK;
end:
	return ret;
}

static rt_s rt_properties_parser_parse_value(const rt_char *str, rt_un str_size, rt_un *index, rt_properties_parser_callback_t callback, void *context)
{
	rt_un start_index = *index;
	rt_b was_backslash = RT_FALSE;
	rt_char current_char;
	rt_s ret = RT_FAILED;

	while (*index < str_size) {
		current_char = str[*index];
		if (!was_backslash) {
			if (current_char == _R('\n') || current_char == _R('\r'))
				break;
		}
		if (current_char == _R('\\')) {
			if (was_backslash) {
				was_backslash = RT_FALSE;
			} else {
				was_backslash = RT_TRUE;
			}
		} else {
			was_backslash = RT_FALSE;
		}

		(*index)++;
	}

	if (RT_UNLIKELY(!callback(RT_PROPERTIES_PARSER_PART_TYPE_VALUE, str, start_index, *index - start_index, context)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s rt_properties_parser_parse_comment(const rt_char *str, rt_un str_size, rt_un *index, rt_properties_parser_callback_t callback, void *context)
{
	rt_un start_index = *index;
	rt_char current_char;
	rt_s ret = RT_FAILED;

	while (*index < str_size) {
		current_char = str[*index];
		if (current_char == _R('\n') || current_char == _R('\r'))
			break;

		(*index)++;
	}

	if (RT_UNLIKELY(!callback(RT_PROPERTIES_PARSER_PART_TYPE_COMMENT, str, start_index, *index - start_index, context)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_properties_parser_parse(const rt_char *str, rt_un str_size, rt_properties_parser_callback_t callback, void *context)
{
	rt_un index = 0;
	rt_char current_char;
	rt_s ret = RT_FAILED;

	while (index < str_size) {
		/* Parse blanks until we reach the key  or a comment. */
		if (RT_UNLIKELY(!rt_properties_parser_parse_blanks(str, str_size, &index, callback, context)))
			goto end;

		if (index < str_size) {

			current_char = str[index];
			if (current_char == _R('#') || current_char == _R('!')) {

				/* Parse comment. */
				if (RT_UNLIKELY(!rt_properties_parser_parse_comment(str, str_size, &index, callback, context)))
					goto end;

			} else {
				/* Parse the key. */
				if (RT_UNLIKELY(!rt_properties_parser_parse_key(str, str_size, &index, callback, context)))
					goto end;

				/* Parse blanks between the key and the separator. */
				if (RT_UNLIKELY(!rt_properties_parser_parse_blanks(str, str_size, &index, callback, context)))
					goto end;
				
				/* Parse the separator. */
				if (RT_UNLIKELY(!rt_properties_parser_parse_separator(str, str_size, &index, callback, context)))
					goto end;

				/* Parse blanks between the separator and the value. */
				if (RT_UNLIKELY(!rt_properties_parser_parse_blanks(str, str_size, &index, callback, context)))
					goto end;

				/* Parse the value. */
				if (RT_UNLIKELY(!rt_properties_parser_parse_value(str, str_size, &index, callback, context)))
					goto end;
			}
		}
	}

	ret = RT_OK;
end:
	return ret;
}

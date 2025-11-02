#include "layer006/rt_properties_parser.h"

#include "layer002/rt_error.h"
#include "layer003/rt_char.h"
#include "layer005/rt_unicode_code_point.h"

static rt_s rt_properties_parser_read_blanks(const rt_char *str, rt_un str_size, rt_un *index, rt_properties_parser_callback_t callback, void *context)
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

static rt_s rt_properties_parser_read_key(const rt_char *str, rt_un str_size, rt_un *index, rt_properties_parser_callback_t callback, void *context)
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

static rt_s rt_properties_parser_read_separator(const rt_char *str, rt_un str_size, rt_un *index, rt_properties_parser_callback_t callback, void *context)
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

static rt_s rt_properties_parser_read_value(const rt_char *str, rt_un str_size, rt_un *index, rt_properties_parser_callback_t callback, void *context)
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

static rt_s rt_properties_parser_read_comment(const rt_char *str, rt_un str_size, rt_un *index, rt_properties_parser_callback_t callback, void *context)
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
		/* Read blanks until we reach the key  or a comment. */
		if (RT_UNLIKELY(!rt_properties_parser_read_blanks(str, str_size, &index, callback, context)))
			goto end;

		if (index < str_size) {

			current_char = str[index];
			if (current_char == _R('#') || current_char == _R('!')) {

				/* Read comment. */
				if (RT_UNLIKELY(!rt_properties_parser_read_comment(str, str_size, &index, callback, context)))
					goto end;

			} else {
				/* Read the key. */
				if (RT_UNLIKELY(!rt_properties_parser_read_key(str, str_size, &index, callback, context)))
					goto end;

				/* Read blanks between the key and the separator. */
				if (RT_UNLIKELY(!rt_properties_parser_read_blanks(str, str_size, &index, callback, context)))
					goto end;
				
				/* Read the separator. */
				if (RT_UNLIKELY(!rt_properties_parser_read_separator(str, str_size, &index, callback, context)))
					goto end;

				/* Read blanks between the separator and the value. */
				if (RT_UNLIKELY(!rt_properties_parser_read_blanks(str, str_size, &index, callback, context)))
					goto end;

				/* Read the value. */
				if (RT_UNLIKELY(!rt_properties_parser_read_value(str, str_size, &index, callback, context)))
					goto end;
			}
		}
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s rt_properties_parser_parse_part(const rt_char *str, rt_un str_size, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_un backslash_index;
	rt_char next_char;
	rt_char new_char;
	rt_char digit;
	rt_un code_point;
	rt_un written_characters;
	rt_un i, j;
	rt_s ret = RT_FAILED;

	/* Search the first backslash. */
	backslash_index = RT_TYPE_MAX_UN;
	for (i = 0; i < str_size; i++) {
		if (str[i] == _R('\\')) {
			backslash_index = i;
			break;
		}
	}

	if (RT_LIKELY(backslash_index == RT_TYPE_MAX_UN)) {

		/* No backslash, just append the key or value. */
		if (RT_UNLIKELY(!rt_char_append(str, str_size, buffer, buffer_capacity, buffer_size)))
			goto end;

	} else {
		/* Append what was before the backslash. */
		if (backslash_index) {
			if (RT_UNLIKELY(!rt_char_append(str, backslash_index, buffer, buffer_capacity, buffer_size)))
				goto end;
		}

		/* Process the rest of the string, skipping the backslash. */
		i = backslash_index + 1;
		if (i < str_size) {
			next_char = str[i];
			if (next_char == _R('n') ||
			    next_char == _R('r') ||
			    next_char == _R('t') ||
			    next_char == _R('f') ||
			    next_char == _R('\\')) {

				switch (next_char) {
				case _R('n'):
					new_char = _R('\n');
					break;
				case _R('r'):
					new_char = _R('\r');
					break;
				case _R('t'):
					new_char = _R('\t');
					break;
				case _R('f'):
					new_char = _R('\f');
					break;
				case _R('\\'):
					new_char = _R('\\');
					break;
				default:
					rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
					goto end;
				}

				if (RT_UNLIKELY(!rt_char_append_char(new_char, buffer, buffer_capacity, buffer_size)))
					goto end;

				i++;

			} else if (next_char == _R('\n') || next_char == _R('\r')) {
				/* Line continuation, skip the end of line and all blanks at the beginning of the next line. */
				i++;
				while (i < str_size) {
					next_char = str[i];
					if (!RT_CHAR_IS_BLANK(next_char))
						break;
					i++;
				}
			} else if (next_char == _R('u')) {

				/* Skip the u character. */
				i++;

				/* Make sure there are at least 4 characters available. */
				if (RT_UNLIKELY(i + 4 > str_size)) {
					rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
					goto end;
				}

				/* Check that the next 4 characters look like hexa. */
				for (j = i; j < i + 4; j++) {
					digit = str[j];
					if (RT_UNLIKELY(digit < _R('0') ||
					               (digit > _R('9') && digit < _R('A')) ||
					               (digit > _R('F') && digit < _R('a')) ||
					                digit > _R('f'))) {
						rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
						goto end;
					}
				}

				/* Deduce the code point. */
				if (RT_UNLIKELY(!rt_char_convert_hex_to_un_with_size(&str[i], 4, &code_point)))
					goto end;

				/* Write the corresponding character. */
				if (RT_UNLIKELY(!rt_unicode_code_point_encode((rt_un32)code_point, &buffer[*buffer_size], buffer_capacity - *buffer_size, &written_characters)))
					goto end;
				*buffer_size += written_characters;

				/* Skip the 4 digits. */
				i += 4;
			}

			if (i < str_size) {
				if (RT_UNLIKELY(!rt_properties_parser_parse_part(&str[i], str_size - i, buffer, buffer_capacity, buffer_size)))
					goto end;
			}
		}
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_properties_parser_parse_key(const rt_char *str, rt_un str_size, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	return rt_properties_parser_parse_part(str, str_size, buffer, buffer_capacity, buffer_size);
}

rt_s rt_properties_parser_parse_value(const rt_char *str, rt_un str_size, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	return rt_properties_parser_parse_part(str, str_size, buffer, buffer_capacity, buffer_size);
}

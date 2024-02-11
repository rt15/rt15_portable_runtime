#include "layer003/rt_char.h"

#include "layer001/rt_memory.h"
#include "layer002/rt_error.h"

#ifdef RT_DEFINE_32
#define RT_CHAR_FNV_OFFSET_BASIS 2166136261u
#define RT_CHAR_FNV_PRIME 16777619u
#else
#define RT_CHAR_FNV_OFFSET_BASIS 14695981039346656037ull
#define RT_CHAR_FNV_PRIME 1099511628211ull
#endif

rt_b rt_char_equals(const rt_char *str1, rt_un str1_size, const rt_char *str2, rt_un str2_size)
{
	rt_b ret;

	if (str1_size != str2_size) {
		ret = RT_FALSE;
	} else {
		ret = !RT_MEMORY_COMPARE(str1, str2, str1_size * sizeof(rt_char));
	}
	return ret;
}

rt_n rt_char_compare(const rt_char *str1, const rt_char *str2)
{
	while (*str1 && (*str1 == *str2)) {
		str1++;
		str2++;
	}
	return *(rt_uchar*)str1 - *(rt_uchar*)str2;
}

rt_s rt_char_append(const rt_char *suffix, rt_un suffix_size, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_un local_buffer_size = *buffer_size;
	rt_un copy_size;
	rt_s ret;

	if (local_buffer_size + suffix_size + 1 <= buffer_capacity) {
		ret = RT_OK;
		copy_size = suffix_size;

	} else {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		ret = RT_FAILED;
		if (buffer_capacity > local_buffer_size + 1) {
			copy_size = buffer_capacity - (local_buffer_size + 1);
		} else {
			/* Add a final zero terminating character just in case. */
			if (buffer_capacity > 0) {
				buffer[buffer_capacity - 1] = 0;
				*buffer_size = buffer_capacity - 1;
			} else {
				buffer_size = 0;
			}
			goto end;
		}
	}

	RT_MEMORY_COPY(suffix, &buffer[local_buffer_size], copy_size * sizeof(rt_char));
	local_buffer_size += copy_size;
	buffer[local_buffer_size] = 0;
	*buffer_size = local_buffer_size;

end:
	return ret;
}

rt_s rt_char_append_char(rt_char character, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_un local_buffer_size = *buffer_size;
	rt_s ret;

	if (local_buffer_size + 1 < buffer_capacity) {
		buffer[local_buffer_size] = character;
		local_buffer_size++;
		buffer[local_buffer_size] = 0;
		*buffer_size = local_buffer_size;
		ret = RT_OK;
	} else {
		if (buffer_capacity > 0) {
			/* Make sure we put a zero terminating character. */
			buffer[buffer_capacity - 1] = 0;
			*buffer_size = buffer_capacity - 1;
		}
		ret = RT_FAILED;
	}
	return ret;
}

rt_s rt_char_copy(const rt_char *str, rt_un str_size, rt_char *buffer, rt_un buffer_capacity)
{
	rt_un copy_size;
	rt_s ret;

	if (str_size + 1 <= buffer_capacity) {
		ret = RT_OK;
		copy_size = str_size;

	} else {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		ret = RT_FAILED;
		if (buffer_capacity > 1) {
			copy_size = buffer_capacity - 1;
		} else {
			/* Add a final zero terminating character just in case. */
			if (buffer_capacity > 0)
				buffer[0] = 0;
			goto end;
		}
	}

	RT_MEMORY_COPY(str, buffer, copy_size * sizeof(rt_char));
	buffer[copy_size] = 0;

end:
	return ret;
}

rt_un rt_char_get_size(const rt_char *str)
{
	const rt_char *in_str;
	rt_un ret;

	in_str = str;
	while (*in_str++);

	ret = (rt_un)(in_str - str - 1);

	return ret;
}

rt_s rt_char_append_un(rt_un value, rt_un base, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_un previous_value;
	rt_char tmp_char;
	rt_un i, j;
	rt_s ret;

	if (RT_UNLIKELY(base < 2 || base > 36))
		goto error;

	i = *buffer_size;
	j = *buffer_size;
	while (i < buffer_capacity - 1) {
		previous_value = value;
		value = value / base;
		buffer[i] = _R("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ")[previous_value - value * base];
		i++;
		if (!value)
			break;
	}
	if (RT_UNLIKELY(value))
		goto error;

	buffer[i] = 0;
	*buffer_size = i;
	i--;
	for (; j < i; j++) {
		tmp_char = buffer[j];
		buffer[j] = buffer[i];
		buffer[i] = tmp_char;
		i--;
	}

	ret = RT_OK;
free:
	return ret;
error:
	/* Add a zero terminating character if possible. */
	if (buffer_capacity) {
		if (*buffer_size >= buffer_capacity)
			*buffer_size = buffer_capacity - 1;
		buffer[*buffer_size] = 0;
	}
	ret = RT_FAILED;
	goto free;
}

rt_s rt_char_append_n(rt_n value, rt_un base, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_n local_value;
	rt_n previous_value;
	rt_char tmp_char;
	rt_un i, j;
	rt_s ret;

	if (RT_UNLIKELY(base < 2 || base > 36))
		goto error;

	local_value = value;
	i = *buffer_size;
	j = *buffer_size;
	while (i < buffer_capacity - 1) {
		previous_value = local_value;
		local_value = local_value / (rt_n)base;
		buffer[i] = _R("ZYXWVUTSRQPONMLKJIHGFEDCBA9876543210123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ")[35 + (previous_value - local_value * base)];
		i++;
		if (!local_value)
			break;
	}
	if (RT_UNLIKELY(local_value))
		goto error;

	if (value < 0) {
		if (RT_UNLIKELY(i >= buffer_capacity - 1))
			goto error;
		buffer[i] = '-';
		i++;
	}
	buffer[i] = 0;
	*buffer_size = i;
	i--;
	for (; j < i; j++) {
		tmp_char = buffer[j];
		buffer[j] = buffer[i];
		buffer[i] = tmp_char;
		i--;
	}

	ret = RT_OK;
free:
	return ret;
error:
	/* Add a zero terminating character if possible. */
	if (buffer_capacity) {
		if (*buffer_size >= buffer_capacity)
			*buffer_size = buffer_capacity - 1;
		buffer[*buffer_size] = 0;
	}
	ret = RT_FAILED;
	goto free;
}

rt_un rt_char_fast_lower(rt_char *str)
{
	rt_char current_char;
	rt_un ret;

	ret = 0;
	while (str[ret]) {
		current_char = str[ret];
		if (current_char >= _R('A') && current_char <= _R('Z')) {
			str[ret] = current_char + 32;
		}
		ret++;
	}
	return ret;
}

rt_un rt_char_fast_upper(rt_char *str)
{
	rt_char current_char;
	rt_un ret;

	ret = 0;
	while (str[ret]) {
		current_char = str[ret];
		if (current_char >= _R('a') && current_char <= _R('z')) {
			str[ret] = current_char - 32;
		}
		ret++;
	}
	return ret;
}

rt_s rt_char_convert_to_un(const rt_char *str, rt_un *result)
{
	rt_char character;
	rt_un local_result = 0;
	rt_un i = 0;
	rt_s ret;

	while (RT_TRUE) {
		character = str[i];
		if (!character)
			break;

		if (RT_UNLIKELY((character < _R('0')) || (character > _R('9')))) {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		} else {
			local_result = local_result * 10 + character - _R('0');
		}
		i++;
	}

	if (RT_UNLIKELY(!i)) {
		/* The string was empty. */
		*result = 0;
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}
	*result = local_result;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_char_convert_to_un_with_size(const rt_char *str, rt_un str_size, rt_un *result)
{
	rt_char character;
	rt_un local_result = 0;
	rt_un i;
	rt_s ret;

	if (RT_UNLIKELY(!str_size)) {
		/* The string is empty. */
		*result = 0;
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	for (i = 0; i < str_size; i++) {
		character = str[i];

		if (RT_UNLIKELY((character < _R('0')) || (character > _R('9')))) {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		} else {
			local_result = local_result * 10 + character - _R('0');
		}
	}

	*result = local_result;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_char_convert_to_n(const rt_char *str, rt_n *result)
{
	rt_b negative;
	rt_char character;
	rt_n local_result = 0;
	rt_un i = 0;
	rt_s ret;

	if (str[0] == _R('-')) {
		negative = RT_TRUE;
		str = &str[1];
	} else {
		negative = RT_FALSE;
	}

	if (RT_UNLIKELY(!str[0])) {
		/* The string is empty or just a minus sign. */
		*result = 0;
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	while (RT_TRUE) {
		character = str[i];
		if (!character)
			break;

		if (RT_UNLIKELY((character < _R('0')) || (character > _R('9')))) {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		} else {
			local_result = local_result * 10 + character - _R('0');
		}
		i++;
	}

	if (negative)
		local_result = -local_result;

	*result = local_result;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_char_convert_to_n_with_size(const rt_char *str, rt_un str_size, rt_n *result)
{
	rt_b negative;
	rt_char character;
	rt_n local_result = 0;
	rt_un i;
	rt_s ret;

	if (RT_UNLIKELY(!str_size)) {
		/* The string is empty. */
		*result = 0;
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	if (str[0] == '-') {
		negative = RT_TRUE;
		if (RT_UNLIKELY(str_size == 1)) {
			/* The string is only a minus sign. */
			*result = 0;
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		}
	} else {
		negative = RT_FALSE;
	}

	for (i = (negative) ? 1 : 0; i < str_size; i++) {
		character = str[i];

		if (RT_UNLIKELY((character < _R('0')) || (character > _R('9')))) {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		} else {
			local_result = local_result * 10 + character - _R('0');
		}
	}

	if (negative)
		local_result = -local_result;

	*result = local_result;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_char_convert_hex_to_un(const rt_char *str, rt_un *result)
{
	rt_char character;
	rt_un local_result = 0;
	rt_uchar8 byte;
	rt_un i = 0;
	rt_s ret;

	while (RT_TRUE) {
		character = str[i];
		if (!character)
			break;

		if (character >= _R('0') && character <= _R('9')) {
			byte = character - _R('0');
		} else if (character >= _R('a') && character <= _R('f')) {
			byte = character - _R('a') + 10;
		} else if (character >= _R('A') && character <= _R('F')) {
			byte = character - _R('A') + 10;
		} else {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		}

		local_result = (local_result << 4);
		local_result |= byte;

		i++;
	}

	if (RT_UNLIKELY(!i)) {
		/* The string was empty. */
		*result = 0;
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}
	*result = local_result;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_char_convert_hex_to_un_with_size(const rt_char *str, rt_un str_size, rt_un *result)
{
	rt_char character;
	rt_un local_result = 0;
	rt_uchar8 byte;
	rt_un i;
	rt_s ret;

	if (RT_UNLIKELY(!str_size)) {
		/* The string is empty. */
		*result = 0;
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	for (i = 0; i < str_size; i++) {
		character = str[i];

		if (character >= _R('0') && character <= _R('9')) {
			byte = character - _R('0');
		} else if (character >= _R('a') && character <= _R('f')) {
			byte = character - _R('a') + 10;
		} else if (character >= _R('A') && character <= _R('F')) {
			byte = character - _R('A') + 10;
		} else {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		}

		local_result = (local_result << 4);
		local_result |= byte;
	}

	*result = local_result;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

void rt_char_trim(rt_b left, rt_b right, rt_char *buffer, rt_un *buffer_size)
{
	rt_un local_buffer_size = *buffer_size;
	rt_un i;

	if (right) {
		while (local_buffer_size && buffer[local_buffer_size - 1] <= _R(' '))
			local_buffer_size--;

		buffer[local_buffer_size] = 0;
	}

	if (left) {
		for (i = 0; i < local_buffer_size; i++) {
			if (buffer[i] > _R(' '))
				break;
		}
		if (i) {
			local_buffer_size -= i;
			if (local_buffer_size) {
				RT_MEMORY_COPY(&buffer[i], buffer, local_buffer_size * sizeof(rt_char));
			}
			buffer[local_buffer_size] = 0;
		}
	}

	*buffer_size = local_buffer_size;
}

rt_s rt_char_left_pad(const rt_char *input, rt_un input_size, rt_char character, rt_un size, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_un padding_size;
	rt_s ret;

	if (input_size < size) {

		/* Makes sure the buffer capacity is enough for the padding + input_size + zero terminating character. */
		if (RT_UNLIKELY(buffer_capacity < size + 1)) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}

		padding_size = size - input_size;
		if (input_size) {
			if (input == buffer) {
				RT_MEMORY_MOVE(input, &buffer[padding_size], input_size * sizeof(rt_char));
			} else {
				RT_MEMORY_COPY(input, &buffer[padding_size], input_size * sizeof(rt_char));
			}
		}

		/* Add padding at the left of the input. */
		RT_MEMORY_SET_CHAR(buffer, character, padding_size);

		/* Add zero terminating character. */
		buffer[size] = 0;

		*buffer_size = size;

	} else {
		/* If the result is the same as the input, then there is nothing to do. */
		if (input != buffer) {
			/* The input is already long enough. We copy it to the result. */
			if (RT_UNLIKELY(!rt_char_copy(input, input_size, buffer, buffer_capacity)))
				goto error;
		}
		*buffer_size = input_size;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_char_right_pad(rt_char character, rt_un size, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_un local_buffer_size = *buffer_size;
	rt_s ret;

	/* If padding is required. */
	if (local_buffer_size < size) {

		/* Makes sure the buffer is enough for its current content + padding + zero terminating character. */
		if (RT_UNLIKELY(buffer_capacity < size + 1)) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}

		RT_MEMORY_SET_CHAR(&buffer[local_buffer_size], character, size - local_buffer_size);

		/* Add zero terminating character. */
		buffer[size] = 0;

		*buffer_size = size;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_char_ends_with(const rt_char *str, rt_un str_size, const rt_char *searched, rt_un searched_size)
{
	const rt_char *str_end;
	rt_b ret;

	if (str_size >= searched_size) {
		if (searched_size) {
			str_end = &str[str_size - searched_size];
			ret = !RT_MEMORY_COMPARE(str_end, searched, searched_size * sizeof(rt_char));
		} else {
			ret = RT_TRUE;
		}
	} else {
		ret = RT_FALSE;
	}
	return ret;
}

rt_un rt_char_search(const rt_char *str, const rt_char *searched)
{
	rt_char first_searched_character = searched[0];
	rt_un in_str = 0;
	rt_char str_character;
	rt_un in_searched;
	rt_un in_str2;
	rt_char searched_character;
	rt_un ret = RT_TYPE_MAX_UN;

	if (first_searched_character) {
		while (RT_TRUE) {
			str_character = str[in_str];
			if (!str_character)
				break;
			if (str_character == first_searched_character) {
				/* We found the first character of searched, in str. */
				/* Let's check the others. */
				in_searched = 1;
				in_str2 = in_str + 1;
				while (RT_TRUE) {
					searched_character = searched[in_searched];
					if (!searched_character) {
						ret = in_str;
						goto end;
					}
					if (str[in_str2] != searched_character)
						break;
					in_searched++;
					in_str2++;
				}
			}
			in_str++;
		}
	}

end:
	return ret;
}

rt_un rt_char_search_with_size(const rt_char *str, rt_un str_size, const rt_char *searched, rt_un searched_size)
{
	rt_char first_searched_character;
	rt_un i, j, k;
	rt_un ret = RT_TYPE_MAX_UN;

	if (searched_size) {
		first_searched_character = searched[0];
		for (i = 0; i < str_size; i++) {
			if (str[i] == first_searched_character) {
				j = i + 1;
				k = 1;
				while (RT_TRUE) {
					if (k >= searched_size) {
						/* We found all characters of searched. */
						ret = i;
						break;
					}

					/* We reached the end of str. */
					if (j >= str_size)
						break;

					if (str[j] != searched[k]) {
						break;
					}

					j++;
					k++;
				}

				/* We found searched, we quit the for loop. */
				if (ret != RT_TYPE_MAX_UN)
					break;
			}
		}
	}

	return ret;
}

rt_un rt_char_search_char(const rt_char *str, rt_char searched)
{
	rt_un ret;

	ret = 0;
	while (str[ret] && str[ret] != searched)
		ret++;
	if (!str[ret]) {
		ret = RT_TYPE_MAX_UN;
	}

	return ret;
}

rt_un rt_char_search_char_with_size(const rt_char *str, rt_un str_size, rt_char searched)
{
	rt_un ret;

	for (ret = 0; ret < str_size; ret++) {
		if (str[ret] == searched)
			break;
	}
	if (ret == str_size)
		ret = RT_TYPE_MAX_UN;
	return ret;
}

rt_un rt_char_count_occurrences(const rt_char *str, const rt_char *searched)
{
	const rt_char *in_str = str;
	rt_un index;
	rt_un searched_size = rt_char_get_size(searched);
	rt_un ret = 0;

	while (RT_TRUE) {
		index = rt_char_search(in_str, searched);
		if (index == RT_TYPE_MAX_UN)
			break;

		ret++;
		in_str = &in_str[index + searched_size];
	}

	return ret;
}

rt_un rt_char_count_occurrences_with_size(const rt_char *str, rt_un str_size, const rt_char *searched, rt_un searched_size)
{
	rt_un index;
	rt_un i = 0;
	rt_un ret = 0;

	while (i <= str_size - searched_size) {
		index = rt_char_search_with_size(&str[i], str_size - i, searched, searched_size);
		if (index == RT_TYPE_MAX_UN)
			break;

		ret++;
		i += index + searched_size;
	}
	return ret;
}

rt_s RT_CDECL rt_char_concat(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size, ...)
{
	va_list args_list;
	rt_s ret;

	va_start(args_list, buffer_size);
	ret = rt_char_vconcat(buffer, buffer_capacity, buffer_size, args_list);
	va_end(args_list);

	return ret;
}

rt_s rt_char_vconcat(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size, va_list args_list)
{
	rt_char *str;
	rt_s ret;

	while (RT_TRUE) {
		str = va_arg(args_list, rt_char*);
		if (str) {
			if (RT_UNLIKELY(!rt_char_append(str, rt_char_get_size(str), buffer, buffer_capacity, buffer_size)))
				goto error;
		} else {
			break;
		}
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_char_replace(const rt_char *str, rt_un str_size,
		     const rt_char *searched, rt_un searched_size,
		     const rt_char *replacement, rt_un replacement_size,
		     rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_un in_str = 0;
	rt_un index;
	rt_un remaining_characters;
	rt_s ret;

	while (RT_TRUE) {
		remaining_characters = str_size - in_str;
		index = rt_char_search_with_size(&str[in_str], remaining_characters, searched, searched_size);

		if (index == RT_TYPE_MAX_UN) {
			/* No more occurrences of the searched string, copy remaining characters. */
			if (remaining_characters) {
				if (RT_UNLIKELY(!rt_char_append(&str[in_str], remaining_characters, buffer, buffer_capacity, buffer_size)))
					goto error;
			}

			/* Job done. */
			break;
		}

		/* Copy from the current position in str to the found index. */
		if (index) {
			if (RT_UNLIKELY(!rt_char_append(&str[in_str], index, buffer, buffer_capacity, buffer_size)))
				goto error;
		}

		/* Copy the replacement. */
		if (replacement_size) {
			if (RT_UNLIKELY(!rt_char_append(replacement, replacement_size, buffer, buffer_capacity, buffer_size)))
				goto error;
		}

		/* Continue reading str. */
		in_str += index + searched_size;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_char_comparison_callback(const void *item1, const void *item2, RT_UNUSED void *context, rt_n *comparison_result)
{
	const rt_uchar *str1 = (rt_uchar*)item1;
	const rt_uchar *str2 = (rt_uchar*)item2;

	while (*str1 && (*str1 == *str2)) {
		str1++;
		str2++;
	}
	*comparison_result = *str1 - *str2;
	return RT_OK;
}

rt_s rt_char_comparison_with_size_callback(const void *item1, rt_un item1_size, const void *item2, rt_un item2_size, RT_UNUSED void *context, rt_n *comparison_result)
{
	const rt_uchar *str1 = (rt_uchar*)item1;
	const rt_uchar *str2 = (rt_uchar*)item2;
	rt_un smaller_size = (item1_size <= item2_size) ? item1_size : item2_size;
	rt_un i;

	for (i = 0; i < smaller_size; i++) {
		if (str1[i] != str2[i])
			break;
	}
	if (i == smaller_size) {
		if (item1_size > smaller_size) {
			*comparison_result = str1[i];
		} else if (item2_size > smaller_size) {
			*comparison_result = -str2[i];
		} else {
			*comparison_result = 0;
		}
	} else {
		*comparison_result = str1[i] - str2[i];
	}
	return RT_OK;
}

rt_un rt_char_hash(const rt_char *data, rt_un data_size)
{
	rt_un i;
	rt_un ret = RT_CHAR_FNV_OFFSET_BASIS;

	for (i = 0; i < data_size; i++) {
		ret ^= data[i];
		ret *= RT_CHAR_FNV_PRIME;
	}

	return ret;
}

rt_s rt_char_hash_callback(const void *data, rt_un data_size, RT_UNUSED void *context, rt_un *hash)
{
	rt_char *chars = (rt_char*)data;
	rt_un local_hash = RT_CHAR_FNV_OFFSET_BASIS;
	rt_un i;

	for (i = 0; i < data_size; i++) {
		local_hash ^= chars[i];
		local_hash *= RT_CHAR_FNV_PRIME;
	}

	*hash = local_hash;

	return RT_OK;
}

rt_s rt_char_split(rt_char *str, const rt_char *delimiters, rt_char **parts, rt_un parts_capacity, rt_un *parts_size)
{
	rt_char character;
	rt_un in_str_index;
	rt_un in_delimiters_index;
	rt_char delimiter;
	rt_b in_delimiters = RT_TRUE;
	rt_b is_delimiter;
	rt_s ret;

	*parts_size = 0;

	in_str_index = 0;
	while (RT_TRUE) {
		character = str[in_str_index];

		if (!character)
			break;

		is_delimiter = RT_FALSE;
		in_delimiters_index = 0;
		while (RT_TRUE) {
			delimiter = delimiters[in_delimiters_index];

			if (!delimiter)
				break;

			if (character == delimiter) {
				is_delimiter = RT_TRUE;
				break;
			}

			in_delimiters_index++;
		}

		if (is_delimiter) {
			str[in_str_index] = 0;
			in_delimiters = RT_TRUE;
		} else {
			if (in_delimiters) {
				if (*parts_size == parts_capacity) {
					rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
					goto error;
				}
				parts[*parts_size] = &str[in_str_index];
				(*parts_size)++;
				in_delimiters = RT_FALSE;
			}
		}
		in_str_index++;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

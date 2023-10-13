#include "layer003/rt_char8.h"

#include "layer001/rt_memory.h"
#include "layer002/rt_error.h"

rt_b rt_char8_equals(const rt_char8 *str1, rt_un str1_size, const rt_char8 *str2, rt_un str2_size)
{
	rt_b ret;

	if (str1_size != str2_size) {
		ret = RT_FALSE;
	} else {
		ret = !RT_MEMORY_COMPARE(str1, str2, str1_size * sizeof(rt_char8));
	}
	return ret;
}

rt_n rt_char8_compare(const rt_char8 *str1, const rt_char8 *str2)
{
	while (*str1 && (*str1 == *str2)) {
		str1++;
		str2++;
	}
	return *(rt_uchar8*)str1 - *(rt_uchar8*)str2;
}

rt_s rt_char8_append(const rt_char8 *suffix, rt_un suffix_size, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size)
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

	RT_MEMORY_COPY(suffix, &buffer[local_buffer_size], copy_size * sizeof(rt_char8));
	local_buffer_size += copy_size;
	buffer[local_buffer_size] = 0;
	*buffer_size = local_buffer_size;

end:
	return ret;
}

rt_s rt_char8_append_char(rt_char8 character, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size)
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

rt_s rt_char8_copy(const rt_char8 *str, rt_un str_size, rt_char8 *buffer, rt_un buffer_capacity)
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

	RT_MEMORY_COPY(str, buffer, copy_size * sizeof(rt_char8));
	buffer[copy_size] = 0;

end:
	return ret;
}

rt_un rt_char8_get_size(const rt_char8 *str)
{
	const rt_char8 *in_string;
	rt_un ret;

	in_string = str;
	while (*in_string++);

	ret = (rt_un)(in_string - str - 1);

	return ret;
}

rt_s rt_char8_append_un(rt_un value, rt_un base, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_un previous_value;
	rt_char8 tmp_char;
	rt_un i, j;
	rt_s ret;

	if (RT_UNLIKELY(base < 2 || base > 36))
		goto error;

	i = *buffer_size;
	j = *buffer_size;
	while (i < buffer_capacity - 1) {
		previous_value = value;
		value = value / base;
		buffer[i] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[previous_value - value * base];
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

rt_s rt_char8_append_n(rt_n value, rt_un base, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_n local_value;
	rt_n previous_value;
	rt_char8 tmp_char;
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
		buffer[i] = "ZYXWVUTSRQPONMLKJIHGFEDCBA9876543210123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[35 + (previous_value - local_value * base)];
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

rt_un rt_char8_fast_lower(rt_char8 *str)
{
	rt_char8 current_char;
	rt_un ret;

	ret = 0;
	while (str[ret]) {
		current_char = str[ret];
		if (current_char >= 'A' && current_char <= 'Z') {
			str[ret] = current_char + 32;
		}
		ret++;
	}
	return ret;
}

rt_un rt_char8_fast_upper(rt_char8 *str)
{
	rt_char8 current_char;
	rt_un ret;

	ret = 0;
	while (str[ret]) {
		current_char = str[ret];
		if (current_char >= 'a' && current_char <= 'z') {
			str[ret] = current_char - 32;
		}
		ret++;
	}
	return ret;
}

rt_s rt_char8_convert_to_un(const rt_char8 *str, rt_un *result)
{
	rt_char8 character;
	rt_un local_result = 0;
	rt_un i = 0;
	rt_s ret;

	while (RT_TRUE) {
		character = str[i];
		if (!character)
			break;

		if (RT_UNLIKELY((character < '0') || (character > '9'))) {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		} else {
			local_result = local_result * 10 + character - '0';
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

rt_s rt_char8_convert_to_un_with_size(const rt_char8 *str, rt_un str_size, rt_un *result)
{
	rt_char8 character;
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

		if (RT_UNLIKELY((character < '0') || (character > '9'))) {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		} else {
			local_result = local_result * 10 + character - '0';
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

rt_s rt_char8_convert_to_n(const rt_char8 *str, rt_n *result)
{
	rt_b negative;
	rt_char8 character;
	rt_n local_result = 0;
	rt_un i = 0;
	rt_s ret;

	if (str[0] == '-') {
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

		if (RT_UNLIKELY((character < '0') || (character > '9'))) {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		} else {
			local_result = local_result * 10 + character - '0';
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

rt_s rt_char8_convert_to_n_with_size(const rt_char8 *str, rt_un str_size, rt_n *result)
{
	rt_b negative;
	rt_char8 character;
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

		if (RT_UNLIKELY((character < '0') || (character > '9'))) {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		} else {
			local_result = local_result * 10 + character - '0';
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

rt_s rt_char8_convert_hex_to_un(const rt_char8 *str, rt_un *result)
{
	rt_char8 character;
	rt_un local_result = 0;
	rt_uchar8 byte;
	rt_un i = 0;
	rt_s ret;

	while (RT_TRUE) {
		character = str[i];
		if (!character)
			break;

		if (character >= '0' && character <= '9') {
			byte = character - '0';
		} else if (character >= 'a' && character <= 'f') {
			byte = character - 'a' + 10;
		} else if (character >= 'A' && character <= 'F') {
			byte = character - 'A' + 10;
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

rt_s rt_char8_convert_hex_to_un_with_size(const rt_char8 *str, rt_un str_size, rt_un *result)
{
	rt_char8 character;
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

		if (character >= '0' && character <= '9') {
			byte = character - '0';
		} else if (character >= 'a' && character <= 'f') {
			byte = character - 'a' + 10;
		} else if (character >= 'A' && character <= 'F') {
			byte = character - 'A' + 10;
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

void rt_char8_trim(rt_b left, rt_b right, rt_char8 *buffer, rt_un *buffer_size)
{
	rt_un local_buffer_size = *buffer_size;
	rt_un i;

	if (right) {
		while (local_buffer_size && buffer[local_buffer_size - 1] <= ' ')
			local_buffer_size--;

		buffer[local_buffer_size] = 0;
	}

	if (left) {
		for (i = 0; i < local_buffer_size; i++) {
			if (buffer[i] > ' ')
				break;
		}
		if (i) {
			local_buffer_size -= i;
			if (local_buffer_size) {
				RT_MEMORY_COPY(&buffer[i], buffer, local_buffer_size * sizeof(rt_char8));
			}
			buffer[local_buffer_size] = 0;
		}
	}

	*buffer_size = local_buffer_size;
}

rt_s rt_char8_left_pad(const rt_char8 *input, rt_un input_size, rt_char8 character, rt_un size, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size)
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
				RT_MEMORY_MOVE(input, &buffer[padding_size], input_size * sizeof(rt_char8));
			} else {
				RT_MEMORY_COPY(input, &buffer[padding_size], input_size * sizeof(rt_char8));
			}
		}

		/* Add padding at the left of the input. */
		RT_MEMORY_SET(buffer, character, padding_size);

		/* Add zero terminating character. */
		buffer[size] = 0;

		*buffer_size = size;

	} else {
		/* If the result is the same as the input, then there is nothing to do. */
		if (input != buffer) {
			/* The input is already long enough. We copy it to the result. */
			if (RT_UNLIKELY(!rt_char8_copy(input, input_size, buffer, buffer_capacity)))
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

rt_s rt_char8_right_pad(rt_char8 character, rt_un size, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size)
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

		RT_MEMORY_SET(&buffer[local_buffer_size], character, size - local_buffer_size);

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

rt_s rt_char8_ends_with(const rt_char8 *str, rt_un str_size, const rt_char8 *searched, rt_un searched_size)
{
	const rt_char8 *str_end;
	rt_b ret;

	if (str_size >= searched_size) {
		if (searched_size) {
			str_end = &str[str_size - searched_size];
			ret = !RT_MEMORY_COMPARE(str_end, searched, searched_size * sizeof(rt_char8));
		} else {
			ret = RT_TRUE;
		}
	} else {
		ret = RT_FALSE;
	}
	return ret;
}


rt_un rt_char8_search(const rt_char8 *str, const rt_char8 *searched)
{
	rt_char8 first_searched_character = searched[0];
	rt_un in_str = 0;
	rt_char8 str_character;
	rt_un in_searched;
	rt_un in_str2;
	rt_char8 searched_character;
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

rt_un rt_char8_search_char(const rt_char8 *str, rt_char8 searched)
{
	rt_un result;

	result = 0;
	while (str[result] && str[result] != searched)
		result++;
	if (!str[result]) {
		result = RT_TYPE_MAX_UN;
	}

	return result;
}

rt_un rt_char8_count_occurrences(const rt_char8 *str, const rt_char8 *searched)
{
	const rt_char8 *in_str = str;
	rt_un index;
	rt_un searched_size = rt_char8_get_size(searched);
	rt_un res = 0;

	while (RT_TRUE) {
		index = rt_char8_search(in_str, searched);
		if (index == RT_TYPE_MAX_UN)
			break;

		res++;
		in_str = &in_str[index + searched_size];
	}

	return res;
}

rt_s RT_CDECL rt_char8_concat(rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size, ...)
{
	va_list args_list;
	rt_s ret;

	va_start(args_list, buffer_size);
	ret = rt_char8_vconcat(buffer, buffer_capacity, buffer_size, args_list);
	va_end(args_list);

	return ret;
}

rt_s rt_char8_vconcat(rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size, va_list args_list)
{
	rt_char8 *str;
	rt_s ret;

	while (RT_TRUE) {
		str = va_arg(args_list, rt_char8*);
		if (str) {
			if (RT_UNLIKELY(!rt_char8_append(str, rt_char8_get_size(str), buffer, buffer_capacity, buffer_size)))
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

rt_s rt_char8_replace(const rt_char8 *str, rt_un str_size,
		     const rt_char8 *searched, rt_un searched_size,
		     const rt_char8 *replacement, rt_un replacement_size,
		     rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_un in_str = 0;
	rt_un index;
	rt_un remaining_characters;
	rt_s ret;

	while (RT_TRUE) {
		index = rt_char8_search(&str[in_str], searched);

		if (index == RT_TYPE_MAX_UN) {
			/* No more occurrences of the searched string, copy remaining characters. */
			remaining_characters = str_size - in_str;
			if (remaining_characters) {
				if (RT_UNLIKELY(!rt_char8_append(&str[in_str], remaining_characters, buffer, buffer_capacity, buffer_size)))
					goto error;
			}

			/* Job done. */
			break;
		}

		/* Copy from the current position in str to the found index. */
		if (index) {
			if (RT_UNLIKELY(!rt_char8_append(&str[in_str], index, buffer, buffer_capacity, buffer_size)))
				goto error;
		}

		/* Copy the replacement. */
		if (replacement_size) {
			if (RT_UNLIKELY(!rt_char8_append(replacement, replacement_size, buffer, buffer_capacity, buffer_size)))
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

rt_s rt_char8_comparison_callback(const void *item1, const void *item2, RT_UNUSED void *context, rt_n *comparison_result)
{
	const rt_uchar8 *str1 = (rt_uchar8*)item1;
	const rt_uchar8 *str2 = (rt_uchar8*)item2;

	while (*str1 && (*str1 == *str2)) {
		str1++;
		str2++;
	}
	*comparison_result = *str1 - *str2;
	return RT_OK;
}

rt_s rt_char8_comparison_with_size_callback(const void *item1, rt_un item1_size, const void *item2, rt_un item2_size, RT_UNUSED void *context, rt_n *comparison_result)
{
	const rt_uchar8 *str1 = (rt_uchar8*)item1;
	const rt_uchar8 *str2 = (rt_uchar8*)item2;
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

rt_un rt_char8_hash(const rt_char8 *data, rt_un data_size)
{
	rt_un i;
	rt_un ret = 14695981039346656037ull;

	for (i = 0; i < data_size; i++) {
		ret ^= data[i];
		ret *= 1099511628211ull;
	}

	return ret;
}

rt_s rt_char8_hash_callback(const void *data, rt_un data_size, RT_UNUSED void *context, rt_un *hash)
{
	rt_char8 *chars = (rt_char8*)data;
	rt_un local_hash = 14695981039346656037ull;
	rt_un i;

	for (i = 0; i < data_size; i++) {
		local_hash ^= chars[i];
		local_hash *= 1099511628211ull;
	}

	*hash = local_hash;

	return RT_OK;
}

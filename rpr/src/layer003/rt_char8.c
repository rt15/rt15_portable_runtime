#include "layer003/rt_char8.h"

#include "layer001/rt_memory.h"
#include "layer002/rt_error.h"

rt_b rt_char8_equals(const rt_char8 *string1, rt_un string1_size, const rt_char8 *string2, rt_un string2_size)
{
	rt_b ret;

	if (string1_size != string2_size) {
		ret = RT_FALSE;
	} else {
		ret = !RT_MEMORY_COMPARE(string1, string2, string1_size * sizeof(rt_char8));
	}
	return ret;
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
			/* Add a final null terminating character just in case. */
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
			/* Make sure we put a null terminating character. */
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
			/* Add a final null terminating character just in case. */
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

rt_s rt_char8_append_n(rt_n value, rt_un base, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_n local_value;
	rt_n previous_value;
	rt_char8 tmp_char;
	rt_un i, j;
	rt_s ret;

	if (base < 2 || base > 36)
		goto error;

	local_value = value;
	i = *buffer_size;
	j = *buffer_size;
	while (i < buffer_capacity - 1) {
		previous_value = local_value;
		local_value = local_value / (rt_n)base;
		buffer[i] = "ZYXWVUTSRQPONMLKJIHGFEDCBA9876543210123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" [35 + (previous_value - local_value * base)];
		i++;
		if (!local_value)
			break;
	}
	if (local_value)
		goto error;

	if (value < 0) {
		if (i >= buffer_capacity - 1)
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
	/* Add a null terminating character if possible. */
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
		if (current_char >= _R('A') && current_char <= _R('Z')) {
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
		if (current_char >= _R('a') && current_char <= _R('z')) {
			str[ret] = current_char - 32;
		}
		ret++;
	}
	return ret;
}

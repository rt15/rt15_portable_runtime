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
			goto out;
		}
	}

	RT_MEMORY_COPY(suffix, &buffer[local_buffer_size], copy_size * sizeof(rt_char8));
	local_buffer_size += copy_size;
	buffer[local_buffer_size] = 0;
	*buffer_size = local_buffer_size;

out:
	return ret;
}

rt_s rt_char8_copy(const rt_char8 *string, rt_un string_size, rt_char8 *buffer, rt_un buffer_capacity)
{
	rt_un copy_size;
	rt_s ret;

	if (string_size + 1 <= buffer_capacity) {
		ret = RT_OK;
		copy_size = string_size;

	} else {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		ret = RT_FAILED;
		if (buffer_capacity > 1) {
			copy_size = buffer_capacity - 1;
		} else {
			/* Add a final null terminating character just in case. */
			if (buffer_capacity > 0)
				buffer[0] = 0;
			goto out;
		}
	}

	RT_MEMORY_COPY(string, buffer, copy_size * sizeof(rt_char8));
	buffer[copy_size] = 0;

out:
	return ret;
}

rt_un rt_char8_get_size(const rt_char8 *string)
{
	const rt_char8 *in_string;
	rt_un ret;

	in_string = string;
	while (*in_string++);

	ret = (rt_un)(in_string - string - 1);

	return ret;
}

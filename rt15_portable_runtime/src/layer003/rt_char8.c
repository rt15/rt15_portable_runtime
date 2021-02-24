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
	rt_s ret;

	if (*buffer_size + suffix_size + sizeof(rt_char8) > buffer_capacity) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto error;
	}

	RT_MEMORY_COPY(suffix, &buffer[*buffer_size], suffix_size * sizeof(rt_char8));
	(*buffer_size) += suffix_size;
	buffer[*buffer_size] = 0;

	ret = RT_OK;

free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
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

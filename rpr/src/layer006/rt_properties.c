#include "layer006/rt_properties.h"

#include "layer002/rt_error.h"
#include "layer003/rt_char.h"
#include "layer004/rt_small_file.h"

enum rt_properties_parsing_state {
	RT_PROPERTIES_PARSING_STATE_EOL,
	RT_PROPERTIES_PARSING_STATE_KEY,
	RT_PROPERTIES_PARSING_STATE_BEFORE_SEPARATOR,
	RT_PROPERTIES_PARSING_STATE_AFTER_SEPARATOR,
	RT_PROPERTIES_PARSING_STATE_VALUE
};

rt_s rt_properties_create(struct rt_hash_table_entry **properties, const rt_char *file_path, enum rt_encoding encoding, rt_un initial_capacity, struct rt_heap *heap)
{
	void *file_heap_buffer = RT_NULL;
	rt_un file_heap_buffer_capacity = 0;
	rt_char8 *file_buffer;
	rt_un file_buffer_size;

	void *heap_buffer = RT_NULL;
	rt_un heap_buffer_capacity = 0;
	rt_char *buffer;
	rt_un buffer_size;

	struct rt_properties_header *header;

	rt_s ret;

	if (!rt_small_file_read(file_path, RT_NULL, 0, &file_heap_buffer, &file_heap_buffer_capacity, &file_buffer, &file_buffer_size, heap))
		goto error;

	if (!rt_encoding_decode(file_buffer, file_buffer_size, encoding, RT_NULL, 0, &heap_buffer, &heap_buffer_capacity, &buffer, &buffer_size, heap))
		goto error;

	if (!rt_properties_create_from_str(properties, buffer, initial_capacity, heap))
		goto error;

	header = RT_PROPERTIES_GET_HEADER(*properties);
	header->file_path = file_path;
	header->buffer = buffer;

	ret = RT_OK;
free:
	/* Free the file heap buffer, but notify error in case of failure. */
	if (!heap->free(heap, &file_heap_buffer) && ret)
		goto error;
	return ret;

error:
	/* In case of error, free the heap buffer ignoring errors. */
	heap->free(heap, &heap_buffer);
	ret = RT_FAILED;
	goto free;
}

/**
 * Read the entries from <tt>str</tt> and put keys and values into <tt>properties</tt>.<br>
 * At this point, <tt>properties</tt> must be a valid hash table.<br>
 * <tt>str</tt> is modified by this function.
 */
static rt_s rt_properties_create_from_str_do(struct rt_hash_table_entry **properties, rt_char *str)
{
	enum rt_properties_parsing_state parsing_state = RT_PROPERTIES_PARSING_STATE_EOL;
	rt_un current_index = 0;
	rt_char current_char;
	rt_char *key;
	rt_char *value;
	rt_s ret;

	while (RT_TRUE) {
		current_char = str[current_index];
		if (!current_char)
			break;

		switch (parsing_state) {
		case RT_PROPERTIES_PARSING_STATE_EOL:
			/* Manage end of lines, either at the beginning of the file or after a value. */
			switch (current_char) {
			case _R('\r'):
			case _R('\n'):
			case _R(' '):
			case _R('\t'):
			case _R('\f'):
				/* Skip all blanks until something interesting is found. */
				str[current_index] = 0;
				break;
			case _R(':'):
			case _R('='):
				/* Empty key. */
				str[current_index] = 0;
				key = &str[current_index];
				parsing_state = RT_PROPERTIES_PARSING_STATE_AFTER_SEPARATOR;
				break;
			case _R('!'):
			case _R('#'):
				/* Comment. */
				current_index++;
				while (RT_TRUE) {
					current_char = str[current_index];
					/* We stop ignoring at the first end of line or at the end of the string. */
					if (current_char == _R('\n') || current_char == _R('\r') || !current_char) {
						/* The current_index is incremented at the end of the main parsing loop. */
						current_index--;
						break;
					}
					current_index++;
				}
				break;
			default:
				/* We found the first character of a key. */
				key = &str[current_index];
				parsing_state = RT_PROPERTIES_PARSING_STATE_KEY;
			}
			break;
		case RT_PROPERTIES_PARSING_STATE_KEY:
			/* Manage a key. */
			switch (current_char) {
			case _R('='):
			case _R(':'):
				str[current_index] = 0;
				parsing_state = RT_PROPERTIES_PARSING_STATE_AFTER_SEPARATOR;
				break;
			case _R(' '):
			case _R('\t'):
			case _R('\f'):
				/* We reached the end of the key. */
				str[current_index] = 0;
				parsing_state = RT_PROPERTIES_PARSING_STATE_BEFORE_SEPARATOR;
				break;
			case _R('\r'):
			case _R('\n'):
				/* Key without value. */
				str[current_index] = 0;
				value = &str[current_index];
				if (!rt_hash_table_set(properties, key, rt_char_get_size(key), value, RT_NULL))
					goto error;
				parsing_state = RT_PROPERTIES_PARSING_STATE_EOL;
				break;
			}
			break;
		case RT_PROPERTIES_PARSING_STATE_BEFORE_SEPARATOR:
			/* Manage spaces before the key/value separator. */
			switch (current_char) {
			case _R('='):
			case _R(':'):
				str[current_index] = 0;
				parsing_state = RT_PROPERTIES_PARSING_STATE_AFTER_SEPARATOR;
				break;
			case _R(' '):
			case _R('\t'):
			case _R('\f'):
				/* Continue ignoring spaces after key. */
				str[current_index] = 0;
				break;
			case _R('\r'):
			case _R('\n'):
				/* Key without value. */
				str[current_index] = 0;
				value = &str[current_index];
				if (!rt_hash_table_set(properties, key, rt_char_get_size(key), value, RT_NULL))
					goto error;
				parsing_state = RT_PROPERTIES_PARSING_STATE_EOL;
				break;
			default:
				/* Beginning of the value. */
				value = &str[current_index];
				parsing_state = RT_PROPERTIES_PARSING_STATE_VALUE;
			}
			break;
		case RT_PROPERTIES_PARSING_STATE_AFTER_SEPARATOR:
			/* Manage spaces after the key/value separator. */
			switch (current_char) {
			case _R(' '):
			case _R('\t'):
			case _R('\f'):
				/* Skip all spaces after '='. */
				str[current_index] = 0;
				break;
			case _R('\r'):
			case _R('\n'):
				/* Key without value. */
				str[current_index] = 0;
				value = &str[current_index];
				if (!rt_hash_table_set(properties, key, rt_char_get_size(key), value, RT_NULL))
					goto error;
				parsing_state = RT_PROPERTIES_PARSING_STATE_EOL;
				break;
			default:
				/* Beginning of the value. */
				value = &str[current_index];
				parsing_state = RT_PROPERTIES_PARSING_STATE_VALUE;
			}
			break;
		case RT_PROPERTIES_PARSING_STATE_VALUE:
			/* Manage value. */
			switch (current_char) {
			case _R('\r'):
			case _R('\n'):
				/* End of value. */
				str[current_index] = 0;
				if (!rt_hash_table_set(properties, key, rt_char_get_size(key), value, RT_NULL))
					goto error;
				parsing_state = RT_PROPERTIES_PARSING_STATE_EOL;
				break;
			}
			break;
		default:
			rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
			goto error;
		}
		current_index++;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_properties_create_from_str(struct rt_hash_table_entry **properties, rt_char *str, rt_un initial_capacity, struct rt_heap *heap)
{
	struct rt_properties_header *header;
	rt_s ret;

	*properties = RT_NULL;

	if (!rt_hash_table_create(properties, &rt_char_hash_callback, &rt_char_comparison_with_size_callback, RT_NULL, initial_capacity, sizeof(struct rt_properties_header), heap))
		goto error;

	header = RT_PROPERTIES_GET_HEADER(*properties);
	header->file_path = RT_NULL;
	header->buffer = RT_NULL;

	if (!rt_properties_create_from_str_do(properties, str))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	/* In case of error, free the hash table ignoring errors. */
	rt_hash_table_free(properties);

	ret = RT_FAILED;
	goto free;
}

rt_s rt_properties_free(struct rt_hash_table_entry **properties)
{
	struct rt_properties_header *header;
	struct rt_heap *heap;
	rt_s ret = RT_OK;

	if (*properties) {
		header = RT_PROPERTIES_GET_HEADER(*properties);

		heap = header->hash_table_header.array_header.heap;
		if (!heap->free(heap, (void**)&header->buffer))
			ret = RT_FAILED;
		if (!rt_hash_table_free(properties))
			ret = RT_FAILED;
	}

	return ret;
}

#include "layer007/rt_properties.h"

#include "layer001/rt_memory.h"
#include "layer002/rt_buffered_output_stream.h"
#include "layer002/rt_error.h"
#include "layer003/rt_char.h"
#include "layer003/rt_char8.h"
#include "layer003/rt_file.h"
#include "layer004/rt_small_file.h"
#include "layer005/rt_unicode_code_point.h"
#include "layer006/rt_properties_parser.h"

struct rt_properties_create_context {
	struct rt_hash_table_entry *hash_table;
	rt_char *buffer;
	rt_un buffer_size;
	rt_un buffer_capacity;
	rt_char *current_key;
};

struct rt_properties_merge_context {
	struct rt_hash_table_entry *properties_hash_table;
	struct rt_hash_table_entry *key_exists_hash_table;
	struct rt_output_stream *output_stream;
	enum rt_encoding encoding;
	enum rt_eol eol;
	rt_b delete_missing_keys;
	rt_b skip_next_value;
	rt_b was_eol;
	struct rt_heap *heap;
};

static rt_s rt_properties_create_callback(enum rt_properties_parser_part_type part_type, const rt_char *str, rt_un index, rt_un size, void *context)
{
	struct rt_properties_create_context *properties_create_context = (struct rt_properties_create_context*)context;
	rt_char *current_key;
	rt_un current_key_size;
	rt_char *current_value;
	rt_s ret = RT_FAILED;

	if (part_type == RT_PROPERTIES_PARSER_PART_TYPE_KEY) {
		properties_create_context->current_key = &properties_create_context->buffer[properties_create_context->buffer_size];
		if (RT_UNLIKELY(!rt_properties_parser_parse_key(&str[index], size, properties_create_context->buffer, properties_create_context->buffer_capacity, &properties_create_context->buffer_size)))
			goto end;
		if (RT_UNLIKELY(!rt_char_append_char(_R('\0'), properties_create_context->buffer, properties_create_context->buffer_capacity, &properties_create_context->buffer_size)))
			goto end;
	}
	else if (part_type == RT_PROPERTIES_PARSER_PART_TYPE_VALUE) {
		current_key = properties_create_context->current_key;
		current_value = &properties_create_context->buffer[properties_create_context->buffer_size];
		if (RT_UNLIKELY(!rt_properties_parser_parse_value(&str[index], size, properties_create_context->buffer, properties_create_context->buffer_capacity, &properties_create_context->buffer_size)))
			goto end;
		if (RT_UNLIKELY(!rt_char_append_char(_R('\0'), properties_create_context->buffer, properties_create_context->buffer_capacity, &properties_create_context->buffer_size)))
			goto end;

		current_key_size = rt_char_get_size(current_key);

		if (RT_UNLIKELY(!rt_hash_table_set(&properties_create_context->hash_table, current_key, current_key_size, current_value, RT_NULL)))
			goto end;
	}
	ret = RT_OK;

end:
	return ret;
}

rt_s rt_properties_create(struct rt_properties *properties, const rt_char *file_path, enum rt_encoding encoding, rt_un initial_capacity, struct rt_heap *heap)
{
	void *file_heap_buffer = RT_NULL;
	rt_un file_heap_buffer_capacity = 0;
	rt_char8 *file_output;
	rt_un file_output_size;
	void *heap_buffer = RT_NULL;
	rt_un heap_buffer_capacity = 0;
	rt_char* output;
	rt_un output_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_small_file_read(file_path, RT_NULL, 0, &file_heap_buffer, &file_heap_buffer_capacity, &file_output, &file_output_size, heap))) goto end;

	if (RT_UNLIKELY(!rt_encoding_decode(file_output, file_output_size, encoding, RT_NULL, 0, &heap_buffer, &heap_buffer_capacity, &output, &output_size, heap))) goto end;

	if (RT_UNLIKELY(!rt_properties_create_from_str(properties, output, output_size, initial_capacity, heap))) goto end;

	ret = RT_OK;
end:
	if (heap_buffer) {
		if (RT_UNLIKELY(!heap->free(heap, &heap_buffer)))
			ret = RT_FAILED;
	}

	if (file_heap_buffer) {
		if (RT_UNLIKELY(!heap->free(heap, &file_heap_buffer)))
			ret = RT_FAILED;
	}

	return ret;
}

rt_s rt_properties_create_from_str(struct rt_properties *properties, const rt_char *str, rt_un str_size, rt_un initial_capacity, struct rt_heap *heap)
{
	struct rt_properties_create_context context;
	rt_s ret = RT_FAILED;

	context.hash_table = RT_NULL;
	context.buffer = RT_NULL;

	if (RT_UNLIKELY(!heap->alloc(heap, (void**)&context.buffer, str_size * sizeof(rt_char))))
		goto end;
	context.buffer[0] = 0;
	context.buffer_size = 0;
	context.buffer_capacity = str_size;
	context.current_key = RT_NULL;

	if (RT_UNLIKELY(!rt_hash_table_create(&context.hash_table, &rt_char_hash_callback, &rt_char_comparison_with_size_callback, RT_NULL, initial_capacity, 0, heap)))
		goto end;

	if (RT_UNLIKELY(!rt_properties_parser_parse(str, str_size, &rt_properties_create_callback, &context)))
		goto end;

	properties->hash_table = context.hash_table;
	properties->buffer = context.buffer;
	properties->heap = heap;

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!ret)) {
		rt_hash_table_free(&context.hash_table);
		heap->free(heap, (void**)&context.buffer);
	}

	return ret;
}

rt_s rt_properties_create_from_hash_table(struct rt_properties *properties, struct rt_hash_table_entry *hash_table, struct rt_heap *heap)
{
	properties->hash_table = hash_table;
	properties->buffer = RT_NULL;
	properties->heap = heap;

	return RT_OK;
}

/**
 * Format the key then write it to the stream.
 */
static rt_s rt_properties_write_key(const rt_char *key, rt_un key_size, enum rt_encoding encoding, struct rt_output_stream *output_stream, struct rt_heap *heap)
{
	rt_b encode_non_ascii = encoding != RT_ENCODING_UTF_8;
	rt_char buffer[RT_CHAR_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret = RT_FAILED;

	buffer_size = 0;
	if (RT_UNLIKELY(!rt_properties_parser_format_key(key, key_size, encode_non_ascii, buffer, RT_CHAR_BIG_STRING_SIZE, &buffer_size)))
		goto end;

	if (RT_UNLIKELY(!rt_encoding_write(buffer, buffer_size, encoding, output_stream, heap)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

/**
 * Format the value then write it to the stream.
 */
static rt_s rt_properties_write_value(const rt_char *value, rt_un value_size, enum rt_encoding encoding, struct rt_output_stream *output_stream, struct rt_heap *heap)
{
	rt_b encode_non_ascii = encoding != RT_ENCODING_UTF_8;
	rt_char buffer[RT_CHAR_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret = RT_FAILED;

	buffer_size = 0;
	if (RT_UNLIKELY(!rt_properties_parser_format_value(value, value_size, encode_non_ascii, buffer, RT_CHAR_BIG_STRING_SIZE, &buffer_size)))
		goto end;

	if (RT_UNLIKELY(!rt_encoding_write(buffer, buffer_size, encoding, output_stream, heap)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s rt_properties_merge_callback(enum rt_properties_parser_part_type part_type, const rt_char *str, rt_un index, rt_un size, void *context)
{
	struct rt_properties_merge_context *properties_merge_context = (struct rt_properties_merge_context*)context;
	struct rt_hash_table_entry *properties_hash_table = properties_merge_context->properties_hash_table;
	struct rt_hash_table_entry *key_exists_hash_table = properties_merge_context->key_exists_hash_table;
	struct rt_output_stream *output_stream = properties_merge_context->output_stream;
	enum rt_encoding encoding = properties_merge_context->encoding;
	enum rt_eol eol = properties_merge_context->eol;
	rt_b delete_missing_keys = properties_merge_context->delete_missing_keys;
	struct rt_heap *heap = properties_merge_context->heap;
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_char *properties_value;
	rt_uchar8 *key_exists;
	rt_un eol_count;
	rt_un i;
	rt_s ret = RT_FAILED;

	switch (part_type) {
	case RT_PROPERTIES_PARSER_PART_TYPE_KEY:
		buffer_size = 0;
		if (RT_UNLIKELY(!rt_properties_parser_parse_key(&str[index], size, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
			goto end;

		if (RT_UNLIKELY(!rt_hash_table_get(properties_hash_table, buffer, buffer_size, (void**)&properties_value)))
			goto end;
		if (properties_value) {

			/* Write the key and the separator. */
			if (RT_UNLIKELY(!rt_encoding_write(&str[index], size, encoding, output_stream, heap))) goto end;
			if (RT_UNLIKELY(!rt_encoding_write(_R("="), 1, encoding, output_stream, heap))) goto end;

			/* Notify that the key already exists in both the input file and the properties. */
			if (RT_UNLIKELY(!rt_hash_table_get(key_exists_hash_table, buffer, buffer_size, (void**)&key_exists)))
				goto end;
			*key_exists = RT_TRUE;

			/* Write the value from the properties. */
			if (RT_UNLIKELY(!rt_properties_write_value(properties_value, rt_char_get_size(properties_value), encoding, output_stream, heap)))
				goto end;

			if (RT_UNLIKELY(!rt_encoding_write_eol(eol, encoding, output_stream)))
				goto end;

			/* We have used the value from the properties so we skip the next value from the file. */
			properties_merge_context->skip_next_value = RT_TRUE;

			properties_merge_context->was_eol = RT_FALSE;

		} else if (delete_missing_keys) {

			/* The key from the file is not part of the properties so we discard it. */
			properties_merge_context->skip_next_value = RT_TRUE;

		} else {

			/* Write the key and the separator. */
			if (RT_UNLIKELY(!rt_encoding_write(&str[index], size, encoding, output_stream, heap))) goto end;
			if (RT_UNLIKELY(!rt_encoding_write(_R("="), 1, encoding, output_stream, heap))) goto end;

			/* Recopy the value from the file. */
			properties_merge_context->skip_next_value = RT_FALSE;

			properties_merge_context->was_eol = RT_FALSE;

		}
		break;
	case RT_PROPERTIES_PARSER_PART_TYPE_SEPARATOR:
		break;
	case RT_PROPERTIES_PARSER_PART_TYPE_VALUE:
		if (!properties_merge_context->skip_next_value) {
			/* Write the value and a end of line. */
			if (RT_UNLIKELY(!rt_encoding_write(&str[index], size, encoding, output_stream, heap))) goto end;
			if (RT_UNLIKELY(!rt_encoding_write_eol(eol, encoding, output_stream))) goto end;
			properties_merge_context->was_eol = RT_FALSE;
		}
		break;
	case RT_PROPERTIES_PARSER_PART_TYPE_BLANKS:
		if (!properties_merge_context->was_eol) {
			eol_count = 0;
			for (i = 0; i < size; i++) {
				if (str[i + index] == _R('\n'))
					eol_count++;
			}
			if (eol_count >= 2) {
				if (RT_UNLIKELY(!rt_encoding_write_eol(eol, encoding, output_stream)))
					goto end;
				properties_merge_context->was_eol = RT_TRUE;
			}
		}
		break;
	case RT_PROPERTIES_PARSER_PART_TYPE_COMMENT:
		if (RT_UNLIKELY(!rt_encoding_write(&str[index], size, encoding, output_stream, heap))) goto end;
		if (RT_UNLIKELY(!rt_encoding_write_eol(eol, encoding, output_stream))) goto end;
		properties_merge_context->was_eol = RT_FALSE;
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s rt_properties_merge_into_file_with_buffer(struct rt_properties *properties, const rt_char *input_buffer, rt_un input_buffer_size, struct rt_output_stream *output_stream, enum rt_encoding encoding, enum rt_eol eol, rt_b delete_missing_keys)
{
	struct rt_hash_table_header *hash_table_header = RT_HASH_TABLE_GET_HEADER(properties->hash_table);
	struct rt_heap *heap = properties->heap;
	rt_uchar8 key_exists_array_buffer[RT_CHAR8_HALF_BIG_STRING_SIZE];
	rt_uchar8 *key_exists_array;
	void *key_exists_array_heap_buffer = RT_NULL;
	rt_un key_exists_array_heap_buffer_capacity = 0;
	struct rt_hash_table_entry *key_exists_hash_table = RT_NULL;
	rt_un index_in_hash_table;
	struct rt_hash_table_entry *hash_table_entry;
	const void *key;
	rt_un key_size;
	rt_un index_in_key_exists_array = 0;
	struct rt_properties_merge_context context;
	rt_uchar8 *key_exists;
	rt_s ret = RT_FAILED;

	/* Allocate an array of booleans to store the values of key exists hash-table. */
	if (RT_UNLIKELY(!rt_heap_alloc_if_needed(key_exists_array_buffer, RT_CHAR8_HALF_BIG_STRING_SIZE, &key_exists_array_heap_buffer, &key_exists_array_heap_buffer_capacity, (void**)&key_exists_array, hash_table_header->size, heap)))
		goto end;
	RT_MEMORY_ZERO(key_exists_array, hash_table_header->size);

	/* Allocate a key exists hash-table with the same callbacks, context and capacity as the initial one. */
	if (RT_UNLIKELY(!rt_hash_table_create(&key_exists_hash_table, hash_table_header->hash_callback, hash_table_header->comparison_callback, hash_table_header->context, hash_table_header->array_header.size, 0, heap)))
		goto end;

	/* Loop through the properties hash-table items to fill the key exists hash-table, initializing it as if none exist. */
	for (index_in_hash_table = 0; index_in_hash_table < hash_table_header->array_header.size; index_in_hash_table++) {
		hash_table_entry = &properties->hash_table[index_in_hash_table];
		key = hash_table_entry->key;
		if (key) {
			key_size = hash_table_entry->key_size;
			/* Add the key to the key exists hash-table. */
			if (RT_UNLIKELY(!rt_hash_table_set(&key_exists_hash_table, key, key_size, &key_exists_array[index_in_key_exists_array], RT_NULL)))
				goto end;
			index_in_key_exists_array++;
		}
	}

	context.properties_hash_table = properties->hash_table;
	context.key_exists_hash_table = key_exists_hash_table;
	context.output_stream = output_stream;
	context.encoding = encoding;
	context.eol = eol;
	context.delete_missing_keys = delete_missing_keys;
	context.skip_next_value = RT_FALSE;
	context.was_eol = RT_FALSE;
	context.heap = heap;

	if (RT_UNLIKELY(!rt_properties_parser_parse(input_buffer, input_buffer_size, &rt_properties_merge_callback, &context)))
		goto end;

	/* Write the keys/values from the properties but absent from the input file. */
	for (index_in_hash_table = 0; index_in_hash_table < hash_table_header->array_header.size; index_in_hash_table++) {
		hash_table_entry = &properties->hash_table[index_in_hash_table];
		key = hash_table_entry->key;
		if (key) {
			key_size = hash_table_entry->key_size;
			if (RT_UNLIKELY(!rt_hash_table_get(key_exists_hash_table, key, key_size, (void**)&key_exists)))
				goto end;
			if (!*key_exists) {
				/* Write the key and the separator. */
				if (RT_UNLIKELY(!rt_properties_write_key(key, key_size, encoding, output_stream, heap))) goto end;
				if (RT_UNLIKELY(!rt_encoding_write(_R("="), 1, encoding, output_stream, heap))) goto end;

				/* Write the value from the properties. */
				if (RT_UNLIKELY(!rt_properties_write_value(hash_table_entry->value, rt_char_get_size(hash_table_entry->value), encoding, output_stream, heap)))
					goto end;

				if (RT_UNLIKELY(!rt_encoding_write_eol(eol, encoding, output_stream)))
					goto end;
			}
		}
	}

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!rt_hash_table_free(&key_exists_hash_table)))
		ret = RT_FAILED;
	if (RT_UNLIKELY(!heap->free(heap, &key_exists_array_heap_buffer)))
		ret = RT_FAILED;

	return ret;
}

rt_s rt_properties_merge_into_file(struct rt_properties *properties, const rt_char *input_file_path, const rt_char *output_file_path, enum rt_encoding encoding, enum rt_eol eol, rt_b delete_missing_keys)
{
	struct rt_heap *heap = properties->heap;
	void *file_heap_buffer = RT_NULL;
	rt_un file_heap_buffer_capacity = 0;
	rt_char8 *file_output;
	rt_un file_output_size;
	void *heap_buffer = RT_NULL;
	rt_un heap_buffer_capacity = 0;
	rt_char* output;
	rt_un output_size;
	struct rt_file output_file;
	rt_b output_file_created = RT_FALSE;
	struct rt_buffered_output_stream buffered_output_stream;
	rt_char8 buffered_output_stream_buffer[RT_CHAR8_BIG_STRING_SIZE];
	rt_s ret = RT_FAILED;

	/* Read the input file, making sure it exists. */
	if (RT_UNLIKELY(!rt_small_file_read(input_file_path, RT_NULL, 0, &file_heap_buffer, &file_heap_buffer_capacity, &file_output, &file_output_size, heap))) goto end;
	if (RT_UNLIKELY(!rt_encoding_decode(file_output, file_output_size, encoding, RT_NULL, 0, &heap_buffer, &heap_buffer_capacity, &output, &output_size, heap))) goto end;

	/* Open the output file for writing, also making sure it does not exist. */
	if (RT_UNLIKELY(!rt_file_create(&output_file, output_file_path, RT_FILE_MODE_NEW)))
		goto end;
	output_file_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_buffered_output_stream_create(&buffered_output_stream, &output_file.io_device.output_stream, buffered_output_stream_buffer, RT_CHAR8_BIG_STRING_SIZE)))
		goto end;

	if (RT_UNLIKELY(!rt_properties_merge_into_file_with_buffer(properties, output, output_size, &buffered_output_stream.output_stream, encoding, eol, delete_missing_keys)))
		goto end;

	if (RT_UNLIKELY(!rt_buffered_output_stream_flush(&buffered_output_stream.output_stream)))
		goto end;

	ret = RT_OK;
end:
	if (output_file_created) {
		if (RT_UNLIKELY(!rt_io_device_free(&output_file.io_device)))
			ret = RT_FAILED;
	}
	if (heap_buffer) {
		if (RT_UNLIKELY(!heap->free(heap, &heap_buffer)))
			ret = RT_FAILED;
	}
	if (file_heap_buffer) {
		if (RT_UNLIKELY(!heap->free(heap, &file_heap_buffer)))
			ret = RT_FAILED;
	}

	return ret;
}

rt_s rt_properties_free(struct rt_properties *properties)
{
	struct rt_heap *heap = properties->heap;
	rt_s ret = RT_OK;

	if (heap) {
		if (RT_UNLIKELY(!heap->free(heap, (void**)&properties->buffer)))
			ret = RT_FAILED;
	}

	if (RT_UNLIKELY(!rt_hash_table_free(&properties->hash_table)))
		ret = RT_FAILED;

	return ret;
}

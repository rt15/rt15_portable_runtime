#include "layer007/rt_properties.h"

#include "layer001/rt_memory.h"
#include "layer002/rt_error.h"
#include "layer003/rt_char.h"
#include "layer004/rt_small_file.h"
#include "layer005/rt_unicode_code_point.h"
#include "layer006/rt_properties_parser.h"

struct rt_properties_context {
	struct rt_hash_table_entry *hash_table;
	rt_char *buffer;
	rt_un buffer_size;
	rt_un buffer_capacity;
	rt_char *current_key;
};

static rt_s rt_properties_callback(enum rt_properties_parser_part_type part_type, const rt_char *str, rt_un index, rt_un size, void *context)
{
	struct rt_properties_context *properties_context = (struct rt_properties_context*)context;
	rt_char *current_key;
	rt_un current_key_size;
	rt_char *current_value;
	rt_s ret = RT_FAILED;

	if (part_type == RT_PROPERTIES_PARSER_PART_TYPE_KEY) {
		properties_context->current_key = &properties_context->buffer[properties_context->buffer_size];
		if (RT_UNLIKELY(!rt_properties_parser_parse_key(&str[index], size, properties_context->buffer, properties_context->buffer_capacity, &properties_context->buffer_size)))
			goto end;
		if (RT_UNLIKELY(!rt_char_append_char(_R('\0'), properties_context->buffer, properties_context->buffer_capacity, &properties_context->buffer_size)))
			goto end;
	}
	else if (part_type == RT_PROPERTIES_PARSER_PART_TYPE_VALUE) {
		current_key = properties_context->current_key;
		current_value = &properties_context->buffer[properties_context->buffer_size];
		if (RT_UNLIKELY(!rt_properties_parser_parse_value(&str[index], size, properties_context->buffer, properties_context->buffer_capacity, &properties_context->buffer_size)))
			goto end;
		if (RT_UNLIKELY(!rt_char_append_char(_R('\0'), properties_context->buffer, properties_context->buffer_capacity, &properties_context->buffer_size)))
			goto end;

		current_key_size = rt_char_get_size(current_key);

		if (RT_UNLIKELY(!rt_hash_table_set(&properties_context->hash_table, current_key, current_key_size, current_value, RT_NULL)))
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
	struct rt_properties_context context;
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

	if (RT_UNLIKELY(!rt_properties_parser_parse(str, str_size, &rt_properties_callback, &context)))
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

#include <rpr.h>

#include "zz_utils.h"

struct zz_test_hash_table_header {
	rt_un value;
	struct rt_hash_table_header hash_table_header;
};

/**
 * The hash is the first letter of <tt>data</tt>. It allows to create collisions easily.
 */
rt_s zz_test_hash_table_hash_callback(const void *data, rt_un data_size, void *context, rt_un *hash)
{
	rt_char8 *chars = (rt_char8*)data;
	rt_un context_value = *(rt_un*)context;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(context_value != 42)) goto end;
	if (RT_UNLIKELY(!data_size)) goto end;

	*hash = chars[0];

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_hash_table_check(struct rt_hash_table_entry *hash_table, rt_un size, rt_un array_size, struct rt_heap *heap)
{
	struct rt_hash_table_header *header;
	struct zz_test_hash_table_header *test_header;
	struct rt_hash_table_entry *entry;
	rt_un entries_count = 0;
	rt_un hash;
	rt_un i;
	rt_s ret = RT_FAILED;

	header = RT_HASH_TABLE_GET_HEADER(hash_table);
	test_header = RT_HASH_TABLE_GET_CUSTOM_HEADER(hash_table, header);

	/* Check array header. */
	if (RT_UNLIKELY(header->array_header.size != array_size)) goto end;
	if (RT_UNLIKELY(header->array_header.item_size != sizeof(struct rt_hash_table_entry))) goto end;
	if (RT_UNLIKELY(header->array_header.header_size != sizeof(struct zz_test_hash_table_header))) goto end;
	if (RT_UNLIKELY(header->array_header.heap != heap)) goto end;

	/* Check hash table header. */
	if (RT_UNLIKELY(*(rt_un*)header->context != 42)) goto end;
	if (RT_UNLIKELY(header->size != size)) goto end;

	/* Check custom header. */
	if (RT_UNLIKELY(test_header->value != 12)) goto end;

	for (i = 0; i < header->array_header.size; i++) {
		entry = &hash_table[i];
		if (entry->key) {
			if (RT_UNLIKELY(rt_char8_get_size(entry->key) != entry->key_size)) goto end;
			if (RT_UNLIKELY(!zz_test_hash_table_hash_callback(entry->key, entry->key_size, header->context, &hash))) goto end;
			if (RT_UNLIKELY(entry->key_hash != hash)) goto end;
			entries_count++;
		}
	}

	if (RT_UNLIKELY(entries_count != size))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_hash_table_display(struct rt_hash_table_entry *hash_table)
{
	struct rt_io_device io_device;
	struct rt_output_stream *output_stream;
	struct rt_array_header *header;
	rt_un array_size;
	rt_char8 buffer[RT_CHAR8_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_un i;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_io_device_create_from_std_output(&io_device)))
		goto end;

	output_stream = &io_device.output_stream;

	header = RT_ARRAY_GET_HEADER(hash_table);
	array_size = header->size;
	for (i = 0; i < array_size; i++) {
		if (hash_table[i].key) {
			buffer_size = 0;
			if (RT_UNLIKELY(!rt_char8_append_un(i, 10, buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
			if (RT_UNLIKELY(!rt_char8_append(": ", 2, buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
			if (RT_UNLIKELY(!rt_char8_append(hash_table[i].key, rt_char8_get_size(hash_table[i].key), buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
			if (RT_UNLIKELY(!rt_char8_append_char('(', buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
			if (RT_UNLIKELY(!rt_char8_append_un(hash_table[i].key_hash, 16, buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
			if (RT_UNLIKELY(!rt_char8_append(") => ", 5, buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
			if (RT_UNLIKELY(!rt_char8_append(hash_table[i].value, rt_char8_get_size(hash_table[i].value), buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
			if (RT_UNLIKELY(!rt_char8_append_char('\n', buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;

			if (RT_UNLIKELY(!output_stream->write(output_stream, buffer, buffer_size))) goto end;
		}
	}
	if (RT_UNLIKELY(!output_stream->write(output_stream, "\n", 1))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_hash_table_set(struct rt_hash_table_entry **hash_table, const void *key, const rt_char8 *value, const rt_char8 *expected_existing_value)
{
	rt_un key_size = rt_char8_get_size(key);
	rt_char8 *existing_value;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_hash_table_set(hash_table, key, key_size, value, (void**)&existing_value))) goto end;
	if (RT_UNLIKELY(!zz_char8_equals_with_nulls(existing_value, expected_existing_value))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_hash_table_get(struct rt_hash_table_entry *hash_table, const void *key, const rt_char8 *expected)
{
	rt_char8 *value;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_hash_table_get(hash_table, key, rt_char8_get_size(key), (void**)&value))) goto end;
	if (RT_UNLIKELY(!zz_char8_equals_with_nulls(value, expected))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_hash_table_delete(struct rt_hash_table_entry **hash_table, const void *key, const rt_char8 *expected_existing_value)
{
	rt_un key_size = rt_char8_get_size(key);
	rt_char8 *existing_value;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_hash_table_delete(hash_table, key, key_size, (void**)&existing_value))) goto end;
	if (RT_UNLIKELY(!zz_char8_equals_with_nulls(existing_value, expected_existing_value))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_hash_table_do(struct rt_heap *heap)
{
	struct rt_hash_table_entry *hash_table = RT_NULL;
	rt_un context_value = 42;
	struct rt_hash_table_header *header;
	struct zz_test_hash_table_header *test_header;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_hash_table_create(&hash_table, &zz_test_hash_table_hash_callback, &rt_char8_comparison_with_size_callback, &context_value, 4, sizeof(struct zz_test_hash_table_header), heap))) goto end;

	header = RT_HASH_TABLE_GET_HEADER(hash_table);
	test_header = RT_HASH_TABLE_GET_CUSTOM_HEADER(hash_table, header);
	test_header->value = 12;

	if (RT_UNLIKELY(!zz_test_hash_table_set(&hash_table, "foo", "TEMP", RT_NULL))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_check(hash_table, 1, 4, heap))) goto end;

	if (RT_UNLIKELY(!zz_test_hash_table_set(&hash_table, "foo", "FOO", "TEMP"))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_check(hash_table, 1, 4, heap))) goto end;

	if (RT_UNLIKELY(!zz_test_hash_table_set(&hash_table, "foo1", "FOO1", RT_NULL))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_check(hash_table, 2, 4, heap))) goto end;

	if (RT_UNLIKELY(!zz_test_hash_table_set(&hash_table, "a", "A", RT_NULL))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_check(hash_table, 3, 4, heap))) goto end;

	if (RT_UNLIKELY(!zz_test_hash_table_set(&hash_table, "a1", "A1", RT_NULL))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_check(hash_table, 4, 8, heap))) goto end;

	if (RT_UNLIKELY(!zz_test_hash_table_set(&hash_table, "foo2", "FOO2", RT_NULL))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_check(hash_table, 5, 8, heap))) goto end;

	if (RT_UNLIKELY(!zz_test_hash_table_display(hash_table))) goto end;

	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "foo", "FOO"))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "foo1", "FOO1"))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "foo2", "FOO2"))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "a", "A"))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "a1", "A1"))) goto end;

	/* Search non-existing item. */
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "team", RT_NULL))) goto end;

	/* Search non-existing item with full hash collision. */
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "fooXXX", RT_NULL))) goto end;

	/* Delete non-existing item. */
	if (RT_UNLIKELY(!zz_test_hash_table_delete(&hash_table, "team", RT_NULL))) goto end;

	/* Delete non-existing item with full hash collision. */
	if (RT_UNLIKELY(!zz_test_hash_table_delete(&hash_table, "fooXXX", RT_NULL))) goto end;

	/* Check that nothing was deleted. */
	if (RT_UNLIKELY(!zz_test_hash_table_check(hash_table, 5, 8, heap))) goto end;

	/* Test delete. */
	if (RT_UNLIKELY(!zz_test_hash_table_delete(&hash_table, "foo1", "FOO1"))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_check(hash_table, 4, 8, heap))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "foo", "FOO"))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "foo1", RT_NULL))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "foo2", "FOO2"))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "a", "A"))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "a1", "A1"))) goto end;

	/* Test delete on another entry. */
	if (RT_UNLIKELY(!zz_test_hash_table_delete(&hash_table, "foo", "FOO"))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_check(hash_table, 3, 8, heap))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "foo", RT_NULL))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "foo1", RT_NULL))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "foo2", "FOO2"))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "a", "A"))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "a1", "A1"))) goto end;

	/* Test delete on another entry. */
	if (RT_UNLIKELY(!zz_test_hash_table_delete(&hash_table, "a", "A"))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_check(hash_table, 2, 8, heap))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "foo", RT_NULL))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "foo1", RT_NULL))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "foo2", "FOO2"))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "a", RT_NULL))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "a1", "A1"))) goto end;

	/* Test delete on another entry. */
	if (RT_UNLIKELY(!zz_test_hash_table_delete(&hash_table, "a1", "A1"))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_check(hash_table, 1, 8, heap))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "foo", RT_NULL))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "foo1", RT_NULL))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "foo2", "FOO2"))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "a", RT_NULL))) goto end;
	if (RT_UNLIKELY(!zz_test_hash_table_get(hash_table, "a1", RT_NULL))) goto end;

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!rt_hash_table_free(&hash_table)))
		ret = RT_FAILED;

	return ret;
}

rt_s zz_test_hash_table(void)
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_runtime_heap_create(&runtime_heap)))
		goto end;
	runtime_heap_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_test_hash_table_do(&runtime_heap.heap))) goto end;

	ret = RT_OK;
end:
	if (runtime_heap_created) {
		if (RT_UNLIKELY(!runtime_heap.heap.close(&runtime_heap.heap)))
			ret = RT_FAILED;
	}

	return ret;
}

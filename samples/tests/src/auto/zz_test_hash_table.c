#include <rpr.h>

#include "zz_utils.h"

struct zz_test_hash_table_header {
	rt_un value;
	struct rt_hash_table_header hash_table_header;
};

/**
 * The hash is the first letter <tt>data</tt>. It allows to create collisions easily.
 */
rt_s zz_test_hash_table_hash_callback(const rt_char8 *data, rt_un data_size, void *context, rt_un *hash)
{
	rt_un context_value = *(rt_un*)context;
	rt_s ret;

	if (context_value != 42) goto error;
	if (!data_size) goto error;

	*hash = data[0];

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_hash_table_check(struct rt_hash_table_entry *hash_table, rt_un size, rt_un array_size, struct rt_heap *heap)
{
	struct rt_hash_table_header *header;
	struct zz_test_hash_table_header *test_header;
	struct rt_hash_table_entry *entry;
	rt_un entries_count = 0;
	rt_un hash;
	rt_un i;
	rt_s ret;

	header = RT_HASH_TABLE_GET_HEADER(hash_table);
	test_header = RT_HASH_TABLE_GET_CUSTOM_HEADER(hash_table, header);

	/* Check array header. */
	if (header->array_header.size != array_size) goto error;
	if (header->array_header.item_size != sizeof(struct rt_hash_table_entry)) goto error;
	if (header->array_header.header_size != sizeof(struct zz_test_hash_table_header)) goto error;
	if (header->array_header.heap != heap) goto error;

	/* Check hash table header. */
	if (*(rt_un*)header->context != 42) goto error;
	if (header->size != size) goto error;

	/* Check custom header. */
	if (test_header->value != 12) goto error;

	for (i = 0; i < header->array_header.size; i++) {
		entry = &hash_table[i];
		if (entry->key) {
			if (rt_char8_get_size(entry->key) != entry->key_size) goto error;
			if (!zz_test_hash_table_hash_callback(entry->key, entry->key_size, header->context, &hash)) goto error;
			if (entry->key_hash != hash) goto error;
			entries_count++;
		}
	}

	if (entries_count != size)
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
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
	rt_s ret;

	if (!rt_io_device_create_from_std_output(&io_device))
		goto error;

	output_stream = &io_device.output_stream;

	header = RT_ARRAY_GET_HEADER(hash_table);
	array_size = header->size;
	for (i = 0; i < array_size; i++) {
		if (hash_table[i].key) {
			buffer_size = 0;
			if (!rt_char8_append_un(i, 10, buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size)) goto error;
			if (!rt_char8_append(": ", 2, buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size)) goto error;
			if (!rt_char8_append(hash_table[i].key, rt_char8_get_size(hash_table[i].key), buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size)) goto error;
			if (!rt_char8_append_char('(', buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size)) goto error;
			if (!rt_char8_append_un(hash_table[i].key_hash, 16, buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size)) goto error;
			if (!rt_char8_append(") => ", 5, buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size)) goto error;
			if (!rt_char8_append(hash_table[i].value, rt_char8_get_size(hash_table[i].value), buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size)) goto error;
			if (!rt_char8_append_char('\n', buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size)) goto error;

			if (!output_stream->write(output_stream, buffer, buffer_size)) goto error;
		}
	}
	if (!output_stream->write(output_stream, "\n", 1)) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_hash_table_set(struct rt_hash_table_entry **hash_table, const rt_char8 *key, const rt_char8 *value, const rt_char8 *expected_existing_value)
{
	rt_un key_size = rt_char8_get_size(key);
	rt_char8 *existing_value;
	rt_s ret;

	if (!rt_hash_table_set(hash_table, key, key_size, value, (void**)&existing_value)) goto error;
	if (!zz_char8_equals_with_nulls(existing_value, expected_existing_value)) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_hash_table_get(struct rt_hash_table_entry *hash_table, const rt_char8 *key, const rt_char8 *expected)
{
	rt_char8 *value;
	rt_s ret;

	if (!rt_hash_table_get(hash_table, key, rt_char8_get_size(key), (void**)&value)) goto error;
	if (!zz_char8_equals_with_nulls(value, expected)) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_hash_table_do(struct rt_heap *heap)
{
	struct rt_hash_table_entry *hash_table = RT_NULL;
	rt_un context_value = 42;
	struct rt_hash_table_header *header;
	struct zz_test_hash_table_header *test_header;
	rt_s ret;

	if (!rt_hash_table_create(&hash_table, &zz_test_hash_table_hash_callback, &rt_char8_comparison_with_size_callback, &context_value, 4, sizeof(struct zz_test_hash_table_header), heap)) goto error;

	header = RT_HASH_TABLE_GET_HEADER(hash_table);
	test_header = RT_HASH_TABLE_GET_CUSTOM_HEADER(hash_table, header);
	test_header->value = 12;

	if (!zz_test_hash_table_set(&hash_table, "foo", "TEMP", RT_NULL)) goto error;
	if (!zz_test_hash_table_check(hash_table, 1, 4, heap)) goto error;

	if (!zz_test_hash_table_set(&hash_table, "foo", "FOO", "TEMP")) goto error;
	if (!zz_test_hash_table_check(hash_table, 1, 4, heap)) goto error;

	if (!zz_test_hash_table_set(&hash_table, "foo1", "FOO1", RT_NULL)) goto error;
	if (!zz_test_hash_table_check(hash_table, 2, 4, heap)) goto error;

	if (!zz_test_hash_table_set(&hash_table, "bar", "BAR", RT_NULL)) goto error;
	if (!zz_test_hash_table_check(hash_table, 3, 4, heap)) goto error;

	if (!zz_test_hash_table_set(&hash_table, "bar1", "BAR1", RT_NULL)) goto error;
	if (!zz_test_hash_table_check(hash_table, 4, 8, heap)) goto error;

	if (!zz_test_hash_table_display(hash_table)) goto error;

	if (!zz_test_hash_table_get(hash_table, "foo", "FOO")) goto error;
	if (!zz_test_hash_table_get(hash_table, "foo1", "FOO1")) goto error;
	if (!zz_test_hash_table_get(hash_table, "bar", "BAR")) goto error;
	if (!zz_test_hash_table_get(hash_table, "bar1", "BAR1")) goto error;

	/* Search non-existing item. */
	if (!zz_test_hash_table_get(hash_table, "team", RT_NULL)) goto error;

	/* Search non-existing item with full hash collision. */
	if (!zz_test_hash_table_get(hash_table, "foo3", RT_NULL)) goto error;

	ret = RT_OK;
free:
	if (!rt_hash_table_free(&hash_table) && ret)
		goto error;
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_hash_table()
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	rt_s ret;

	if (!rt_runtime_heap_create(&runtime_heap))
		goto error;
	runtime_heap_created = RT_TRUE;

	if (!zz_test_hash_table_do(&runtime_heap.heap)) goto error;

	ret = RT_OK;
free:
	if (runtime_heap_created) {
		runtime_heap_created = RT_FALSE;
		if (!runtime_heap.heap.close(&runtime_heap.heap) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

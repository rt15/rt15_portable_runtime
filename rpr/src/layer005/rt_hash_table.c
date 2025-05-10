#include "layer005/rt_hash_table.h"

#include "layer001/rt_memory.h"
#include "layer002/rt_error.h"

rt_s rt_hash_table_create(struct rt_hash_table_entry **hash_table, rt_hash_callback_t hash_callback, rt_comparison_with_size_callback_t comparison_callback, void *context, rt_un initial_capacity, rt_un header_size, struct rt_heap *heap)
{
	struct rt_hash_table_header *header;
	rt_un i;
	rt_s ret = RT_FAILED;

	*hash_table = RT_NULL;

	if (RT_UNLIKELY(!RT_MEMORY_IS_POWER_OF_TWO(initial_capacity))) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	if (!header_size)
		header_size = sizeof(struct rt_hash_table_header);

	if (RT_UNLIKELY(!rt_array_create((void**)hash_table, initial_capacity, sizeof(struct rt_hash_table_entry), header_size, heap)))
		goto end;

	header = RT_HASH_TABLE_GET_HEADER(*hash_table);
	header->hash_callback = hash_callback;
	header->comparison_callback = comparison_callback;
	header->context = context;
	header->size = 0;

	/* Mark all the entries as empty. */
	for (i = 0; i < initial_capacity; i++)
		(*hash_table)[i].key = RT_NULL;

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!ret)) {
		rt_hash_table_free(hash_table);
	}

	return ret;
}

static rt_s rt_hash_table_expand(struct rt_hash_table_entry **hash_table)
{
	struct rt_hash_table_entry *new_hash_table = RT_NULL;
	rt_un header_size;
	rt_un full_hash;
	rt_un index;
	struct rt_hash_table_entry *old_hash_table = *hash_table;

	struct rt_hash_table_header *header;
	rt_uchar8 *custom_header;
	rt_un array_size;
	struct rt_hash_table_entry *entry;

	struct rt_hash_table_header *new_header;
	rt_uchar8 *new_custom_header;
	rt_un new_array_size;
	struct rt_hash_table_entry *new_entry;

	rt_un i;
	rt_s ret = RT_FAILED;

	header = RT_HASH_TABLE_GET_HEADER(*hash_table);
	header_size = header->array_header.header_size;
	array_size = header->array_header.size;
	new_array_size = array_size * 2;

	if (RT_UNLIKELY(!rt_array_create((void**)&new_hash_table, new_array_size, sizeof(struct rt_hash_table_entry), header_size, header->array_header.heap))) goto end;

	new_header = RT_HASH_TABLE_GET_HEADER(new_hash_table);

	/* Mark all the entries as empty in the new hash table. */
	for (i = 0; i < new_array_size; i++)
		new_hash_table[i].key = RT_NULL;

	/* Copy the current header into the new header (including the custom part). */
	custom_header = RT_HASH_TABLE_GET_CUSTOM_HEADER(*hash_table, header);
	new_custom_header = RT_HASH_TABLE_GET_CUSTOM_HEADER(new_hash_table, new_header);
	RT_MEMORY_COPY(custom_header, new_custom_header, header_size);

	/* Fix the size after overwriting the header. */
	new_header->array_header.size = new_array_size;

	/* Put existing items into the new array. */
	for (i = 0; i < array_size; i++) {
		entry = &(*hash_table)[i];
		if (entry->key) {
			full_hash = entry->key_hash;

			index = (full_hash & (new_array_size - 1));
			new_entry = &new_hash_table[index];

			/* We search for the first free spot. We know that there is one since we have expanded the hash table size. */
			while (new_entry->key) {
				index++;
				if (index == new_array_size)
					index = 0;
				new_entry = &new_hash_table[index];
			}
			new_entry->key_hash = full_hash;
			new_entry->key = entry->key;
			new_entry->key_size = entry->key_size;
			new_entry->value = entry->value;
		}
	}
	*hash_table = new_hash_table;

	/* If it fails, then we do not free the new hash table. */
	ret = rt_array_free((void**)&old_hash_table);

end:
	if (RT_UNLIKELY(!ret)) {
		rt_array_free((void**)&new_hash_table);
	}

	return ret;
}

rt_s rt_hash_table_set(struct rt_hash_table_entry **hash_table, const void *key, rt_un key_size, const void *value, void **existing_value)
{
	struct rt_hash_table_header *header;
	rt_un array_size;
	rt_un full_hash;
	rt_un index;
	struct rt_hash_table_entry *entry;
	rt_n comparison_result;
	rt_s ret = RT_FAILED;

	header = RT_HASH_TABLE_GET_HEADER(*hash_table);

	/* Expand the hash table if it is already more than 50% full. */
	if (header->size > header->array_header.size / 2) {
		if (RT_UNLIKELY(!rt_hash_table_expand(hash_table)))
			goto end;

		header = RT_HASH_TABLE_GET_HEADER(*hash_table);
	}

	array_size = header->array_header.size;

	if (RT_UNLIKELY(!header->hash_callback(key, key_size, header->context, &full_hash)))
		goto end;

	index = (full_hash & (array_size - 1));
	entry = &(*hash_table)[index];

	/* We search for the first free spot. We know that there is one since we have expanded the hash table size if needed. */
	while (entry->key) {
		if (entry->key_hash == full_hash) {
			if (RT_UNLIKELY(!header->comparison_callback(entry->key, entry->key_size, key, key_size, header->context, &comparison_result)))
				goto end;
			if (!comparison_result) {
				/* Same key, we will replace the existing value. */
				break;
			}
		}

		index++;
		if (index == array_size)
			index = 0;
		entry = &(*hash_table)[index];
	}
	if (entry->key) {
		/* New value for an existing item. */
		if (existing_value)
			*existing_value = (void*)entry->value;
		entry->value = value;

	} else {
		/* New item. */
		if (existing_value)
			*existing_value = RT_NULL;
		entry->key_hash = full_hash;
		entry->key = key;
		entry->key_size = key_size;
		entry->value = value;
		header->size++;
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_hash_table_get(struct rt_hash_table_entry *hash_table, const void *key, rt_un key_size, void **value)
{
	struct rt_hash_table_header *header;
	rt_un array_size;
	rt_un full_hash;
	rt_un index;
	struct rt_hash_table_entry *entry;
	rt_n comparison_result;
	rt_s ret = RT_FAILED;

	*value = RT_NULL;

	header = RT_HASH_TABLE_GET_HEADER(hash_table);
	array_size = header->array_header.size;

	if (RT_UNLIKELY(!header->hash_callback(key, key_size, header->context, &full_hash)))
		goto end;

	index = (full_hash & (array_size - 1));
	entry = &hash_table[index];
	while (entry->key) {
		if (entry->key_hash == full_hash) {
			if (RT_UNLIKELY(!header->comparison_callback(entry->key, entry->key_size, key, key_size, header->context, &comparison_result)))
				goto end;
			if (!comparison_result) {
				*value = (void*)entry->value;
				break;
			}
		}
		index++;
		if (index == array_size)
			index = 0;
		entry = &hash_table[index];
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_hash_table_delete(struct rt_hash_table_entry **hash_table, const void *key, rt_un key_size, void **existing_value)
{
	struct rt_hash_table_header *header;
	rt_un array_size;
	rt_un full_hash;
	rt_un delete_index;
	rt_un index;
	rt_un preferred_index;
	struct rt_hash_table_entry *delete_entry;
	struct rt_hash_table_entry *entry;
	rt_n comparison_result;
	rt_s ret = RT_FAILED;

	if (existing_value)
		*existing_value = RT_NULL;

	header = RT_HASH_TABLE_GET_HEADER(*hash_table);
	array_size = header->array_header.size;

	if (RT_UNLIKELY(!header->hash_callback(key, key_size, header->context, &full_hash)))
		goto end;

	delete_index = (full_hash & (array_size - 1));
	delete_entry = &(*hash_table)[delete_index];
	while (delete_entry->key) {
		if (delete_entry->key_hash == full_hash) {
			if (RT_UNLIKELY(!header->comparison_callback(delete_entry->key, delete_entry->key_size, key, key_size, header->context, &comparison_result)))
				goto end;
			if (!comparison_result) {

				/* We found the item to delete. */

				if (existing_value)
					*existing_value = (void*)delete_entry->value;

				/* Delete the entry, keeping linear probing functional. */
				index = delete_index;
				while (RT_TRUE) {
					index = index + 1;
					if (index == array_size)
						index = 0;

					entry = &(*hash_table)[index];
					if (!entry->key) {
						/* We found an empty entry. */
						break;
					}
					preferred_index = (entry->key_hash & (array_size - 1));

					/* Is entry at index (with preferred index preferred_index) invalidly positioned? */
					if ((index > delete_index && (preferred_index <= delete_index || preferred_index > index)) ||
					    (index < delete_index && (preferred_index <= delete_index && preferred_index > index))) {

						/* Replace the entry at delete_index by the one at index. */
						delete_entry = &(*hash_table)[delete_index];
						RT_MEMORY_COPY(entry, delete_entry, sizeof(struct rt_hash_table_entry));

						/* The entry we just moved is now the one to delete. */
						delete_index = index;
					}

				}

				/* Mark the entry to delete as deleted. */
				delete_entry = &(*hash_table)[delete_index];
				delete_entry->key = RT_NULL;
				header->size--;

				/* We found the record. */
				break;
			}
		}
		delete_index++;
		if (delete_index == array_size)
			delete_index = 0;
		delete_entry = &(*hash_table)[delete_index];
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_hash_table_free(struct rt_hash_table_entry **hash_table)
{
	return rt_array_free((void**)hash_table);
}

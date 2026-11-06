#include "layer005/rt_hash_table.h"

#include "layer001/rt_memory.h"
#include "layer002/rt_error.h"

rt_s rt_hash_table_create(struct rt_hash_table *hash_table, rt_hash_callback_t hash_callback, rt_comparison_with_size_callback_t comparison_callback, void *context, rt_un initial_capacity, struct rt_heap *heap)
{
	struct rt_hash_table_entry *entries;
	rt_un i;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!RT_MEMORY_IS_POWER_OF_TWO(initial_capacity))) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	hash_table->hash_callback = hash_callback;
	hash_table->comparison_callback = comparison_callback;
	hash_table->context = context;
	hash_table->size = 0;

	if (RT_UNLIKELY(!rt_array_create(&hash_table->entries, initial_capacity, sizeof(struct rt_hash_table_entry), heap)))
		goto end;

	/* Mark all the entries as empty. */
	entries = (struct rt_hash_table_entry*)hash_table->entries.items;
	for (i = 0; i < initial_capacity; i++)
		entries[i].key = RT_NULL;

	ret = RT_OK;
end:
	return ret;
}

static rt_s rt_hash_table_expand(struct rt_hash_table *hash_table)
{
	struct rt_array old_array;
	struct rt_array new_array;
	rt_b new_array_created = RT_FALSE;
	rt_b new_array_used = RT_FALSE;
	rt_un full_hash;
	rt_un index;

	struct rt_hash_table_entry *entries;
	rt_un array_size;
	struct rt_hash_table_entry *entry;

	struct rt_hash_table_entry *new_entries;
	rt_un new_array_size;
	struct rt_hash_table_entry *new_entry;

	rt_un i;
	rt_s ret = RT_FAILED;

	entries = (struct rt_hash_table_entry*)hash_table->entries.items;
	array_size = hash_table->entries.size;
	new_array_size = array_size * 2;

	if (RT_UNLIKELY(!rt_array_create(&new_array, new_array_size, sizeof(struct rt_hash_table_entry), hash_table->entries.heap))) goto end;
	new_array_created = RT_TRUE;

	new_entries = (struct rt_hash_table_entry*)new_array.items;

	/* Mark all the entries as empty in the new array. */
	for (i = 0; i < new_array_size; i++)
		new_entries[i].key = RT_NULL;

	/* Put existing items into the new array. */
	for (i = 0; i < array_size; i++) {
		entry = &entries[i];
		if (entry->key) {
			full_hash = entry->key_hash;

			index = (full_hash & (new_array_size - 1));
			new_entry = &new_entries[index];

			/* We search for the first free spot. We know that there is one since we have expanded the hash table size. */
			while (new_entry->key) {
				index++;
				if (index == new_array_size)
					index = 0;
				new_entry = &new_entries[index];
			}
			new_entry->key_hash = full_hash;
			new_entry->key = entry->key;
			new_entry->key_size = entry->key_size;
			new_entry->value = entry->value;
		}
	}

	/* Replace the old array by the new one. */
	RT_MEMORY_COPY(&hash_table->entries, &old_array, sizeof(struct rt_array));
	RT_MEMORY_COPY(&new_array, &hash_table->entries, sizeof(struct rt_array));
	new_array_used = RT_TRUE;

	/* If it fails, then we do not free the new array since it is used by the hash table. */
	if (RT_UNLIKELY(!rt_array_free(&old_array)))
		goto end;

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!ret && new_array_created && !new_array_used)) {
		rt_array_free(&new_array);
	}

	return ret;
}

rt_s rt_hash_table_set(struct rt_hash_table *hash_table, const void *key, rt_un key_size, const void *value, void **existing_value)
{
	struct rt_hash_table_entry *entries;
	rt_un array_size;
	rt_un full_hash;
	rt_un index;
	struct rt_hash_table_entry *entry;
	rt_n comparison_result;
	rt_s ret = RT_FAILED;

	/* Expand the hash table if it is already more than 50% full. */
	if (hash_table->size > hash_table->entries.size / 2) {
		if (RT_UNLIKELY(!rt_hash_table_expand(hash_table)))
			goto end;
	}

	entries = (struct rt_hash_table_entry*)hash_table->entries.items;
	array_size = hash_table->entries.size;

	if (RT_UNLIKELY(!hash_table->hash_callback(key, key_size, hash_table->context, &full_hash)))
		goto end;

	index = (full_hash & (array_size - 1));
	entry = &entries[index];

	/* We search for the first free spot. We know that there is one since we have expanded the hash table size if needed. */
	while (entry->key) {
		if (entry->key_hash == full_hash) {
			if (RT_UNLIKELY(!hash_table->comparison_callback(entry->key, entry->key_size, key, key_size, hash_table->context, &comparison_result)))
				goto end;
			if (!comparison_result) {
				/* Same key, we will replace the existing value. */
				break;
			}
		}

		index++;
		if (index == array_size)
			index = 0;
		entry = &entries[index];
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
		hash_table->size++;
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_hash_table_get(struct rt_hash_table *hash_table, const void *key, rt_un key_size, void **value)
{
	struct rt_hash_table_entry *entries;
	rt_un array_size;
	rt_un full_hash;
	rt_un index;
	struct rt_hash_table_entry *entry;
	rt_n comparison_result;
	rt_s ret = RT_FAILED;

	*value = RT_NULL;

	entries = (struct rt_hash_table_entry*)hash_table->entries.items;
	array_size = hash_table->entries.size;

	if (RT_UNLIKELY(!hash_table->hash_callback(key, key_size, hash_table->context, &full_hash)))
		goto end;

	index = (full_hash & (array_size - 1));
	entry = &entries[index];
	while (entry->key) {
		if (entry->key_hash == full_hash) {
			if (RT_UNLIKELY(!hash_table->comparison_callback(entry->key, entry->key_size, key, key_size, hash_table->context, &comparison_result)))
				goto end;
			if (!comparison_result) {
				*value = (void*)entry->value;
				break;
			}
		}
		index++;
		if (index == array_size)
			index = 0;
		entry = &entries[index];
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_hash_table_delete(struct rt_hash_table *hash_table, const void *key, rt_un key_size, void **existing_value)
{
	struct rt_hash_table_entry *entries;
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

	entries = (struct rt_hash_table_entry*)hash_table->entries.items;
	array_size = hash_table->entries.size;

	if (RT_UNLIKELY(!hash_table->hash_callback(key, key_size, hash_table->context, &full_hash)))
		goto end;

	delete_index = (full_hash & (array_size - 1));
	delete_entry = &entries[delete_index];
	while (delete_entry->key) {
		if (delete_entry->key_hash == full_hash) {
			if (RT_UNLIKELY(!hash_table->comparison_callback(delete_entry->key, delete_entry->key_size, key, key_size, hash_table->context, &comparison_result)))
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

					entry = &entries[index];
					if (!entry->key) {
						/* We found an empty entry. */
						break;
					}
					preferred_index = (entry->key_hash & (array_size - 1));

					/* Is entry at index (with preferred index preferred_index) invalidly positioned? */
					if ((index > delete_index && (preferred_index <= delete_index || preferred_index > index)) ||
					    (index < delete_index && (preferred_index <= delete_index && preferred_index > index))) {

						/* Replace the entry at delete_index by the one at index. */
						delete_entry = &entries[delete_index];
						RT_MEMORY_COPY(entry, delete_entry, sizeof(struct rt_hash_table_entry));

						/* The entry we just moved is now the one to delete. */
						delete_index = index;
					}

				}

				/* Mark the entry to delete as deleted. */
				delete_entry = &entries[delete_index];
				delete_entry->key = RT_NULL;
				hash_table->size--;

				/* We found the record. */
				break;
			}
		}
		delete_index++;
		if (delete_index == array_size)
			delete_index = 0;
		delete_entry = &entries[delete_index];
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_hash_table_free(struct rt_hash_table *hash_table)
{
	hash_table->size = 0;

	return rt_array_free(&hash_table->entries);
}

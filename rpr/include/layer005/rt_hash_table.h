#ifndef RT_HASH_TABLE_H
#define RT_HASH_TABLE_H

#include "layer000/rt_types.h"
#include "layer001/rt_comparison.h"
#include "layer001/rt_hash.h"
#include "layer003/rt_heap.h"
#include "layer004/rt_array.h"

/**
 * @file
 * A hash table implementation.
 *
 * <p>
 * Collisions are dealt with using linear probing:<br>
 * If a new key has the same hash as an existing key, then the first next free entry is used.
 * </p>
 *
 * <p>
 * The memory of keys and values are not managed by the hash table.<br>
 * The hash table only contains pointers.
 * </p>
 */

#define RT_HASH_TABLE_GET_HEADER(HASH_TABLE) (struct rt_hash_table_header*)HASH_TABLE - 1
#define RT_HASH_TABLE_GET_CUSTOM_HEADER(HASH_TABLE, HASH_TABLE_HEADER) (void*)((rt_uchar8*)HASH_TABLE - HASH_TABLE_HEADER->array_header.header_size)

struct rt_hash_table_header {
	rt_hash_callback_t hash_callback;
	rt_comparison_with_size_callback_t comparison_callback;
	void *context;
	rt_un size;
	struct rt_array_header array_header;
};

struct rt_hash_table_entry {
	rt_un key_hash;      /* Full hash of the key.           */
	const rt_char8 *key; /* If RT_NULL, the entry is empty. */
	rt_un key_size;      /* Key size, used to compare keys. */
	const void *value;   /* Value associated with the key.  */
};

/**
 * <p>
 * Do not forget to call <tt>rt_hash_table_free</tt> once you are done with the hash table.
 * </p>
 *
 * @param initial_capacity Should be at least twice the initial number of entries and must be a power of two.
 * @param header_size Can be zero to use the default header size.
 */
rt_s rt_hash_table_create(struct rt_hash_table_entry **hash_table, rt_hash_callback_t hash_callback, rt_comparison_with_size_callback_t comparison_callback, void *context, rt_un initial_capacity, rt_un header_size, struct rt_heap *heap);

/**
 * <p>
 * If an entry with given <tt>key</tt> already exists, then the existing value is replaced by <tt>value</tt>.
 * </p>
 *
 * @param existing_value Out parameter. If not RT_NULL, receives the existing value if the key was already used.
 */
rt_s rt_hash_table_set(struct rt_hash_table_entry **hash_table, const rt_char8 *key, rt_un key_size, const void *value, void **existing_value);

/**
 *
 * @param value Will receive RT_NULL if the entry is not found.
 */
rt_s rt_hash_table_get(struct rt_hash_table_entry *hash_table, const rt_char8 *key, rt_un key_size, void **value);

/**
 *
 * <p>
 * Does nothing if <tt>key</tt> is not found in the hash table.
 * </p>
 *
 * @param existing_value Out parameter. If not RT_NULL, receives the existing value if the key was used.
 */
rt_s rt_hash_table_delete(struct rt_hash_table_entry **hash_table, const rt_char8 *key, rt_un key_size, void **existing_value);

/**
 * Same as <tt>rt_array_free</tt>.
 *
 * <p>
 * Set <tt>*hash_table</tt> to RT_NULL.<br>
 * Does nothing if <tt>*hash_table</tt> is RT_NULL.
 * </p>
 */
rt_s rt_hash_table_free(struct rt_hash_table_entry **hash_table);

#endif /* RT_HASH_TABLE_H */
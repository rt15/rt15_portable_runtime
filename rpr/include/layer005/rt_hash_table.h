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

struct rt_hash_table {
	rt_hash_callback_t hash_callback;
	rt_comparison_with_size_callback_t comparison_callback;
	void *context;
	rt_un size;              /* Number of used entries. */
	struct rt_array entries; /* Array of struct rt_hash_table_entry. */
};

struct rt_hash_table_entry {
	rt_un key_hash;    /* Full hash of the key.           */
	const void *key;   /* If RT_NULL, the entry is empty. */
	rt_un key_size;    /* Key size, used to compare keys. */
	const void *value; /* Value associated with the key.  */
};

/**
 * Create an empty hashtable with given initial capacity.
 *
 * <p>
 * Do not forget to call <tt>rt_hash_table_free</tt> once you are done with the hash table.<br>
 * In case of failure, <tt>rt_hash_table_free</tt> must not be called.
 * </p>
 *
 * @param initial_capacity Should be at least twice the initial number of entries and must be a power of two.
 */
RT_API rt_s rt_hash_table_create(struct rt_hash_table *hash_table, rt_hash_callback_t hash_callback, rt_comparison_with_size_callback_t comparison_callback, void *context, rt_un initial_capacity, struct rt_heap *heap);

/**
 * Add or set the entry of given <tt>key</tt> with associated given <tt>value</tt>.
 *
 * <p>
 * If an entry with given <tt>key</tt> already exists, then the existing value is replaced by <tt>value</tt>.
 * </p>
 *
 * @param existing_value Out parameter. If not RT_NULL, receives the existing value if the key was already used.
 */
RT_API rt_s rt_hash_table_set(struct rt_hash_table *hash_table, const void *key, rt_un key_size, const void *value, void **existing_value);

/**
 * Retrieve the value for given <tt>key</tt>.
 *
 * @param value Will receive RT_NULL if the entry is not found.
 */
RT_API rt_s rt_hash_table_get(struct rt_hash_table *hash_table, const void *key, rt_un key_size, void **value);

/**
 * Delete the entry with given <tt>key</tt>.
 *
 * <p>
 * Does nothing if <tt>key</tt> is not found in the hash table.
 * </p>
 *
 * @param existing_value Out parameter. If not RT_NULL, receives the existing value if the key was used.
 */
RT_API rt_s rt_hash_table_delete(struct rt_hash_table *hash_table, const void *key, rt_un key_size, void **existing_value);

/**
 * Same as <tt>rt_array_free</tt>.
 *
 * <p>
 * It is safe to call this function several times on the same hash table.
 * </p>
 */
RT_API rt_s rt_hash_table_free(struct rt_hash_table *hash_table);

#endif /* RT_HASH_TABLE_H */

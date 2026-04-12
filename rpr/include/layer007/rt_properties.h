#ifndef RT_PROPERTIES_H
#define RT_PROPERTIES_H

#include "layer000/rt_types.h"
#include "layer004/rt_encoding.h"
#include "layer005/rt_hash_table.h"

/**
 * @file
 * See rt_properties_parser for a description of the properties file format.
 */

struct rt_properties {
	struct rt_hash_table_entry *hash_table;
	const rt_char *buffer;
	struct rt_heap *heap;
};

/**
 * Create a properties structure loading given <tt>file_path</tt>.<br>
 * Keys and values are stored into <tt>buffer</tt>.
 *
 * @param initial_capacity Should be at least twice the initial number of entries and must be a power of two.
 */
RT_API rt_s rt_properties_create(struct rt_properties *properties, const rt_char *file_path, enum rt_encoding encoding, rt_un initial_capacity, struct rt_heap *heap);

/**
 * Create a property structure parsing given <tt>str</tt>.<br>
 * Keys and values are stored into <tt>buffer</tt>.
 *
 * @param str The string to parse.
 * @param initial_capacity Should be at least twice the initial number of entries and must be a power of two.
 */
RT_API rt_s rt_properties_create_from_str(struct rt_properties *properties, const rt_char *str, rt_un str_size, rt_un initial_capacity, struct rt_heap *heap);

/**
 * Create a properties structure from given <tt>hash_table</tt>.<br>
 * The buffer is set to RT_NULL since the hash-table manages the keys and values.
 */
RT_API rt_s rt_properties_create_from_hash_table(struct rt_properties *properties, struct rt_hash_table_entry *hash_table, struct rt_heap *heap);

/**
 * Free the buffer with the keys and values then the hash table.
 */
RT_API rt_s rt_properties_free(struct rt_properties *properties);

#endif /* RT_PROPERTIES_H */

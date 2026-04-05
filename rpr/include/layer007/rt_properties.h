#ifndef RT_PROPERTIES_H
#define RT_PROPERTIES_H

#include "layer000/rt_types.h"
#include "layer004/rt_encoding.h"
#include "layer005/rt_hash_table.h"

/**
 * @file
 * See rt_properties_parser for a description of the properties file format.
 */

#define RT_PROPERTIES_GET_HEADER(PROPERTIES) (struct rt_properties_header*)PROPERTIES - 1

struct rt_properties_header {
	const rt_char *buffer;
	struct rt_heap *heap;
	struct rt_hash_table_header hash_table_header;
};

/**
 * Create an hash table loading given <tt>file_path</tt>.<br>
 * Keys and values are stored into <tt>buffer</tt>.
 *
 * @param initial_capacity Should be at least twice the initial number of entries and must be a power of two.
 */
RT_API rt_s rt_properties_create(struct rt_hash_table_entry **properties, const rt_char *file_path, enum rt_encoding encoding, rt_un initial_capacity, struct rt_heap *heap);

/**
 * Create an hash table parsing given <tt>str</tt>.
 *
 * <p>
 * The <tt>file_path</tt> field is initialized to RT_NULL.<br>
 * The <tt>buffer</tt> field is allocated to store parsed keys and values.
 * </p>
 *
 * @param str The string to parse (will not be modified).
 * @param initial_capacity Should be at least twice the initial number of entries and must be a power of two.
 */
RT_API rt_s rt_properties_create_from_str(struct rt_hash_table_entry **properties, const rt_char *str, rt_un str_size, rt_un initial_capacity, struct rt_heap *heap);

/**
 * Free the buffer with the keys and values then the hash table.
 *
 * <p>
 * Set <tt>*properties</tt> to RT_NULL.<br>
 * Does nothing if <tt>*properties</tt> is RT_NULL.
 * </p>
 */
RT_API rt_s rt_properties_free(struct rt_hash_table_entry **properties);

#endif /* RT_PROPERTIES_H */

#ifndef RT_PROPERTIES_H
#define RT_PROPERTIES_H

#include "layer000/rt_types.h"
#include "layer004/rt_encoding.h"
#include "layer005/rt_hash_table.h"

/**
 * @file
 * See wikipedia for a description of the properties file format.
 *
 * <p>
 * Unlike the specification, a backslash at an end of line means that the line continues at the first non blank/EOL character.<br>
 * It is due to this implementation that adds end of line characters to fill gaps due to characters removals.
 * </p>
 *
 * <p>
 * Spaces before and after the key are ignored.<br>
 * Spaces before the value are ignored. Trailing spaces are not.
 * </p>
 */

#define RT_PROPERTIES_GET_HEADER(PROPERTIES) (struct rt_properties_header*)PROPERTIES - 1

struct rt_properties_header {
	const rt_char *file_path;
	const rt_char *buffer;
	struct rt_hash_table_header hash_table_header;
};

/**
 * Create an hash table loading given <tt>file_path</tt>.<br>
 * Keys and values are stored into <tt>buffer</tt>.
 *
 * @param initial_capacity Should be at least twice the initial number of entries and must be a power of two.
 */
rt_s rt_properties_create(struct rt_hash_table_entry **properties, const rt_char *file_path, enum rt_encoding encoding, rt_un initial_capacity, struct rt_heap *heap);

/**
 * Create an hash table parsing given <tt>str</tt>.
 *
 * <p>
 * The <tt>file_path</tt> and <tt>buffer</tt> fields are initialized to RT_NULL.
 * </p>
 *
 * @param str Will be modified by this function.
 * @param initial_capacity Should be at least twice the initial number of entries and must be a power of two.
 */
rt_s rt_properties_create_from_str(struct rt_hash_table_entry **properties, rt_char *str, rt_un initial_capacity, struct rt_heap *heap);

/**
 * Free the hash table and, if not RT_NULL, the buffer.
 *
 * <p>
 * Set <tt>*properties</tt> to RT_NULL.<br>
 * Does nothing if <tt>*properties</tt> is RT_NULL.
 * </p>
 */
rt_s rt_properties_free(struct rt_hash_table_entry **properties);

#endif /* RT_PROPERTIES_H */

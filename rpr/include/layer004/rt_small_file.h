#ifndef RT_SMALL_FILE_H
#define RT_SMALL_FILE_H

#include "layer000/rt_types.h"

#include "layer003/rt_heap.h"

/**
 * @file
 * Read/write from/to a buffer into/from a file.
 *
 * <p>
 * For obvious memory concerns, these functions must be used on small files only.
 * </p>
 */

enum rt_small_file_mode {
	/* Open an existing or create a new file and append data into it. */
	RT_SMALL_FILE_MODE_APPEND,
	/* Remove an existing file if it exists and create a new one. */
	RT_SMALL_FILE_MODE_TRUNCATE,
	/* Create a new file. If the file already exists, the function fails. */
	RT_SMALL_FILE_MODE_NEW
};

/**
 * Load a file in a buffer.
 *
 * <p>
 * A zero terminating character is put after the file content.<br>
 * As a result, the size of the buffer is the size of the file + 1.
 * </p>
 *
 * <p>
 * Beware that some unicode files starts with a BOM (Byte Order Mark).<br>
 * For UTF-8, this is not really a Byte Order Mark because there is no endianness issue with this encoding.<br>
 * It is 3 bytes that indicates that the file is encoded in UTF-8 (0xEF, 0xBB, 0xBF).<br>
 * For UTF-16, the size of the BOM is 2 bytes and it indicates the endianness.<br>
 * 0xFF, 0xFE for little endian (Used by Windows).<br>
 * 0xFE, 0xFF for big endian.
 * </p>
 */
rt_s rt_small_file_read(const rt_char *file_path, rt_char8 *buffer, rt_un buffer_capacity, void **heap_buffer, rt_un *heap_buffer_capacity, rt_char8 **output, rt_un *output_size, struct rt_heap *heap);

/**
 * Write the content of a buffer into a file.
 */
rt_s rt_small_file_write(const rt_char *file_path, enum rt_small_file_mode mode, const rt_char8 *data, rt_un bytes_to_write);

#endif /* RT_SMALL_FILE_H */

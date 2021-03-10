#ifndef RT_FILE_H
#define RT_FILE_H

#include "layer000/rt_types.h"
#include "layer002/rt_io_device.h"

/**
 * @file
 * Simple file management.
 */

struct rt_file
{
	struct rt_io_device io_device;
};

enum rt_file_mode {
	/* Open the file for read only access. Fails if the file does not exist. */
	RT_FILE_MODE_READ,
	/* Open an existing or create a new file for read/write access. */
	RT_FILE_MODE_READ_WRITE,
	/* Remove an existing file if it exists and create a new one with read/write access. */
	RT_FILE_MODE_TRUNCATE,
	/* Create a new file with read/write access. If the file already exists, the function fails. */
	RT_FILE_MODE_NEW
};

enum rt_file_position {
	RT_FILE_POSITION_BEGIN,
	RT_FILE_POSITION_CURRENT,
	RT_FILE_POSITION_END
};

/**
 * Open the specified file.<br>
 * Use <tt>rt_io_device_free</tt> to close the file.
 *
 * <p>
 * The created file is not inheritable.
 * </p>
 */
rt_s rt_file_create(struct rt_file *file, const rt_char *file_path, enum rt_file_mode mode);

/**
 * Compute the size of an open file.
 *
 * @return RT_FAILED in case of error (And the file pointer might be corrupted).
 */
rt_s rt_file_get_size(struct rt_file *file, rt_un64 *file_size);

rt_s rt_file_set_pointer(struct rt_file *file, rt_n64 offset, enum rt_file_position position);

rt_s rt_file_get_pointer(struct rt_file *file, rt_un64 *offset);

#endif /* RT_FILE_H */

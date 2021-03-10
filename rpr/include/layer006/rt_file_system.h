#ifndef RT_FILE_SYSTEM_H
#define RT_FILE_SYSTEM_H

#include "layer000/rt_types.h"

/**
 *
 * <p>
 * This function fails if:
 * </p>
 * <ul>
 * <li>The parent directory does not exist.</li>
 * <li>The directory already exists.</li>
 * </ul>
 */
rt_s rt_file_system_create_dir(const rt_char *path);

/**
 * Create all necessary directories to create given <tt>path</tt>.
 */
rt_s rt_file_system_create_dirs(const rt_char *path);

/**
 *
 * <p>
 * The directory must exist and be empty.
 * </p>
 */
rt_s rt_file_system_delete_dir(const rt_char *path);

/**
 * Attempt to delete the directory and check error code in case of error.<br>
 * Ignore the error if the directory has not been found.
 *
 * <p>
 * The directory must be empty.
 * </p>
 */
rt_s rt_file_system_delete_dir_if_exists(const rt_char *path);

#endif /* RT_FILE_SYSTEM_H */

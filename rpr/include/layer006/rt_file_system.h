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
rt_s rt_file_system_create_dir(const rt_char *dir_path);

/**
 * Create all necessary directories to create given <tt>dir_path</tt>.
 */
rt_s rt_file_system_create_dirs(const rt_char *dir_path);

/**
 *
 * <p>
 * The directory must exist and be empty.
 * </p>
 */
rt_s rt_file_system_delete_dir(const rt_char *dir_path);

/**
 * Attempt to delete the directory and check error code in case of error.<br>
 * Ignore the error if the directory has not been found.
 *
 * <p>
 * The directory must be empty.
 * </p>
 */
rt_s rt_file_system_delete_dir_if_exists(const rt_char *dir_path);


/**
 * Delete a file (not a directory).
 *
 * <p>
 * The file must exist.
 * </p>
 */
rt_s rt_file_system_delete_file(const rt_char *file_path);

/**
 * Attempt to delete the file and check error code in case of error.<br>
 * Ignore the error if the file has not been found.
 */
rt_s rt_file_system_delete_file_if_exists(const rt_char *file_path);

/**
 * If <tt>truncate</tt> is true and corresponding file exists, it is replaced by an empty one.<br>
 * If <tt>truncate</tt> is false, there is an error if the file already exists.
 */
rt_s rt_file_system_create_empty_file(const rt_char *file_path, rt_b truncate);

/**
 * Delete given directory and its content, recursively.
 *
 * <p>
 * The directory can exist or not.
 * </p>
 */
rt_s rt_file_system_delete_dir_recursively(const rt_char *dir_path);

#endif /* RT_FILE_SYSTEM_H */

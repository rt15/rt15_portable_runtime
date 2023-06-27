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

/**
 * Rename a directory.
 *
 * <p>
 * This function fails if a directory or a file already exists with <tt>destination_dir_name</tt> in the same directory.
 * </p>
 *
 * @param destination_dir_name Directory name.
 */
rt_s rt_file_system_rename_dir(const rt_char *source_dir_path, const rt_char *destination_dir_name);

/**
 * Find out the size of a file without opening it.
 *
 * <p>
 * Under Windows, the size of a directory is zero.<br>
 * Under Linux, the size of a directory is the size of its metadata, not the size of its content.
 * </p>
 */
rt_s rt_file_system_get_file_size(const rt_char *file_path, rt_un64 *file_size);

/**
 * <p>
 * This function fails if the destination already exists.
 * </p>
 */
rt_s rt_file_system_copy_file(const rt_char *source_file_path, const rt_char *destination_file_path);

/**
 * Move a file.
 *
 * <p>
 * This function fails if the destination already exists.
 * </p>
 *
 * @param source_file_path Current file path.
 * @param destination_file_path Expected new file path (Must not be a directory).
 */
rt_s rt_file_system_move_file(const rt_char *source_file_path, const rt_char *destination_file_path);

/**
 * Rename a file.
 *
 * <p>
 * This function fails if a file already exists with <tt>destination_file_name</tt> in the same directory.
 * </p>
 *
 * @param destination_file_name File name.
 */
rt_s rt_file_system_rename_file(const rt_char *source_file_path, const rt_char *destination_file_name);

#endif /* RT_FILE_SYSTEM_H */

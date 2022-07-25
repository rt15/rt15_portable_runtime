#ifndef RT_TEMP_FILE_H
#define RT_TEMP_FILE_H

#include "layer000/rt_types.h"
#include "layer003/rt_file.h"

/**
 * @file
 * Temporary files creation.
 *
 * <p>
 * As the file is opened, there is no risk of parallel conflict that could occur if a path was returned instead.
 * </p>
 *
 * <p>
 * Under Windows, up to the first three characters of the prefix are used in the file name.
 * </p>
 */

/**
 * Create a temporary file in the temporary directory.
 *
 * <p>
 * The created file is not inheritable.
 * </p>
 *
 * @param buffer Receive the path to the file so the caller can delete the file.
 * @param buffer_capacity Must be RT_FILE_PATH_SIZE.
 */
rt_s rt_temp_file_create(struct rt_file *file, const rt_char *prefix, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * Create a temporary file in given path.
 *
 * <p>
 * The created file is not inheritable.
 * </p>
 *
 * @param parent_path Directory in which the temporary file is created.
 * @param buffer Receive the path to the file so the caller can delete the file.
 * @param buffer_capacity Must be RT_FILE_PATH_SIZE.
 */
rt_s rt_temp_file_create_with_parent_path(struct rt_file *file, const rt_char *prefix, const rt_char *parent_path, rt_un parent_path_size, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

#endif /* RT_TEMP_FILE_H */

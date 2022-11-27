#ifndef ZZ_UTILS_H
#define ZZ_UTILS_H

#include <rpr.h>

/**
 * Retrieve the tmp directory (the one in samples/tests) path.
 */
rt_s zz_get_tmp_dir(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * Retrieve the test_resources directory (the one in samples/tests) path.
 */
rt_s zz_get_test_resources_dir(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * If the file already exists, it is truncated.
 */
rt_s zz_create_file(const rt_char *file_path, const rt_char8 *str);

/**
 * <p>
 * The size of the file must be smaller than RT_CHAR_BIG_STRING_SIZE.
 * </p>
 */
rt_s zz_check_file_content(const rt_char *file_path, const rt_char8 *expected);

/**
 * Return true if <tt>string1</tt> is equal to <tt>string2</tt> or if they are both null.
 */
rt_b zz_char_equals_with_nulls(const rt_char *string1, const rt_char *string2);

/**
 * Read <tt>file_path</tt> and decode it according to <tt>encoding</tt>.
 */
rt_s zz_read_text_file(const rt_char *file_path, enum rt_encoding encoding, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

#endif /* ZZ_UTILS_H */

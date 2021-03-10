#ifndef ZZ_UTILS_H
#define ZZ_UTILS_H

#include <rpr.h>

/**
 * Retrieve the tmp directory (the one in samples/tests) path.
 */
rt_s zz_get_tmp_dir(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

#endif /* ZZ_UTILS_H */

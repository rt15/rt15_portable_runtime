#ifndef RT_HASH_H
#define RT_HASH_H

#include "layer000/rt_types.h"

typedef rt_s (*rt_hash_callback_t)(const rt_char8 *data, rt_un data_size, void *context, rt_un *hash);

/**
 * Compute a non-cryptographic hash of given data.
 *
 * <p>
 * The algorithm is FNV-1a.
 * </p>
 */
rt_un rt_hash(const rt_char8 *data, rt_un data_size);

rt_s rt_hash_default_callback(const rt_char8 *data, rt_un data_size, void *context, rt_un *hash);

#endif /* RT_HASH_H */

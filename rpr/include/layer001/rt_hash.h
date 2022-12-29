#ifndef RT_HASH_H
#define RT_HASH_H

#include "layer000/rt_types.h"

typedef rt_s (*rt_hash_callback_t)(const void *data, rt_un data_size, void *context, rt_un *hash);

#endif /* RT_HASH_H */

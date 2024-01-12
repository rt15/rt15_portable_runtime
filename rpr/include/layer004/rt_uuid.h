#ifndef RT_UUID_H
#define RT_UUID_H

#include "layer000/rt_types.h"

struct rt_uuid {
	rt_n32 data1;
	rt_un16 data2;
	rt_un16 data3;
	rt_uchar8 data4[8];
};

RT_API rt_s rt_uuid_create(struct rt_uuid *uuid);

RT_API rt_s rt_uuid_parse(struct rt_uuid *uuid, const rt_char *str);

/**
 *
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
RT_API rt_s rt_uuid_append(struct rt_uuid *uuid, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

RT_API rt_s rt_uuid_parse8(struct rt_uuid *uuid, const rt_char8 *str);

/**
 *
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
RT_API rt_s rt_uuid_append8(struct rt_uuid *uuid, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size);

#endif /* RT_UUID_H */

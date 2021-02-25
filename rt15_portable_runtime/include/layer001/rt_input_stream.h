#ifndef RT_INPUT_STREAM_H
#define RT_INPUT_STREAM_H

#include "layer000/rt_types.h"

struct rt_input_stream;

typedef rt_s (*rt_input_stream_read_t)(struct rt_input_stream *input_stream, rt_char8 *buffer, rt_un bytes_to_read, rt_un *bytes_read);

struct rt_input_stream {
	rt_input_stream_read_t read;
};

#endif /* RT_INPUT_STREAM_H */

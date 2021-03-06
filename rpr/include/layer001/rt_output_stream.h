#ifndef RT_OUTPUT_STREAM_H
#define RT_OUTPUT_STREAM_H

#include "layer000/rt_types.h"

struct rt_output_stream;

typedef rt_s (*rt_output_stream_write_t)(struct rt_output_stream *output_stream, const rt_char8 *data, rt_un bytes_to_write);

struct rt_output_stream {
	rt_output_stream_write_t write;
};

#endif /* RT_OUTPUT_STREAM_H */

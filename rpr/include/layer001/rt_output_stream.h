#ifndef RT_OUTPUT_STREAM_H
#define RT_OUTPUT_STREAM_H

#include "layer000/rt_types.h"

struct rt_output_stream;

/**
 * @param bytes_to_write Must be less than 4 GB under Windows.
 */
typedef rt_s (*rt_output_stream_write_t)(struct rt_output_stream *output_stream, const rt_char8 *data, rt_un bytes_to_write);

typedef rt_s (*rt_output_stream_flush_t)(struct rt_output_stream *output_stream);

struct rt_output_stream {
	rt_output_stream_write_t write;
	/* Flush user space buffers if any. Does not flush at kernel level. */
	rt_output_stream_flush_t flush;
};

#endif /* RT_OUTPUT_STREAM_H */

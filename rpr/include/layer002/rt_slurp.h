#ifndef RT_SLURP_H
#define RT_SLURP_H

#include "layer000/rt_types.h"
#include "layer001/rt_input_stream.h"
#include "layer001/rt_output_stream.h"

/**
 * Write the content of <tt>input_stream</tt> into <tt>output_stream</tt>.
 *
 * @param buffer Temporary area used to copy chunks of data from one stream to the other.
 */
rt_s rt_slurp(struct rt_input_stream *input_stream, struct rt_output_stream *output_stream, rt_char8 *buffer, rt_un buffer_capacity);

#endif /* RT_SLURP_H */

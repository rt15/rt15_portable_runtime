#ifndef ZZ_UTILS_H
#define ZZ_UTILS_H

#include <rpr.h>

rt_s zz_start_chrono(struct rt_chrono *chrono);

rt_s zz_stop_chrono(rt_char8 *identifier, struct rt_chrono *chrono, struct rt_output_stream *output_stream);

#endif /* ZZ_UTILS_H */

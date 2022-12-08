#ifndef RT_SELECT_H
#define RT_SELECT_H

#include "layer000/rt_types.h"
#include "layer006/rt_socket.h"

/**
 * @file
 *
 *
 * <p>
 * Under Windows, only socket file descriptors are supported.
 * </p>
 */

#ifdef RT_DEFINE_WINDOWS
#define RT_SELECT_MAX_ITEMS 160
#else
#define RT_SELECT_MAX_ITEMS 1024
#endif

struct rt_select_item {
#ifdef RT_DEFINE_WINDOWS
	rt_un file_descriptor;
#else
	rt_n32 file_descriptor;
#endif
	rt_b ready;
};

/**
 *
 * <p>
 * The size of each individual array must be less or equal to <tt>RT_SELECT_MAX_ITEMS</tt>.
 * </p>
 *
 * @param read_items Can be RT_NULL.
 * @param read_items_size Must be zero if read_items is RT_NULL.
 * @param write_items Can be RT_NULL.
 * @param write_items_size Must be zero if write_items is RT_NULL.
 * @param except_items Can be RT_NULL.
 * @param except_items_size Must be zero if except_items is RT_NULL.
 * @param timeout_milliseconds If zero, <tt>rt_select</tt> can wait indefinitely.
 * @param ready_items_count Out parameter. Optional. Total number of ready file descriptor.
 */
rt_s rt_select(struct rt_select_item *read_items,  rt_un read_items_size,
	       struct rt_select_item *write_items, rt_un write_items_size,
	       struct rt_select_item *except_items, rt_un except_items_size,
	       rt_un timeout_milliseconds, rt_un *ready_items_count);

rt_s rt_select_wait_for_ready_to_read(struct rt_socket *socket, rt_un timeout_milliseconds);

rt_s rt_select_wait_for_ready_to_write(struct rt_socket *socket, rt_un timeout_milliseconds);

#endif /* RT_SELECT_H */

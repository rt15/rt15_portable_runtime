#ifndef RT_RING_H
#define RT_RING_H

#include "layer000/rt_types.h"
#include "layer004/rt_array.h"

/**
 * @file
 * Ring/FIFO/queue.
 * 
 * <p>
 * The capacity is fixed so <tt>rt_ring_new_item</tt> can fail.
 * </p>
 * 
 * <p>
 * This implementation is not thread safe.
 * </p>
 */

struct rt_ring {
	struct rt_array array;
	rt_un head_index; /* Index of the next free item. */
	rt_un tail_index; /* Index of the oldest item. */
	rt_un size; /* Number of items currently in the ring. */
};

RT_API rt_s rt_ring_create(struct rt_ring *ring, rt_un capacity, rt_un item_size, struct rt_heap *heap);

/**
 * Prepare a new item.
 *
 * <p>
 * Fails if the ring is full.<br>
 * The data in <tt>item</tt> has to be set before the corresponding call to <tt>rt_ring_pop_item</tt>.<br>
 * Calls to <tt>rt_ring_new_item</tt> cannot corrupt the data pointed to by <tt>item</tt>: the ring will be full before that.
 * </p>
 */
RT_API rt_s rt_ring_new_item(struct rt_ring *ring, void **item);

/**
 * Retrieve the address of the oldest item and consider it consumed.
 *
 * <p>
 * Fails if the ring is empty.<br>
 * The data pointed to by <tt>item</tt> can get corrupted by subsequent calls to <tt>rt_ring_new_item</tt>.
 * </p>
 */
RT_API rt_s rt_ring_pop_item(struct rt_ring *ring, void **item);

RT_API rt_s rt_ring_free(struct rt_ring *ring);

#endif /* RT_RING_H */

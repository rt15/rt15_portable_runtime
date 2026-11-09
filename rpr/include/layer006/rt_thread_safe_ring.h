#ifndef RT_THREAD_SAFE_RING_H
#define RT_THREAD_SAFE_RING_H

#include "layer000/rt_types.h"
#include "layer002/rt_critical_section.h"
#include "layer003/rt_condition_variable.h"
#include "layer005/rt_ring.h"

/**
 * @file
 * Thread safe ring/FIFO/queue.
 *
 * <p>
 * <tt>rt_thread_safe_ring_push</tt> blocks on a full ring.<br>
 * <tt>rt_thread_safe_ring_pop</tt> blocks on an empty ring unless it is closed.
 * </p>
 */

struct rt_thread_safe_ring {
	struct rt_ring ring;
	struct rt_critical_section critical_section;
	struct rt_condition_variable not_full_condition_variable;
	struct rt_condition_variable not_empty_condition_variable;
	rt_b closed;
};

RT_API rt_s rt_thread_safe_ring_create(struct rt_thread_safe_ring *thread_safe_ring, rt_un capacity, rt_un item_size, struct rt_heap *heap);

/**
 * Push a new item in the ring.
 *
 * <p>
 * Blocks if the ring is full.<br>
 * Fails if the ring is closed.
 * </p>
 */
RT_API rt_s rt_thread_safe_ring_push(struct rt_thread_safe_ring *thread_safe_ring, const void *item);

/**
 * Pop the oldest item from the ring.
 *
 * <p>
 * Blocks if the ring is empty unless the ring is closed.
 * </p>
 *
 * @param closed Set to RT_TRUE (and item is left untouched) when the ring is closed and has no items left.
 */
RT_API rt_s rt_thread_safe_ring_pop(struct rt_thread_safe_ring *thread_safe_ring, void *item, rt_b *closed);

/**
 * Must be called to either cancel the processing or safely complete it.
 *
 * <p>
 * Wakes all blocked threads.<br>
 * Further calls to <tt>rt_thread_safe_ring_push</tt> will fail.<br>
 * Remaining items can be consumed by <tt>rt_thread_safe_ring_pop</tt> until *closed is RT_TRUE.
 * </p>
 */
RT_API rt_s rt_thread_safe_ring_close(struct rt_thread_safe_ring *thread_safe_ring);

/**
 * All threads must have left push and pop before calling this function.
 */
RT_API rt_s rt_thread_safe_ring_free(struct rt_thread_safe_ring *thread_safe_ring);

#endif /* RT_THREAD_SAFE_RING_H */

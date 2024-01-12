#ifndef RT_FAST_INITIALIZATION_H
#define RT_FAST_INITIALIZATION_H

#include "layer000/rt_types.h"

/**
 * @file
 * Help to manage fast lazy initialization.
 *
 * <p>
 * Sometimes, in a multithreaded environment, some initialization code must be executed once by the first thread that reach this code.<br>
 * Functions provided in this header ensure that only the first thread that executes rt_fast_initialization_is_required will return RT_TRUE.<br>
 * Then all others threads executing rt_fast_initialization_is_required will wait in an infinite loop until the first thread calls rt_fast_initialization_notify_done, then RT_FALSE is returned.<br>
 * Beware that the infinite loop will consume a lot of CPU.<br>
 * Use <tt>rt_critical_section</tt> for time consuming initialization code.<br>
 * Be sure to initialize the rt_fast_initialization structure with RT_FAST_INITIALIZATION_STATIC_INIT.
 * </p>
 *
 * <p>
 * A big advantage of fast initialization is that there is no need to create/free them.
 * </p>
 */

struct rt_fast_initialization {
	rt_un initialized;
	rt_un lock;
};

/**
 * Must be used to initialize a <tt>rt_fast_initialization</tt> structure.
 */
#define RT_FAST_INITIALIZATION_STATIC_INIT {0, 0}

RT_API rt_b rt_fast_initialization_is_required(struct rt_fast_initialization *fast_initialization);
RT_API void rt_fast_initialization_notify_done(struct rt_fast_initialization *fast_initialization);

/**
 * After using <tt>rt_fast_initialization_is_required</tt>/<tt>rt_fast_initialization_notify_done</tt>, we might have some clean-up to execute later on.<br>
 * This function can be used to perform this clean-up if it returns RT_TRUE.
 */
RT_API rt_b rt_fast_initialization_is_done(struct rt_fast_initialization *fast_initialization);

#endif /* RT_FAST_INITIALIZATION_H */

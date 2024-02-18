#ifndef RT_SLEEP_H
#define RT_SLEEP_H

#include "layer000/rt_types.h"

/**
 * @file
 * Allow to pause a thread or request a context switch.
 */

/**
 * Request a context switch.
 *
 * <p>
 * Ask the scheduler to execute another thread.
 * </p>
 */
RT_API void rt_sleep_yield(void);

RT_API void rt_sleep_sleep(rt_un32 milliseconds);

#endif /* RT_SLEEP_H */

#ifndef RT_OS_VERSION_H
#define RT_OS_VERSION_H

#include "layer000/rt_types.h"

/**
 * Returns the major version and optionally the minor and patch versions.
 *
 * <p>
 * Under Windows, patch is the major SP version, 3 for SP3.
 * </p>
 *
 * <p>
 * Under Linux, it is the kernel version, not the distribution version.
 * </p>
 *
 * <pre>
 * Windows 10                10.0 (6.2 if the application is not manifested)
 * Windows Server 2016       10.0 (6.2 if the application is not manifested)
 * Windows 8.1                6.3 (6.2 if the application is not manifested)
 * Windows Server 2012 R2     6.3 (6.2 if the application is not manifested)
 * Windows 8                  6.2
 * Windows Server 2012        6.2
 * Windows 7                  6.1
 * Windows Server 2008 R2     6.1
 * Windows Server 2008        6.0
 * Windows Vista              6.0
 * Windows Server 2003 R2     5.2
 * Windows Server 2003        5.2
 * Windows XP 64-Bit Edition  5.2
 * Windows XP                 5.1
 * Windows 2000               5.0
 * </pre>
 *
 * @param major Cannot be RT_NULL.
 * @param minor Can be RT_NULL.
 * @param patch Can be RT_NULL.
 */
RT_API rt_s rt_os_version_get(rt_un *major, rt_un *minor, rt_un *patch);

RT_API rt_s rt_os_version_is_greater_or_equal_to(rt_un major, rt_un minor, rt_un patch, rt_b *result);

#endif /* RT_OS_VERSION_H */

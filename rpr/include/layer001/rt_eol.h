#ifndef RT_EOL_H
#define RT_EOL_H

#include "layer000/rt_types.h"

/**
 * @file
 * End of lines.
 *
 * <p>
 * An end of line is either:
 * </p>
 * <ul>
 * <li>LF</li>
 * <li>CRLF</li>
 * <li>CR</li>
 * </ul>
 *
 * <p>
 * LFLF, CRCR and LFCR are considered to be two end of lines.
 * </p>
 */

enum rt_eol {
	RT_EOL_NONE,
	RT_EOL_LF,
	RT_EOL_CRLF,
	RT_EOL_CR
};

#endif /* RT_EOL_H */

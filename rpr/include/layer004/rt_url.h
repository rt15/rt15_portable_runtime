#ifndef RT_URL_H
#define RT_URL_H

#include "layer000/rt_types.h"

/**
 * @file
 * Urls management.
 *
 * <pre>
 * scheme:[//[user:password@]host[:port]][/]path[?query][#fragment]
 * </pre>
 */

struct rt_url_info {
	const rt_char *scheme;		/* Mandatory. */
	rt_un scheme_size;
	const rt_char *user;		/* Optional. */
	rt_un user_size;
	const rt_char *password;	/* Optional. */
	rt_un password_size;
	const rt_char *host;		/* Mandatory. */
	rt_un host_size;
	rt_un port;			/* Optional, RT_TYPE_MAX_UN if not set. */
	const rt_char *path;		/* Mandatory but can be empty. */
	rt_un path_size;
	const rt_char *query;		/* Optional. */
	rt_un query_size;
	const rt_char *fragment;	/* Optional. */
	rt_un fragment_size;
};

/**
 * Parse given <tt>url</tt> to fill <tt>url_info</tt>.
 */
RT_API rt_s rt_url_parse(const rt_char *url, struct rt_url_info *url_info);

#endif /* RT_URL_H */

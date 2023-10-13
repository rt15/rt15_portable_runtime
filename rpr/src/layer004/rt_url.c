#include "layer004/rt_url.h"

#include "layer002/rt_error.h"
#include "layer003/rt_char.h"

rt_s rt_url_parse(const rt_char *url, struct rt_url_info *url_info)
{
	rt_b authentication;
	rt_un i;
	rt_un j;
	rt_s ret;

	i = 0;
	while (url[i] != _R(':')) {
		if (RT_LIKELY(((url[i] >= _R('a')) && (url[i] <= _R('z'))) ||
			      ((url[i] >= _R('A')) && (url[i] <= _R('Z'))) ||
			      ((url[i] >= _R('0')) && (url[i] <= _R('9'))) ||
			       (url[i] == _R('+')) ||
			       (url[i] == _R('-')) ||
			       (url[i] == _R('.'))))
		{
			i++;
		} else {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		}
	}

	url_info->scheme = url;
	url_info->scheme_size = i;

	/* Skip ':'. */
	i++;

	/* Read "//". */
	for (j = 0; j < 2; j++) {
		if (RT_UNLIKELY(url[i] != _R('/'))) {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		}
		i++;
	}

	/* Search for authentication part user:password@. Stop at first slash or end of string. */
	authentication = RT_FALSE;
	j = i;
	while (url[j] && url[j] != _R('/')) {
		if (url[j] == _R('@')) {
			authentication = RT_TRUE;
			break;
		}
		j++;
	}

	if (authentication) {
		j = i;
		while (url[j] != _R(':') && url[j] != _R('@')) {
			j++;
		}
		url_info->user = &url[i];
		url_info->user_size = j - i;

		i = j;
		if (url[i] == _R(':')) {
			/* Skip ':'. */
			i++;

			j = i;
			while (url[j] != _R('@')) {
				j++;
			}
			url_info->password = &url[i];
			url_info->password_size = j - i;
		} else {
			/* No password. */
			url_info->password = RT_NULL;
			url_info->password_size = 0;
		}

		i = j;
		/* Skip '@'. */
		i++;
	} else {
		url_info->user = RT_NULL;
		url_info->user_size = 0;
		url_info->password = RT_NULL;
		url_info->password_size = 0;
	}

	j = i;

	/* IPv6 addresses are enclosed in brackets. */
	if (url[j] == _R('[')) {
		/* Skip opening bracket. */
		j++;
		while (url[j] && url[j] != _R(']')) {
			j++;
		}
		/* No closing bracket. */
		if (RT_UNLIKELY(!url[j])) {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		}
		/* Skip closing bracket. */
		j++;
	} else {
		while (url[j] && url[j] != _R(':') && url[j] != _R('/')) {
			j++;
		}
	}
	url_info->host = &url[i];
	url_info->host_size = j - i;

	i = j;

	/* Port? */
	if (url[i] == _R(':')) {
		/* Skip ':'. */
		i++;
		j = i;
		while (url[j] && url[j] != _R('/')) {
			j++;
		}

		if (RT_UNLIKELY(!rt_char_convert_to_un_with_size(&url[i], j - i, &url_info->port)))
			goto error;
		i = j;
	} else {
		url_info->port = RT_TYPE_MAX_UN;
	}

	/* Initialize remaining fields. */
	url_info->path = RT_NULL;
	url_info->path_size = 0;
	url_info->query = RT_NULL;
	url_info->query_size = 0;
	url_info->fragment = RT_NULL;
	url_info->fragment_size = 0;

	if (url[i]) {
		/* Path is "optional" but if there is a path, there must be a slash. */
		if (RT_UNLIKELY(url[i] != _R('/'))) {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		}

		/* Skip '/'. */
		i++;
		j = i;

		while (url[j] && url[j] != _R('?') && url[j] != _R('#')) {
			j++;
		}
		url_info->path = &url[i];
		url_info->path_size = j - i;

		i = j;

		/* Is there a query? */
		if (url[i] == _R('?')) {
			/* Skip '?'. */
			i++;
			j = i;

			while (url[j] && url[j] != _R('#')) {
				j++;
			}
			url_info->query = &url[i];
			url_info->query_size = j - i;

			i = j;
		}

		/* Is there a fragment? */
		if (url[i] == _R('#')) {
			/* Skip '#'. */
			i++;
			j = i;

			while (url[j]) {
				j++;
			}
			url_info->fragment = &url[i];
			url_info->fragment_size = j - i;
		}
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

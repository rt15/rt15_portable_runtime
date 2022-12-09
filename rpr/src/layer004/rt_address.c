#include "layer004/rt_address.h"

#include "layer001/rt_memory.h"
#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer003/rt_char.h"

void rt_address_create_ipv4_loopback(struct rt_address_ipv4 *address)
{
	address->u.address = htonl(INADDR_LOOPBACK);
}

void rt_address_create_ipv4_any(struct rt_address_ipv4 *address)
{
	address->u.address = htonl(INADDR_ANY);
}

rt_s rt_address_create_ipv4(struct rt_address_ipv4 *address, const rt_char *str)
{
	rt_uchar8 *bytes = (rt_uchar8*)address;
	const rt_char *in_str = str;
	rt_un separator_index;
	rt_un value;
	rt_un i;
	rt_s ret;

	/* Parse the first three numbers, all ending with a dot. */
	for (i = 0; i < 3; i++) {
		separator_index =  rt_char_search_char(in_str, _R('.'));
		if (separator_index == RT_TYPE_MAX_UN)
			goto error;
		if (!rt_char_convert_to_un_with_size(in_str, separator_index, &value))
			goto error;
		if (value > RT_TYPE_MAX_UCHAR8)
			goto error;
		bytes[i] = (rt_uchar8)value;

		in_str = &in_str[separator_index + 1];
	}

	/* Parse the last number, ending with terminating zero. */
	if (!rt_char_convert_to_un(in_str, &value))
		goto error;
	if (value > RT_TYPE_MAX_UCHAR8)
		goto error;
	bytes[3] = (rt_uchar8)value;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_address_create_ipv6(struct rt_address_ipv6 *address, const rt_char *str)
{
	rt_un16 *words = (rt_un16*)address;
	rt_char character;
	rt_un value;
	rt_un in_words = 0;
	rt_un zero_sequence_index = RT_TYPE_MAX_UN;
	rt_un i = 0;
	rt_un j;
	rt_s ret;

	while (RT_TRUE) {
		j = i;
		while (RT_TRUE) {
			character = str[j];
			if ((character < _R('0') || character > _R('9')) &&
			    (character < _R('a') || character > _R('f')))
				break;
			j++;
		}
		if (character == _R(':') || !character) {
			if (i == j) {
				/* Either two consecutive colons, or a colon at the beginning of the string. */
				if (zero_sequence_index != RT_TYPE_MAX_UN) {
					/* There should be one zeros sequence per address. */
					rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
					goto error;
				}
				zero_sequence_index = in_words;
				if (!j) {
					j++;
					if (str[j] != _R(':')) {
						/* If the first character is a colon, then the second character should be a colon too. */
						rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
						goto error;
					}
				}
				if ((character == ':') && (!str[j + 1]))
					break;
			} else {
				if (!rt_char_convert_hex_to_un_with_size(&str[i], j - i, &value))
					goto error;
				if (value > 0xFFFF) {
					rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
					goto error;
				}
				words[in_words] = RT_MEMORY_SWAP_BYTES16((rt_un16)value);
				in_words++;
			}
			if (!character)
				break;
		} else if (character == _R('.')) {
			/* The beginning of an IPv4 address. */

			/* Makes sure there is enough remaining space in the IPv6 address. */
			/* We need at least the words at indexes 6 and 7. */
			if (in_words > 6) {
				rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
				goto error;
			}

			if (!rt_address_create_ipv4((struct rt_address_ipv4*)&words[in_words], &str[i]))
				goto error;

			in_words += 2;
			break;
		} else {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		}
		i = j;
		i++;
	}

	if (zero_sequence_index != RT_TYPE_MAX_UN) {
		if (zero_sequence_index < in_words)
			RT_MEMORY_MOVE(&words[zero_sequence_index], &words[8 - (in_words - zero_sequence_index)], (in_words - zero_sequence_index) * sizeof(rt_un16));
		RT_MEMORY_ZERO(&words[zero_sequence_index], (8 - in_words) * sizeof(rt_un16));
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

void rt_address_create_ipv6_loopback(struct rt_address_ipv6 *address)
{
	*(struct in6_addr*)address = in6addr_loopback;
}

void rt_address_create_ipv6_any(struct rt_address_ipv6 *address)
{
	*(struct in6_addr*)address = in6addr_any;
}

rt_s rt_address_create_from_host_name(const rt_char *host_name, struct rt_address_ipv4 *ipv4_address, struct rt_address_ipv6 *ipv6_address, enum rt_address_family *address_family)
{
	int returned_value;
#ifdef RT_DEFINE_WINDOWS
	struct addrinfoW *address_info;
#else
	struct addrinfo *address_info;
#endif
	rt_b address_info_created = RT_FALSE;
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	returned_value = GetAddrInfo(host_name, RT_NULL, RT_NULL, &address_info);
	if (returned_value) {
		/* GetAddrInfoW returns an error code in case of error, zero otherwise. */
		SetLastError(returned_value);
		goto error;
	}
	address_info_created = RT_TRUE;
#else
	/* getaddrinfo returns 0 if it is successful. It returns an error code in case of error and set errno only if the error code is EAI_SYSTEM. */
	returned_value = getaddrinfo(host_name, RT_NULL, RT_NULL, &address_info);
	if (returned_value) {
		if (returned_value == EAI_SYSTEM) {
			goto error;
		} else {
			rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
			goto error;
		}
	}
	address_info_created = RT_TRUE;
#endif

	switch (address_info->ai_family) {
	case RT_ADDRESS_FAMILY_IPV4:
		RT_MEMORY_COPY(&((struct sockaddr_in*)address_info->ai_addr)->sin_addr, ipv4_address, sizeof(struct rt_address_ipv4));
		break;
	case RT_ADDRESS_FAMILY_IPV6:
		RT_MEMORY_COPY(&((struct sockaddr_in6*)address_info->ai_addr)->sin6_addr, ipv6_address, sizeof(struct rt_address_ipv6));
		break;
	default:
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto error;
	}
	*address_family = address_info->ai_family;

	ret = RT_OK;
free:
	if (address_info_created) {
		address_info_created = RT_FALSE;
#ifdef RT_DEFINE_WINDOWS
		/* FreeAddrInfo cannot fails. */
		FreeAddrInfo(address_info);
#else
		/* freeaddrinfo cannot fails. */
		freeaddrinfo(address_info);
#endif
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_address_append_ipv4(struct rt_address_ipv4 *address, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_uchar8 *bytes = (rt_uchar8*)address;
	rt_b first = RT_TRUE;
	rt_un i;
	rt_s ret;

	for (i = 0; i < 4; i ++) {
		if (first) {
			first = RT_FALSE;
		} else {
			if (!rt_char_append_char(_R('.'), buffer, buffer_capacity, buffer_size))
				goto error;
		}
		if (!rt_char_append_un(bytes[i], 10, buffer, buffer_capacity, buffer_size))
			goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_address_append_ipv6(struct rt_address_ipv6 *address, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_char *ipv6_str = &buffer[*buffer_size];
	rt_un16 *words = (rt_un16*)address;
	rt_b first = RT_TRUE;
	rt_char character;
	rt_un zeros_count;
	rt_un max_zeros = 1;
	rt_un best_index = 0;
	rt_un max_sequence_size;
	rt_un i;
	rt_un j;
	rt_s ret;

	if (!RT_MEMORY_COMPARE(address, "\0\0\0\0\0\0\0\0\0\0\377\377", 12)) {
		/* IPv4-mapped address. */

		if (!rt_char_append(_R("::ffff:"), 7, buffer, buffer_capacity, buffer_size))
			goto error;
		if (!rt_address_append_ipv4((struct rt_address_ipv4*)&((rt_char8*)address)[12], buffer, buffer_capacity, buffer_size))
			goto error;

	} else if (!RT_MEMORY_COMPARE(address, "\0\0\0\0\0\0\0\0\377\377\0\0", 12)) {
		/* IPv4-translated address. */

		if (!rt_char_append(_R("::ffff:0:"), 9, buffer, buffer_capacity, buffer_size))
			goto error;
		if (!rt_address_append_ipv4((struct rt_address_ipv4*)&((rt_char8*)address)[12], buffer, buffer_capacity, buffer_size))
			goto error;
	} else {
		/* Regular IPv6 address. */

		/* Naively write the address. */
		for (i = 0; i < 8; i ++) {
			if (first) {
				first = RT_FALSE;
			} else {
				if (!rt_char_append_char(_R(':'), buffer, buffer_capacity, buffer_size))
					goto error;
			}
			if (!rt_char_append_un(RT_MEMORY_SWAP_BYTES16(words[i]), 16, buffer, buffer_capacity, buffer_size))
				goto error;
		}

		/* Find the longest zeros sequence to replace it with two colons. */
		i = 0;
		while (ipv6_str[i]) {
			j = i;
			zeros_count = 0;
			if (ipv6_str[j] == _R('0')) {
				zeros_count++;
				j++;
				while (RT_TRUE) {
					character = ipv6_str[j];
					if (character == _R('0')) {
						zeros_count++;
					} else if (character != _R(':')) {
						break;
					}
					j++;
				}
			} else {
				/* Not a zero, skip until next word. */
				j++;
				while (RT_TRUE) {
					character = ipv6_str[j];
					if (!character) {
						break;
					} else if (character == _R(':')) {
						j++;
						break;
					}
					j++;
				}
			}
			if (zeros_count > max_zeros) {
				max_zeros = zeros_count;
				max_sequence_size = j - i;
				best_index = i;
			}
			i = j;
		}
		if (max_zeros > 1) {
			if (!best_index) {
				/* If it is the beginning of the string, we add a colon and update best_index and max_sequence_size to ignore it. */
				ipv6_str[best_index] = _R(':');
				best_index++;
				max_sequence_size--;
			}
			ipv6_str[best_index] = _R(':');
			RT_MEMORY_COPY(&ipv6_str[best_index + max_sequence_size], &ipv6_str[best_index + 1], (i - (best_index + max_sequence_size) + 1) * sizeof(rt_char));
			*buffer_size -= (max_sequence_size - 1);
		}

		rt_char_fast_lower(ipv6_str);
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

#include "layer004/rt_uuid.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer003/rt_char.h"
#include "layer003/rt_char8.h"

#ifdef RT_DEFINE_WINDOWS

rt_s rt_uuid_create(struct rt_uuid *uuid)
{
	RPC_STATUS status;
	rt_s ret;

	status = UuidCreate((UUID*)uuid);
	if (RT_UNLIKELY(status != RPC_S_OK && status != RPC_S_UUID_LOCAL_ONLY)) {
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto error;
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_uuid_parse(struct rt_uuid *uuid, const rt_char *str)
{
	rt_s ret;

	if (RT_UNLIKELY(UuidFromString((rt_char*)str, (UUID*)uuid)) != RPC_S_OK) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_uuid_append(struct rt_uuid *uuid, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_char *uuid_str;
	rt_b uuid_str_created = RT_FALSE;
	rt_un uuid_str_size;
	rt_s ret;

	if (RT_UNLIKELY(UuidToString((UUID*)uuid, &uuid_str) != RPC_S_OK)) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}
	uuid_str_created = RT_TRUE;

	uuid_str_size = rt_char_get_size(uuid_str);

	if (RT_UNLIKELY(!rt_char_append(uuid_str, uuid_str_size, buffer, buffer_capacity, buffer_size)))
		goto error;

	ret = RT_OK;
free:
	if (uuid_str_created) {
		uuid_str_created = RT_FALSE;
		if (RT_UNLIKELY(RpcStringFree(&uuid_str) != RPC_S_OK && ret)) {
			rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
			goto error;
		}
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_uuid_parse8(struct rt_uuid *uuid, const rt_char8 *str)
{
	rt_s ret;

	if (RT_UNLIKELY(UuidFromStringA((rt_uchar8*)str, (UUID*)uuid)) != RPC_S_OK) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_uuid_append8(struct rt_uuid *uuid, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_char8 *uuid_str;
	rt_b uuid_str_created = RT_FALSE;
	rt_un uuid_str_size;
	rt_s ret;

	if (RT_UNLIKELY(UuidToStringA((UUID*)uuid, (rt_uchar8**)&uuid_str) != RPC_S_OK)) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}
	uuid_str_created = RT_TRUE;

	uuid_str_size = rt_char8_get_size(uuid_str);

	if (RT_UNLIKELY(!rt_char8_append(uuid_str, uuid_str_size, buffer, buffer_capacity, buffer_size)))
		goto error;

	ret = RT_OK;
free:
	if (uuid_str_created) {
		uuid_str_created = RT_FALSE;
		if (RT_UNLIKELY(RpcStringFreeA((rt_uchar8**)&uuid_str) != RPC_S_OK && ret)) {
			rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
			goto error;
		}
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

#else

rt_s rt_uuid_create(struct rt_uuid *uuid)
{
	/* uuid_generate returns void. */
	uuid_generate((rt_uchar8*)uuid);

	return RT_OK;
}

rt_s rt_uuid_parse(struct rt_uuid *uuid, const rt_char *str)
{
	rt_s ret;

	if (RT_UNLIKELY(uuid_parse(str, (rt_uchar8*)uuid))) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_uuid_append(struct rt_uuid *uuid, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_s ret;

	if (RT_UNLIKELY(buffer_capacity - *buffer_size < 37)) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto error;
	}

	/* uuid_unparse_lower returns void */
	uuid_unparse_lower((rt_uchar8*)uuid, &buffer[*buffer_size]);

	*buffer_size += rt_char_get_size(&buffer[*buffer_size]);

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_uuid_parse8(struct rt_uuid *uuid, const rt_char8 *str)
{
	rt_s ret;

	if (RT_UNLIKELY(uuid_parse(str, (rt_uchar8*)uuid))) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_uuid_append8(struct rt_uuid *uuid, rt_char8 *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_s ret;

	if (RT_UNLIKELY(buffer_capacity - *buffer_size < 37)) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto error;
	}

	/* uuid_unparse_lower returns void */
	uuid_unparse_lower((rt_uchar8*)uuid, &buffer[*buffer_size]);

	*buffer_size += rt_char8_get_size(&buffer[*buffer_size]);

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

#endif

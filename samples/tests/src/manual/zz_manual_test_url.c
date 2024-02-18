#include <rpr.h>

static rt_s zz_manual_test_url_print_info_item(const rt_char *field_name, const rt_char *field_value, rt_un field_value_size)
{
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret;

	buffer_size = rt_char_get_size(field_name);
	if (RT_UNLIKELY(!rt_char_copy(field_name, buffer_size, buffer, RT_CHAR_HALF_BIG_STRING_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R(" = "), 3, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto error;

	if (field_value) {
		if (RT_UNLIKELY(!rt_char_append(field_value, field_value_size, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
			goto error;
	} else {
		if (RT_UNLIKELY(!rt_char_append(_R("null"), 4, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
			goto error;
	}

	if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto error;

	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_manual_test_url_print_info(struct rt_url_info *url_info)
{
	rt_char buffer[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret;

	if (RT_UNLIKELY(!zz_manual_test_url_print_info_item(_R("Scheme"),   url_info->scheme,   url_info->scheme_size)))   goto error;
	if (RT_UNLIKELY(!zz_manual_test_url_print_info_item(_R("User"),     url_info->user,     url_info->user_size)))     goto error;
	if (RT_UNLIKELY(!zz_manual_test_url_print_info_item(_R("Password"), url_info->password, url_info->password_size))) goto error;
	if (RT_UNLIKELY(!zz_manual_test_url_print_info_item(_R("Host"),     url_info->host,     url_info->host_size)))     goto error;

	if (url_info->port == RT_TYPE_MAX_UN) {
		if (RT_UNLIKELY(!zz_manual_test_url_print_info_item(_R("Port"), RT_NULL, 0)))
			goto error;
	} else {
		buffer_size = 0;
		if (RT_UNLIKELY(!rt_char_append_un(url_info->port, 10, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto error;
		if (RT_UNLIKELY(!zz_manual_test_url_print_info_item(_R("Port"), buffer, buffer_size))) goto error;
	}

	if (RT_UNLIKELY(!zz_manual_test_url_print_info_item(_R("Query"), url_info->query, url_info->query_size))) goto error;
	if (RT_UNLIKELY(!zz_manual_test_url_print_info_item(_R("Fragment"), url_info->fragment, url_info->fragment_size))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_manual_test_url_do(const rt_char *url)
{
	struct rt_url_info url_info;
	rt_s ret;

	if (RT_UNLIKELY(!rt_console_write_str(url))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str_with_size(_R("\n"), 1))) goto error;
	if (RT_UNLIKELY(!rt_url_parse(url, &url_info))) goto error;
	if (RT_UNLIKELY(!zz_manual_test_url_print_info(&url_info))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_manual_test_url(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_manual_test_url_do(_R("http://Jane:Doe@www.example.com:8888/chemin/d/acc%C3%A8s.php?q=req&q2=req2#signet")))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

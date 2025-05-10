#include <rpr.h>

static rt_s zz_test_url_check_info_item(const rt_char *url_info_item, rt_un url_info_item_size, const rt_char *expected_value)
{
	rt_s ret = RT_FAILED;

	if (url_info_item) {
		if (RT_UNLIKELY(!expected_value))
			goto end;

		if (RT_UNLIKELY(!rt_char_equals(url_info_item, url_info_item_size, expected_value, rt_char_get_size(expected_value))))
			goto end;
	} else {
		if (RT_UNLIKELY(expected_value))
			goto end;
		/* Ok, both are null. */
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_url_check_info(struct rt_url_info *url_info, const rt_char *expected_scheme, const rt_char *expected_user, const rt_char *expected_password, const rt_char *expected_host, rt_un expected_port, const rt_char *expected_path, const rt_char *expected_query, const rt_char *expected_fragment)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_url_check_info_item(url_info->scheme,   url_info->scheme_size,   expected_scheme)))   goto end;
	if (RT_UNLIKELY(!zz_test_url_check_info_item(url_info->user,     url_info->user_size,     expected_user)))     goto end;
	if (RT_UNLIKELY(!zz_test_url_check_info_item(url_info->password, url_info->password_size, expected_password))) goto end;
	if (RT_UNLIKELY(!zz_test_url_check_info_item(url_info->host,     url_info->host_size,     expected_host)))     goto end;
	if (RT_UNLIKELY(!zz_test_url_check_info_item(url_info->path,     url_info->path_size,     expected_path)))     goto end;
	if (RT_UNLIKELY(!zz_test_url_check_info_item(url_info->query,    url_info->query_size,    expected_query)))    goto end;
	if (RT_UNLIKELY(!zz_test_url_check_info_item(url_info->fragment, url_info->fragment_size, expected_fragment))) goto end;

	if (RT_UNLIKELY(url_info->port != expected_port)) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_url(void)
{
	struct rt_url_info url_info;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_url_parse(_R("http://Jane:Doe@www.example.com:8888/chemin/d/acc%C3%A8s.php?q=req&q2=req2#signet"), &url_info))) goto end;
	if (RT_UNLIKELY(!zz_test_url_check_info(&url_info, _R("http"), _R("Jane"), _R("Doe"), _R("www.example.com"), 8888, _R("chemin/d/acc%C3%A8s.php"), _R("q=req&q2=req2"), _R("signet")))) goto end;

	if (RT_UNLIKELY(!rt_url_parse(_R("http://www.example.com:8888/chemin/d/acc%C3%A8s.php?q=req&q2=req2"), &url_info))) goto end;
	if (RT_UNLIKELY(!zz_test_url_check_info(&url_info, _R("http"), RT_NULL, RT_NULL, _R("www.example.com"), 8888, _R("chemin/d/acc%C3%A8s.php"), _R("q=req&q2=req2"), RT_NULL))) goto end;

	if (RT_UNLIKELY(!rt_url_parse(_R("http://www.example.com/chemin/d/acc%C3%A8s.php?q=req&q2=req2"), &url_info))) goto end;
	if (RT_UNLIKELY(!zz_test_url_check_info(&url_info, _R("http"), RT_NULL, RT_NULL, _R("www.example.com"), RT_TYPE_MAX_UN, _R("chemin/d/acc%C3%A8s.php"), _R("q=req&q2=req2"), RT_NULL))) goto end;

	if (RT_UNLIKELY(!rt_url_parse(_R("http://[2607:f0d0:1002:0051:0000:0000:0000:0004]/chemin/d/acc%C3%A8s.php"), &url_info))) goto end;
	if (RT_UNLIKELY(!zz_test_url_check_info(&url_info, _R("http"), RT_NULL, RT_NULL, _R("[2607:f0d0:1002:0051:0000:0000:0000:0004]"), RT_TYPE_MAX_UN, _R("chemin/d/acc%C3%A8s.php"), RT_NULL, RT_NULL))) goto end;

	if (RT_UNLIKELY(!rt_url_parse(_R("http://www.example.com"), &url_info))) goto end;
	if (RT_UNLIKELY(!zz_test_url_check_info(&url_info, _R("http"), RT_NULL, RT_NULL, _R("www.example.com"), RT_TYPE_MAX_UN, RT_NULL, RT_NULL, RT_NULL))) goto end;

	if (RT_UNLIKELY(!rt_url_parse(_R("http://www.example.com:8080"), &url_info))) goto end;
	if (RT_UNLIKELY(!zz_test_url_check_info(&url_info, _R("http"), RT_NULL, RT_NULL, _R("www.example.com"), 8080, RT_NULL, RT_NULL, RT_NULL))) goto end;

	ret = RT_OK;
end:
	return ret;
}

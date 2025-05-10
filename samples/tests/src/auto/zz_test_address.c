#include <rpr.h>

static rt_s zz_test_address_check_stuctures_sizes(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(sizeof(struct rt_address_ipv4) != 4)) goto end;
	if (RT_UNLIKELY(sizeof(struct rt_address_ipv6) != 16)) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_address_create_from_host_name(void)
{
	struct rt_address_ipv4 ipv4_address;
	struct rt_address_ipv6 ipv6_address;
	enum rt_address_family address_family;
	rt_char buffer[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret = RT_FAILED;

	address_family = RT_ADDRESS_FAMILY_IPV4;
	if (RT_UNLIKELY(!rt_address_create_from_host_name(_R("localhost"), &ipv4_address, &ipv6_address, &address_family))) goto end;
	if (RT_UNLIKELY(address_family != RT_ADDRESS_FAMILY_IPV4)) goto end;
	buffer_size = 0;
	if (RT_UNLIKELY(!rt_address_append_ipv4(&ipv4_address, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size))) goto end;

	address_family = RT_ADDRESS_FAMILY_IPV6;
	if (RT_UNLIKELY(!rt_address_create_from_host_name(_R("localhost"), &ipv4_address, &ipv6_address, &address_family))) goto end;
	if (RT_UNLIKELY(address_family != RT_ADDRESS_FAMILY_IPV6)) goto end;
	buffer_size = 0;
	if (RT_UNLIKELY(!rt_address_append_ipv6(&ipv6_address, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_address_check_ipv4_append(struct rt_address_ipv4 *address, const rt_char *expected)
{
	rt_char buffer[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret = RT_FAILED;

	buffer_size = 0;
	if (RT_UNLIKELY(!rt_address_append_ipv4(address, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(rt_char_get_size(buffer) != buffer_size)) goto end;
	if (RT_UNLIKELY(!rt_char_equals(buffer, buffer_size, expected, rt_char_get_size(expected)))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_address_check_ipv4_do(const rt_char *str)
{
	struct rt_address_ipv4 address;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_address_create_ipv4(&address, str))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv4_append(&address, str))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_address_check_ipv4(void)
{
	struct rt_address_ipv4 address;
	rt_s ret = RT_FAILED;

	rt_address_create_ipv4_loopback(&address);
	if (RT_UNLIKELY(!zz_test_address_check_ipv4_append(&address, _R("127.0.0.1")))) goto end;

	rt_address_create_ipv4_any(&address);
	if (RT_UNLIKELY(!zz_test_address_check_ipv4_append(&address, _R("0.0.0.0")))) goto end;

	if (RT_UNLIKELY(!zz_test_address_check_ipv4_do(_R("192.168.18.139")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv4_do(_R("255.255.255.255")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv4_do(_R("0.0.0.0")))) goto end;

	if (RT_UNLIKELY(rt_address_create_ipv4(&address, _R("255.255.255.256")))) goto end;
	if (RT_UNLIKELY(rt_address_create_ipv4(&address, _R("255.255.256.255")))) goto end;
	if (RT_UNLIKELY(rt_address_create_ipv4(&address, _R("255.255.a.255")))) goto end;
	if (RT_UNLIKELY(rt_address_create_ipv4(&address, _R("255.255.255.a")))) goto end;
	if (RT_UNLIKELY(rt_address_create_ipv4(&address, _R("255.255.255")))) goto end;
	if (RT_UNLIKELY(rt_address_create_ipv4(&address, _R("255.255")))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_address_check_ipv6_append(struct rt_address_ipv6 *address, const rt_char *expected)
{
	rt_char buffer[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret = RT_FAILED;

	buffer_size = 0;
	if (RT_UNLIKELY(!rt_address_append_ipv6(address, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(rt_char_get_size(buffer) != buffer_size)) goto end;
	if (RT_UNLIKELY(!rt_char_equals(buffer, buffer_size, expected, rt_char_get_size(expected)))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_address_check_ipv6_do(const rt_char *str)
{
	struct rt_address_ipv6 address;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_address_create_ipv6(&address, str))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_append(&address, str))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_address_check_ipv6(void)
{
	struct rt_address_ipv6 address;
	rt_s ret = RT_FAILED;

	rt_address_create_ipv6_loopback(&address);
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_append(&address, _R("::1")))) goto end;

	rt_address_create_ipv6_any(&address);
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_append(&address, _R("::")))) goto end;

	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::ffff:192.0.2.128")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::ffff:0:192.0.2.128")))) goto end;

	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("2001:db8:85a3:8d3::370:7348")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("2001:db8:85a3:8d3:1319:8a2e:370:7348")))) goto end;

	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:0:0:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:0:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:0:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:0:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:0:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:0:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:0:a000::a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:0:a000::a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:0:a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:0:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:0:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:0:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:0:a000:0:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:a000:0:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:a000:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:a000:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:a000:a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:a000:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:a000:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:0:a000:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:0:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:0:a000:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:0:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:0:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:0:a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:0:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:0:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:0:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:0:a000:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:0:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000::a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000::a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000::a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000::a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000::a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000::a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:0:0:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000::a000:0:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000::a000:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000::a000:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000::a000:a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000::a000:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000::a000:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000::a000:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:0:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:0:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:0:a000::a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:0:a000::a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:0:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:0:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:0:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:0:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:a000:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:a000:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:a000:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:a000:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:a000:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000::a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000::a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000::a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000::a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000::a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000::a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:0:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:0:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:0:a000:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:0:a000:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:0:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:0:a000:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:0:a000:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:0:a000:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000::a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000::a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:0:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:0:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:0:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:0:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:a000:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:a000:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:a000:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:a000:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:a000:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:0:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:0:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:0:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:0:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:0:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:0:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:0:a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:0:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:0:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:0:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:0:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:a000:0:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:a000:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:a000:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:a000:a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:a000:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:a000:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000::a000:a000:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000::a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000::a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000::a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000::a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000::a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000::a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:0:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:0:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:0:a000:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:0:a000:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:0:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:0:a000:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:0:a000:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:0:a000:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000::a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000::a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:0:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:0:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:0:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:0:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:a000:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:a000:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:a000:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:a000:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:a000:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000::a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000::a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000::a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000::a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000::a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000::a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:0:0:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000::a000:0:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000::a000:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000::a000:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000::a000:a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000::a000:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000::a000:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000::a000:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000::a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000::a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:0:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:0:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:0:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:0:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:a000:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:a000:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:a000:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:a000:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:a000:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000::a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000::a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000::a000:0:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000::a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000::a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000::a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:0:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:0:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:0:a000:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:0:a000:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:0:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:0:a000:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:0:a000:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:0:a000:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000::a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000::a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:0:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:0:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:0:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:0:a000:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:a000::a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:a000:0:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:a000:0:a000:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:a000:a000::")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:a000:a000:0:a000")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:a000:a000:a000:0")))) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:a000:a000:a000:a000")))) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_address(void)
{
	rt_b sockets_initialized = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_socket_initialize())) goto end;
	sockets_initialized = RT_TRUE;

	if (RT_UNLIKELY(!zz_test_address_check_stuctures_sizes())) goto end;
	if (RT_UNLIKELY(!zz_test_address_create_from_host_name())) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv4())) goto end;
	if (RT_UNLIKELY(!zz_test_address_check_ipv6())) goto end;

	ret = RT_OK;
end:
	if (sockets_initialized) {
		if (RT_UNLIKELY(!rt_socket_cleanup()))
			ret = RT_FAILED;
	}

	return ret;
}

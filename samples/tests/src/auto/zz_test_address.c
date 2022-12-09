#include <rpr.h>

static rt_s zz_test_address_check_stuctures_sizes()
{
	rt_s ret;

	if (sizeof(struct rt_address_ipv4) != 4) goto error;
	if (sizeof(struct rt_address_ipv6) != 16) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_address_check_ipv4_append(struct rt_address_ipv4 *address, const rt_char *expected)
{
	rt_char buffer[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret;

	buffer_size = 0;
	if (!rt_address_append_ipv4(address, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size)) goto error;
	if (rt_char_get_size(buffer) != buffer_size) goto error;
	if (!rt_char_equals(buffer, buffer_size, expected, rt_char_get_size(expected))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_address_check_ipv4_do(const rt_char *str)
{
	struct rt_address_ipv4 address;
	rt_s ret;

	if (!rt_address_create_ipv4(&address, str)) goto error;
	if (!zz_test_address_check_ipv4_append(&address, str)) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_address_check_ipv4()
{
	struct rt_address_ipv4 address;
	rt_s ret;

	rt_address_create_ipv4_loopback(&address);
	if (!zz_test_address_check_ipv4_append(&address, _R("127.0.0.1"))) goto error;

	rt_address_create_ipv4_any(&address);
	if (!zz_test_address_check_ipv4_append(&address, _R("0.0.0.0"))) goto error;

	if (!zz_test_address_check_ipv4_do(_R("192.168.18.139"))) goto error;
	if (!zz_test_address_check_ipv4_do(_R("255.255.255.255"))) goto error;
	if (!zz_test_address_check_ipv4_do(_R("0.0.0.0"))) goto error;

	if (rt_address_create_ipv4(&address, _R("255.255.255.256"))) goto error;
	if (rt_address_create_ipv4(&address, _R("255.255.256.255"))) goto error;
	if (rt_address_create_ipv4(&address, _R("255.255.a.255"))) goto error;
	if (rt_address_create_ipv4(&address, _R("255.255.255.a"))) goto error;
	if (rt_address_create_ipv4(&address, _R("255.255.255"))) goto error;
	if (rt_address_create_ipv4(&address, _R("255.255"))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_address_check_ipv6_append(struct rt_address_ipv6 *address, const rt_char *expected)
{
	rt_char buffer[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret;

	buffer_size = 0;
	if (!rt_address_append_ipv6(address, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size)) goto error;
	if (rt_char_get_size(buffer) != buffer_size) goto error;
	if (!rt_char_equals(buffer, buffer_size, expected, rt_char_get_size(expected))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_address_check_ipv6_do(const rt_char *str)
{
	struct rt_address_ipv6 address;
	rt_s ret;

	if (!rt_address_create_ipv6(&address, str)) goto error;
	if (!zz_test_address_check_ipv6_append(&address, str)) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_address_check_ipv6()
{
	struct rt_address_ipv6 address;
	rt_s ret;

	rt_address_create_ipv6_loopback(&address);
	if (!zz_test_address_check_ipv6_append(&address, _R("::1"))) goto error;

	rt_address_create_ipv6_any(&address);
	if (!zz_test_address_check_ipv6_append(&address, _R("::"))) goto error;

	if (!zz_test_address_check_ipv6_do(_R("::ffff:192.0.2.128"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::ffff:0:192.0.2.128"))) goto error;

	if (!zz_test_address_check_ipv6_do(_R("2001:db8:85a3:8d3::370:7348"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("2001:db8:85a3:8d3:1319:8a2e:370:7348"))) goto error;

	if (!zz_test_address_check_ipv6_do(_R("::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:0:0:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:0:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:0:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:0:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:0:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:0:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:0:a000::a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:0:a000::a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:0:a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:0:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:0:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:0:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:0:a000:0:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:a000:0:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:a000:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:a000:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:a000:a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:a000:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:a000:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:0:a000:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:0:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:0:a000:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:0:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:0:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:0:a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:0:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:0:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:0:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:0:a000:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:0:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("::a000:a000:a000:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000::a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000::a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000::a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000::a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000::a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000::a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:0:0:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000::a000:0:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000::a000:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000::a000:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000::a000:a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000::a000:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000::a000:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000::a000:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:0:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:0:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:0:a000::a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:0:a000::a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:0:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:0:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:0:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:0:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:a000:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:a000:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:a000:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:a000:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:0:a000:a000:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000::a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000::a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000::a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000::a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000::a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000::a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:0:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:0:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:0:a000:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:0:a000:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:0:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:0:a000:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:0:a000:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:0:a000:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000::a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000::a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:0:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:0:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:0:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:0:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:a000:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:a000:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:a000:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:a000:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("0:a000:a000:a000:a000:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:0:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:0:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:0:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:0:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:0:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:0:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:0:a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:0:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:0:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:0:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:0:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:a000:0:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:a000:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:a000:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:a000:a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:a000:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:a000:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000::a000:a000:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000::a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000::a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000::a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000::a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000::a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000::a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:0:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:0:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:0:a000:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:0:a000:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:0:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:0:a000:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:0:a000:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:0:a000:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000::a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000::a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:0:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:0:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:0:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:0:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:a000:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:a000:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:a000:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:a000:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:0:a000:a000:a000:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000::a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000::a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000::a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000::a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000::a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000::a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:0:0:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000::a000:0:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000::a000:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000::a000:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000::a000:a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000::a000:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000::a000:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000::a000:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000::a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000::a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:0:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:0:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:0:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:0:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:a000:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:a000:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:a000:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:a000:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:0:a000:a000:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000::a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000::a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000::a000:0:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000::a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000::a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000::a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:0:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:0:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:0:a000:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:0:a000:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:0:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:0:a000:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:0:a000:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:0:a000:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000::a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000::a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:0:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:0:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:0:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:0:a000:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:a000::a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:a000:0:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:a000:0:a000:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:a000:a000::"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:a000:a000:0:a000"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:a000:a000:a000:0"))) goto error;
	if (!zz_test_address_check_ipv6_do(_R("a000:a000:a000:a000:a000:a000:a000:a000"))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_address()
{
	rt_s ret;

	if (!zz_test_address_check_stuctures_sizes()) goto error;
	if (!zz_test_address_check_ipv4()) goto error;
	if (!zz_test_address_check_ipv6()) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

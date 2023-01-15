#include <rpr.h>

#include "zz_utils.h"

static rt_s zz_test_properties_display(struct rt_hash_table_entry *properties)
{
	rt_char buffer[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	struct rt_array_header *header;
	rt_un array_size;
	rt_char *key_chars;
	rt_char *value_chars;
	rt_un i;
	rt_s ret;

	header = RT_ARRAY_GET_HEADER(properties);
	array_size = header->size;

	for (i = 0; i < array_size; i++) {
		key_chars = (rt_char*)properties[i].key;
		if (key_chars) {
			value_chars = (rt_char*)properties[i].value;
			buffer_size = 0;
			if (!rt_char_append_char(_R('"'), buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size)) goto error;
			if (!rt_char_append(key_chars, rt_char_get_size(key_chars), buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size)) goto error;
			if (!rt_char_append(_R("\" => \""), 6, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size)) goto error;
			if (!rt_char_append(value_chars, rt_char_get_size(value_chars), buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size)) goto error;
			if (!rt_char_append(_R("\"\n"), 2, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size)) goto error;

			if (!rt_console_write_string_with_size(buffer, buffer_size)) goto error;
		}
	}
	if (!rt_console_write_string_with_size(_R("\n"), 1)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_properties_check_value(struct rt_hash_table_entry *properties, const rt_char *key, const rt_char *expected_value)
{
	rt_char *value;
	rt_s ret;

	if (!rt_hash_table_get(properties, key, rt_char_get_size(key), (void**)&value)) goto error;
	if (!rt_char_equals(value, rt_char_get_size(value), expected_value, rt_char_get_size(expected_value))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_properties_do(rt_char *test_resources_dir, struct rt_heap *heap)
{
	rt_char properties_file_path[RT_FILE_PATH_SIZE];
	rt_un properties_file_path_size;
	struct rt_hash_table_entry *properties = RT_NULL;
	struct rt_properties_header *header;
	rt_s ret;

	properties_file_path_size = rt_char_get_size(test_resources_dir);
	if (!rt_char_copy(test_resources_dir, properties_file_path_size, properties_file_path, RT_FILE_PATH_SIZE)) goto error;
	if (!rt_file_path_append_separator(properties_file_path, RT_FILE_PATH_SIZE, &properties_file_path_size)) goto error;
	if (!rt_char_append(_R("test.properties"), 15, properties_file_path, RT_FILE_PATH_SIZE, &properties_file_path_size)) goto error;

	if (!rt_properties_create(&properties, properties_file_path, RT_ENCODING_ISO_8859_1, 128, heap)) goto error;

	if (!zz_test_properties_display(properties)) goto error;

	/* Check the properties header. */
	header = RT_PROPERTIES_GET_HEADER(properties);
	if (!rt_char_equals(header->file_path, rt_char_get_size(header->file_path), properties_file_path, properties_file_path_size)) goto error;
	if (header->hash_table_header.size != 21) goto error;
	if (header->hash_table_header.array_header.size != 128) goto error;

	/* Check the hash table content. */
	if (!zz_test_properties_check_value(properties, _R("foo1"),                        _R("FOO1")))                  goto error;
	if (!zz_test_properties_check_value(properties, _R("foo2"),                        _R("FOO2")))                  goto error;
	if (!zz_test_properties_check_value(properties, _R("foo3"),                        _R("FOO3")))                  goto error;
	if (!zz_test_properties_check_value(properties, _R("foo4"),                        _R("FOO4")))                  goto error;
	if (!zz_test_properties_check_value(properties, _R("foo5"),                        _R("FOO5")))                  goto error;
	if (!zz_test_properties_check_value(properties, _R("foo6"),                        _R("FOO6")))                  goto error;
	if (!zz_test_properties_check_value(properties, _R("foo7"),                        _R("FOO7 # Not a comment."))) goto error;
	if (!zz_test_properties_check_value(properties, _R("#hash"),                       _R("#HASH")))                 goto error;
	if (!zz_test_properties_check_value(properties, _R("h#hash"),                      _R("H#HASH")))                goto error;
	if (!zz_test_properties_check_value(properties, _R("key"),                         _R("not_key = not_value")))   goto error;
	if (!zz_test_properties_check_value(properties, _R("space"),                       _R("space ")))                goto error;
	if (!zz_test_properties_check_value(properties, _R("without_value"),               _R("")))                      goto error;
	if (!zz_test_properties_check_value(properties, _R("without_value_and_separator"), _R("")))                      goto error;
	if (!zz_test_properties_check_value(properties, _R("without_value_and_space"),     _R("")))                      goto error;
	if (!zz_test_properties_check_value(properties, _R(""),                            _R("without_key")))           goto error;
	if (!zz_test_properties_check_value(properties, _R("\\backslash1\\"),              _R("\\BACKSLASH1\\")))        goto error;
	if (!zz_test_properties_check_value(properties, _R("\\backslash2\\"),              _R("\\BACKSLASH2\\")))        goto error;
	if (!zz_test_properties_check_value(properties, _R("foo bar"),                     _R("team")))                  goto error;
	if (!zz_test_properties_check_value(properties, _R("\t"),                          _R("\t")))                    goto error;
	if (!zz_test_properties_check_value(properties, _R("=equals="),                    _R("=EQUALS=")))              goto error;
	if (!zz_test_properties_check_value(properties, _R("last_key"),                    _R("last_value")))            goto error;

	ret = RT_OK;
free:
	if (!rt_properties_free(&properties) && ret)
		goto error;
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_properties()
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	rt_char test_resources_dir[RT_FILE_PATH_SIZE];
	rt_un test_resources_dir_size;
	rt_s ret;

	if (!rt_runtime_heap_create(&runtime_heap))
		goto error;
	runtime_heap_created = RT_TRUE;

	if (!zz_get_test_resources_dir(test_resources_dir, RT_FILE_PATH_SIZE, &test_resources_dir_size))
		goto error;

	if (!zz_test_properties_do(test_resources_dir, &runtime_heap.heap))
		goto error;

	ret = RT_OK;
free:
	if (runtime_heap_created) {
		runtime_heap_created = RT_FALSE;
		if (!runtime_heap.heap.close(&runtime_heap.heap) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

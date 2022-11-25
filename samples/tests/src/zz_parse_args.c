#include <rpr.h>

static rt_s zz_parse_args_callback(enum rt_command_line_args_type arg_type, rt_b valid, rt_char short_option, const rt_char *long_option,
				   enum rt_command_line_args_value_cardinality value_cardinality, const rt_char *value, RT_UNUSED void *context)
{
	rt_char message[RT_CHAR_BIG_STRING_SIZE];
	rt_un message_size;
	rt_s ret;

	message_size = 0;

	if (arg_type == RT_COMMAND_LINE_ARGS_TYPE_SHORT) {

		message_size = 1;
		if (!rt_char_copy(&short_option, message_size, message, RT_CHAR_BIG_STRING_SIZE)) goto error;
		if (!rt_char_append(_R(", short"), 7, message, RT_CHAR_BIG_STRING_SIZE, &message_size)) goto error;

	} else {

		message_size = rt_char_get_size(long_option);
		if (!rt_char_copy(long_option, message_size, message, RT_CHAR_BIG_STRING_SIZE)) goto error;
		if (!rt_char_append(_R(", long"), 6, message, RT_CHAR_BIG_STRING_SIZE, &message_size)) goto error;

	}

	if (valid) {
		if (!rt_char_append(_R(", valid"), 7, message, RT_CHAR_BIG_STRING_SIZE, &message_size))
			goto error;
	} else {
		if (!rt_char_append(_R(", invalid"), 9, message, RT_CHAR_BIG_STRING_SIZE, &message_size))
			goto error;
	}

	switch (value_cardinality)
	{
		case RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_NONE:
			if (!rt_char_append(_R(", without value"), 15, message, RT_CHAR_BIG_STRING_SIZE, &message_size))
				goto error;
			break;
		case RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_OPTIONAL:
			if (!rt_char_append(_R(", with optional value"), 21, message, RT_CHAR_BIG_STRING_SIZE, &message_size))
				goto error;
			break;
		case RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_REQUIRED:
			if (!rt_char_append(_R(", with required value"), 21, message, RT_CHAR_BIG_STRING_SIZE, &message_size))
				goto error;
			break;
		default:
			goto error;
	}

	if (value) {
		if (!rt_char_append(_R(" => "), 4, message, RT_CHAR_BIG_STRING_SIZE, &message_size))
			goto error;
		if (!rt_char_append(value, rt_char_get_size(value), message, RT_CHAR_BIG_STRING_SIZE, &message_size))
			goto error;
	} else {
		if (!rt_char_append(_R(", empty"), 7, message, RT_CHAR_BIG_STRING_SIZE, &message_size))
			goto error;
	}

	if (!rt_char_append(_R("\n"), 1, message, RT_CHAR_BIG_STRING_SIZE, &message_size))
		goto error;

	if (!rt_console_write_string_with_size(message, message_size))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_parse_args(rt_un argc, const rt_char *argv[])
{
	const rt_char *long_options_without_arg[6];
	const rt_char *long_options_with_optional_arg[6];
	const rt_char *long_options_with_arg[6];
	rt_un non_options_index;
	rt_un i;
	rt_s ret;

	long_options_without_arg[0] = _R("none1");
	long_options_without_arg[1] = _R("none2");
	long_options_without_arg[2] = _R("none3");
	long_options_without_arg[3] = _R("none4");
	long_options_without_arg[4] = _R("none5");
	long_options_without_arg[5] = RT_NULL;

	long_options_with_optional_arg[0] = _R("optional1");
	long_options_with_optional_arg[1] = _R("optional2");
	long_options_with_optional_arg[2] = _R("optional3");
	long_options_with_optional_arg[3] = _R("optional4");
	long_options_with_optional_arg[4] = _R("optional5");
	long_options_with_optional_arg[5] = RT_NULL;

	long_options_with_arg[0] = _R("required1");
	long_options_with_arg[1] = _R("required2");
	long_options_with_arg[2] = _R("required3");
	long_options_with_arg[3] = _R("required4");
	long_options_with_arg[4] = _R("required5");
	long_options_with_arg[5] = RT_NULL;

	if (!rt_command_line_args_parse(&argc, argv, &zz_parse_args_callback, RT_NULL,
					_R("abcdefg"), _R("hijklmn"), _R("opqrstu"),
					long_options_without_arg, long_options_with_optional_arg, long_options_with_arg,
					&non_options_index))
		goto error;

	if (!rt_console_write_string(_R("\nNon-options:\n")))
		goto error;

	for (i = non_options_index; i < argc; i++) {
		if (!rt_console_write_string(argv[i])) goto error;
		if (!rt_console_write_string(_R("\n"))) goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	rt_console_write_error(_R("Parsing has failed.\n"));
	ret = RT_FAILED;
	goto free;
}

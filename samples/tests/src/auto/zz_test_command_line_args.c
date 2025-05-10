#include <rpr.h>

#include "zz_utils.h"

static rt_s zz_test_command_line_args_check(enum rt_command_line_args_type expected_arg_type, rt_b expected_valid,
					    rt_char expected_short_option, const rt_char *expected_long_option, enum rt_command_line_args_value_cardinality expected_value_cardinality,
					    const rt_char *expected_value,
					    enum rt_command_line_args_type arg_type, rt_b valid,
					    rt_char short_option, const rt_char *long_option, enum rt_command_line_args_value_cardinality value_cardinality,
					    const rt_char *value)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(arg_type != expected_arg_type)) goto end;
	if (RT_UNLIKELY((valid && !expected_valid) || (!valid && expected_valid))) goto end;
	if (RT_UNLIKELY(short_option != expected_short_option)) goto end;
	if (RT_UNLIKELY(!zz_char_equals_with_nulls(long_option, expected_long_option))) goto end;
	if (RT_UNLIKELY(value_cardinality != expected_value_cardinality)) goto end;
	if (RT_UNLIKELY(!zz_char_equals_with_nulls(value, expected_value))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_command_line_args_callback(enum rt_command_line_args_type arg_type, rt_b valid, rt_char short_option, const rt_char *long_option,
					       enum rt_command_line_args_value_cardinality value_cardinality, const rt_char *value, void *context)
{
	rt_un *argument_index = (rt_un*)context;
	rt_s ret = RT_FAILED;

	switch (*argument_index)
	{
		case 0:
			if (RT_UNLIKELY(!zz_test_command_line_args_check(RT_COMMAND_LINE_ARGS_TYPE_SHORT, RT_TRUE, _R('a'), RT_NULL, RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_NONE, RT_NULL,
							     arg_type, valid, short_option, long_option, value_cardinality, value)))
				goto end;
			break;
		case 1:
			if (RT_UNLIKELY(!zz_test_command_line_args_check(RT_COMMAND_LINE_ARGS_TYPE_SHORT, RT_TRUE, _R('b'), RT_NULL, RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_NONE, RT_NULL,
							     arg_type, valid, short_option, long_option, value_cardinality, value)))
				goto end;
			break;
		case 2:
			if (RT_UNLIKELY(!zz_test_command_line_args_check(RT_COMMAND_LINE_ARGS_TYPE_SHORT, RT_TRUE, _R('h'), RT_NULL, RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_OPTIONAL, _R("HVAL"),
							     arg_type, valid, short_option, long_option, value_cardinality, value)))
				goto end;
			break;
		case 3:
			if (RT_UNLIKELY(!zz_test_command_line_args_check(RT_COMMAND_LINE_ARGS_TYPE_SHORT, RT_TRUE, _R('c'), RT_NULL, RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_NONE, RT_NULL,
							     arg_type, valid, short_option, long_option, value_cardinality, value)))
				goto end;
			break;
		case 4:
			if (RT_UNLIKELY(!zz_test_command_line_args_check(RT_COMMAND_LINE_ARGS_TYPE_SHORT, RT_TRUE, _R('o'), RT_NULL, RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_REQUIRED, _R("OVAL"),
							     arg_type, valid, short_option, long_option, value_cardinality, value)))
				goto end;
			break;
		case 5:
			if (RT_UNLIKELY(!zz_test_command_line_args_check(RT_COMMAND_LINE_ARGS_TYPE_SHORT, RT_TRUE, _R('d'), RT_NULL, RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_NONE, RT_NULL,
							     arg_type, valid, short_option, long_option, value_cardinality, value)))
				goto end;
			break;
		case 6:
			if (RT_UNLIKELY(!zz_test_command_line_args_check(RT_COMMAND_LINE_ARGS_TYPE_SHORT, RT_TRUE, _R('p'), RT_NULL, RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_REQUIRED, _R("PVAL"),
							     arg_type, valid, short_option, long_option, value_cardinality, value)))
				goto end;
			break;
		case 7:
			if (RT_UNLIKELY(!zz_test_command_line_args_check(RT_COMMAND_LINE_ARGS_TYPE_LONG, RT_TRUE, 0, _R("optional1"), RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_OPTIONAL, RT_NULL,
							     arg_type, valid, short_option, long_option, value_cardinality, value)))
				goto end;
			break;
		case 8:
			if (RT_UNLIKELY(!zz_test_command_line_args_check(RT_COMMAND_LINE_ARGS_TYPE_LONG, RT_TRUE, 0, _R("optional2"), RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_OPTIONAL, _R("OPTIONAL2"),
							     arg_type, valid, short_option, long_option, value_cardinality, value)))
				goto end;
			break;
		case 9:
			if (RT_UNLIKELY(!zz_test_command_line_args_check(RT_COMMAND_LINE_ARGS_TYPE_LONG, RT_FALSE, 0, _R("unknown"), RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_NONE, _R("UNKNOWN"),
							     arg_type, valid, short_option, long_option, value_cardinality, value)))
				goto end;
			break;
		case 10:
			if (RT_UNLIKELY(!zz_test_command_line_args_check(RT_COMMAND_LINE_ARGS_TYPE_SHORT, RT_FALSE, _R('v'), RT_NULL, RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_NONE, RT_NULL,
							     arg_type, valid, short_option, long_option, value_cardinality, value)))
				goto end;
			break;
		case 11:
			if (RT_UNLIKELY(!zz_test_command_line_args_check(RT_COMMAND_LINE_ARGS_TYPE_LONG, RT_TRUE, 0, _R("required1"), RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_REQUIRED, _R("REQUIRED1"),
							     arg_type, valid, short_option, long_option, value_cardinality, value)))
				goto end;
			break;
		case 12:
			if (RT_UNLIKELY(!zz_test_command_line_args_check(RT_COMMAND_LINE_ARGS_TYPE_LONG, RT_TRUE, 0, _R("required2"), RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_REQUIRED, _R("REQUIRED2"),
							     arg_type, valid, short_option, long_option, value_cardinality, value)))
				goto end;
			break;
		case 13:
			if (RT_UNLIKELY(!zz_test_command_line_args_check(RT_COMMAND_LINE_ARGS_TYPE_LONG, RT_TRUE, 0, _R("none1"), RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_NONE, RT_NULL,
							     arg_type, valid, short_option, long_option, value_cardinality, value)))
				goto end;
			break;
		case 14:
			if (RT_UNLIKELY(!zz_test_command_line_args_check(RT_COMMAND_LINE_ARGS_TYPE_LONG, RT_TRUE, 0, _R("none2"), RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_NONE, _R("BAD_VALUE"),
							     arg_type, valid, short_option, long_option, value_cardinality, value)))
				goto end;
			break;
		case 15:
			if (RT_UNLIKELY(!zz_test_command_line_args_check(RT_COMMAND_LINE_ARGS_TYPE_SHORT, RT_TRUE, _R('e'), RT_NULL, RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_NONE, RT_NULL,
							     arg_type, valid, short_option, long_option, value_cardinality, value)))
				goto end;
			break;
		case 16:
			if (RT_UNLIKELY(!zz_test_command_line_args_check(RT_COMMAND_LINE_ARGS_TYPE_LONG, RT_TRUE, 0, _R("required3"), RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_REQUIRED, _R("REQUIRED3"),
							     arg_type, valid, short_option, long_option, value_cardinality, value)))
				goto end;
			break;
		default:
			goto end;
	}

	ret = RT_OK;
end:
	/* Next argument. */
	(*argument_index)++;

	return ret;
}

rt_s zz_test_command_line_args(void)
{
	rt_un argc;
	const rt_char *argv[22];
	const rt_char *long_options_without_arg[3];
	const rt_char *long_options_with_optional_arg[4];
	const rt_char *long_options_with_arg[4];
	const rt_char *non_options[4];
	rt_un argument_index;
	rt_un non_options_index;
	rt_un i;
	rt_un j;
	rt_s ret = RT_FAILED;

	argc = sizeof(argv) / sizeof(rt_char*);
	argv[0]	= _R("command");
	argv[1]	= _R("-abhHVAL");
	argv[2]	= _R("-coOVAL");
	argv[3]	= _R("-dp");
	argv[4]	= _R("PVAL");
	argv[5]	= _R("--optional1");
	argv[6]	= _R("--optional2=OPTIONAL2");
	argv[7]	= _R("--unknown=UNKNOWN");
	argv[8]	= _R("-v");
	argv[9]	= _R("--required1");
	argv[10] = _R("REQUIRED1");
	argv[11] = _R("--required2=REQUIRED2");
	argv[12] = _R("NON_OPTION1");
	argv[13] = _R("--none1");
	argv[14] = _R("--none2=BAD_VALUE");
	argv[15] = _R("-e");
	argv[16] = _R("NON_OPTION2");
	argv[17] = _R("--required3");
	argv[18] = _R("REQUIRED3");
	argv[19] = _R("--");
	argv[20] = _R("--optional3");
	argv[21] = _R("-f");

	long_options_without_arg[0] = _R("none1");
	long_options_without_arg[1] = _R("none2");
	long_options_without_arg[2] = RT_NULL;

	long_options_with_optional_arg[0] = _R("optional1");
	long_options_with_optional_arg[1] = _R("optional2");
	long_options_with_optional_arg[2] = _R("optional3");
	long_options_with_optional_arg[3] = RT_NULL;

	long_options_with_arg[0] = _R("required1");
	long_options_with_arg[1] = _R("required2");
	long_options_with_arg[2] = _R("required3");
	long_options_with_arg[3] = RT_NULL;

	argument_index = 0;
	if (RT_UNLIKELY(!rt_command_line_args_parse(&argc, argv, &zz_test_command_line_args_callback, &argument_index,
						    _R("abcdefg"), _R("hijklmn"), _R("opqrstu"),
						    long_options_without_arg, long_options_with_optional_arg, long_options_with_arg,
						    &non_options_index)))
		goto end;

	if (RT_UNLIKELY(argc - non_options_index != sizeof(non_options) / sizeof(rt_char*)))
		goto end;

	non_options[0] = _R("NON_OPTION1");
	non_options[1] = _R("NON_OPTION2");
	non_options[2] = _R("--optional3");
	non_options[3] = _R("-f");

	j = 0;
	for (i = non_options_index; i < argc; i++) {
		if (RT_UNLIKELY(!rt_char_equals(non_options[j], rt_char_get_size(non_options[j]), argv[i], rt_char_get_size(argv[i]))))
			goto end;
		j++;
	}

	ret = RT_OK;
end:
	return ret;
}

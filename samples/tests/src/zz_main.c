#include <rpr.h>
#include <rpr_main.h>

rt_s zz_auto_tests();
rt_s zz_manual_tests();
rt_s zz_parse_args(rt_un argc, const rt_char *argv[]);

rt_s zz_display_help(rt_s ret)
{
	if (!rt_console_write_string(_R("tests [\"--parse-args\"|-p]\n")))
		ret = RT_FAILED;

	return ret;
}

rt_s zz_test()
{
	rt_s ret;

	if (!zz_auto_tests()) goto error;
	if (!zz_manual_tests()) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_main_do(rt_un argc, const rt_char *argv[])
{
	rt_un arg_size;
	rt_s ret;

	if (argc == 1) {
		if (!zz_test())
			goto error;
	} else {
		arg_size = rt_char_get_size(argv[1]);
		if (rt_char_equals(argv[1], arg_size, _R("--parse-args"), 12) ||
		    rt_char_equals(argv[1], arg_size, _R("-p"), 2)) {

			if (!zz_parse_args(argc - 1, &argv[1]))
				goto error;

		} else if (argc == 2) {

			if (rt_char_equals(argv[1], arg_size, _R("--help"), 6) ||
				   rt_char_equals(argv[1], arg_size, _R("-h"), 2) ||
				   rt_char_equals(argv[1], arg_size, _R("/?"), 2)) {

				if (!zz_display_help(RT_OK))
					goto error;

			} else {
				if (!zz_display_help(RT_FAILED))
					goto error;
			}
		} else {
			if (!zz_display_help(RT_FAILED))
				goto error;
		}
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_un16 rpr_main(rt_un argc, const rt_char *argv[])
{
	int ret;

	if (zz_main_do(argc, argv))
		ret = 0;
	else
		ret = 1;
	return ret;
}

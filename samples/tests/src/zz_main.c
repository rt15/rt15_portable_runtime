#include <rpr.h>
#include <rpr_main.h>

rt_s zz_auto_tests(void);
rt_s zz_manual_tests(void);
rt_s zz_parse_args(rt_un argc, const rt_char *argv[]);
rt_s zz_display_args(rt_un argc, const rt_char *argv[]);
rt_s zz_display_env_vars(void);
rt_s zz_display_env_var(const rt_char *name);
rt_s zz_read_line(void);
rt_s zz_generate_char8(void);
rt_s zz_lock_shared_mutex(void);
rt_s zz_shared_memory_chat(void);

static rt_s zz_display_help(rt_s ret)
{
	rt_b error = !ret;

	if (!rt_console_write(_R("tests\n"), error))
		ret = RT_FAILED;
	if (!rt_console_write(_R("tests <--parse-args|-p> ARGS...\n"), error))
		ret = RT_FAILED;
	if (!rt_console_write(_R("tests <--display-args|-d> ARGS...\n"), error))
		ret = RT_FAILED;
	if (!rt_console_write(_R("tests <--display-env-var|-v> [ENV_VAR]\n"), error))
		ret = RT_FAILED;
	if (!rt_console_write(_R("tests <--read-line|-r>\n"), error))
		ret = RT_FAILED;
	if (!rt_console_write(_R("tests <--manual|-m>\n"), error))
		ret = RT_FAILED;
	if (!rt_console_write(_R("tests <--clear-screen|-c>\n"), error))
		ret = RT_FAILED;
	if (!rt_console_write(_R("tests <--generate-char8|-g>\n"), error))
		ret = RT_FAILED;
	if (!rt_console_write(_R("tests <--lock-mutex|-l>\n"), error))
		ret = RT_FAILED;
	if (!rt_console_write(_R("tests <--shared-memory-chat|-s>\n"), error))
		ret = RT_FAILED;

	return ret;
}

static rt_s zz_main(rt_un argc, const rt_char *argv[])
{
	rt_un arg_size;
	rt_s ret;

	if (argc == 1) {
		if (RT_UNLIKELY(!zz_auto_tests()))
			goto error;
	} else {
		arg_size = rt_char_get_size(argv[1]);
		if (rt_char_equals(argv[1], arg_size, _R("--parse-args"), 12) ||
		    rt_char_equals(argv[1], arg_size, _R("-p"), 2)) {

			if (RT_UNLIKELY(!zz_parse_args(argc - 1, &argv[1])))
				goto error;

		} else if (rt_char_equals(argv[1], arg_size, _R("--display-args"), 14) ||
			   rt_char_equals(argv[1], arg_size, _R("-d"), 2)) {

			if (RT_UNLIKELY(!zz_display_args(argc - 2, &argv[2])))
				goto error;

		} else if (rt_char_equals(argv[1], arg_size, _R("--display-env-var"), 17) ||
			   rt_char_equals(argv[1], arg_size, _R("-v"), 2)) {
			if (argc == 2) {
				if (RT_UNLIKELY(!zz_display_env_vars()))
					goto error;
			} else if (argc == 3) {
				if (RT_UNLIKELY(!zz_display_env_var(argv[2])))
					goto error;
			} else {
				if (RT_UNLIKELY(!zz_display_help(RT_FAILED)))
					goto error;
			}

		} else if (argc == 2) {

			if (rt_char_equals(argv[1], arg_size, _R("--help"), 6) ||
			    rt_char_equals(argv[1], arg_size, _R("-h"), 2) ||
			    rt_char_equals(argv[1], arg_size, _R("/?"), 2)) {

				if (RT_UNLIKELY(!zz_display_help(RT_OK)))
					goto error;

			} else if (rt_char_equals(argv[1], arg_size, _R("--read-line"), 11) ||
				   rt_char_equals(argv[1], arg_size, _R("-r"), 2)) {

				if (RT_UNLIKELY(!zz_read_line()))
					goto error;

			} else if (rt_char_equals(argv[1], arg_size, _R("--manual"), 8) ||
				   rt_char_equals(argv[1], arg_size, _R("-m"), 2)) {

				if (RT_UNLIKELY(!zz_manual_tests()))
					goto error;

			} else if (rt_char_equals(argv[1], arg_size, _R("--clear-screen"), 14) ||
				   rt_char_equals(argv[1], arg_size, _R("-c"), 2)) {

				if (RT_UNLIKELY(!rt_console_clear()))
					goto error;

			} else if (rt_char_equals(argv[1], arg_size, _R("--generate-char8"), 16) ||
				   rt_char_equals(argv[1], arg_size, _R("-g"), 2)) {

				if (RT_UNLIKELY(!zz_generate_char8()))
					goto error;

			} else if (rt_char_equals(argv[1], arg_size, _R("--lock-mutex"), 12) ||
				   rt_char_equals(argv[1], arg_size, _R("-l"), 2)) {

				if (RT_UNLIKELY(!zz_lock_shared_mutex()))
					goto error;

			} else if (rt_char_equals(argv[1], arg_size, _R("--shared-memory-chat"), 20) ||
				   rt_char_equals(argv[1], arg_size, _R("-s"), 2)) {

				if (RT_UNLIKELY(!zz_shared_memory_chat()))
					goto error;

			} else {
				if (RT_UNLIKELY(!zz_display_help(RT_FAILED)))
					goto error;
			}
		} else {
			if (RT_UNLIKELY(!zz_display_help(RT_FAILED)))
				goto error;
		}
	}

	ret = RT_OK;
free:

	if (RT_UNLIKELY(!rt_last_error_message_cleanup_thread_buffer()))
		ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_last_error_message_cleanup()))
		ret = RT_FAILED;

	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_un16 rpr_main(rt_un argc, const rt_char *argv[])
{
	int ret;

	if (zz_main(argc, argv))
		ret = 0;
	else
		ret = 1;
	return ret;
}

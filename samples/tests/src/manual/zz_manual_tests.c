#include <rpr.h>

/* Layer 000. */
rt_s zz_manual_test_types(void);

/* Layer 002. */
rt_s zz_manual_test_get_process_id(void);
rt_s zz_manual_test_system_info(void);

/* Layer 003. */
rt_s zz_manual_test_env_var(void);

/* Layer 004. */
rt_s zz_manual_test_encoding(void);
rt_s zz_manual_test_file_path(void);
rt_s zz_manual_test_os_version(void);
rt_s zz_manual_test_time(void);
rt_s zz_manual_test_url(void);

/* Layer 005. */
rt_s zz_manual_test_console(void);
rt_s zz_manual_test_temp_file(void);

/* Layer 006. */
rt_s zz_manual_test_console8(void);
rt_s zz_manual_test_error_message(void);

/* Layer 007. */
rt_s zz_manual_test_process(void);

rt_s zz_manual_tests(void)
{
	rt_s ret = RT_FAILED;

	/* Layer 000. */
	if (RT_UNLIKELY(!zz_manual_test_types())) goto end;

	/* Layer 002. */
	if (RT_UNLIKELY(!zz_manual_test_get_process_id())) goto end;
	if (RT_UNLIKELY(!zz_manual_test_system_info())) goto end;

	/* Layer 003. */
	if (RT_UNLIKELY(!zz_manual_test_env_var())) goto end;

	/* Layer 004. */
	if (RT_UNLIKELY(!zz_manual_test_encoding())) goto end;
	if (RT_UNLIKELY(!zz_manual_test_file_path())) goto end;
	if (RT_UNLIKELY(!zz_manual_test_os_version())) goto end;
	if (RT_UNLIKELY(!zz_manual_test_time())) goto end;
	if (RT_UNLIKELY(!zz_manual_test_url())) goto end;

	/* Layer 005. */
	if (RT_UNLIKELY(!zz_manual_test_console())) goto end;
	if (RT_UNLIKELY(!zz_manual_test_temp_file())) goto end;

	/* Layer 006. */
	if (RT_UNLIKELY(!zz_manual_test_console8())) goto end;
	if (RT_UNLIKELY(!zz_manual_test_error_message())) goto end;

	/* Layer 007. */
	if (RT_UNLIKELY(!zz_manual_test_process())) goto end;

	ret = RT_OK;
end:
	return ret;
}

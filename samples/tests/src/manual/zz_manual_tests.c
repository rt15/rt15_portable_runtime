#include <rpr.h>

/* Layer 000. */
rt_s zz_manual_test_types();

/* Layer 002. */
rt_s zz_manual_test_get_process_id();

/* Layer 003. */
rt_s zz_manual_test_env_var();

/* Layer 004. */
rt_s zz_manual_test_encoding();
rt_s zz_manual_test_file_path();
rt_s zz_manual_test_os_version();
rt_s zz_manual_test_url();

/* Layer 005. */
rt_s zz_manual_test_console();
rt_s zz_manual_test_temp_file();

/* Layer 006. */
rt_s zz_manual_test_error_message();

/* Layer 007. */
rt_s zz_manual_test_process();

rt_s zz_manual_tests()
{
	rt_s ret;

	/* Layer 000. */
	if (RT_UNLIKELY(!zz_manual_test_types())) goto error;

	/* Layer 002. */
	if (RT_UNLIKELY(!zz_manual_test_get_process_id())) goto error;

	/* Layer 003. */
	if (RT_UNLIKELY(!zz_manual_test_env_var())) goto error;

	/* Layer 004. */
	if (RT_UNLIKELY(!zz_manual_test_encoding())) goto error;
	if (RT_UNLIKELY(!zz_manual_test_file_path())) goto error;
	if (RT_UNLIKELY(!zz_manual_test_os_version())) goto error;
	if (RT_UNLIKELY(!zz_manual_test_url())) goto error;

	/* Layer 005. */
	if (RT_UNLIKELY(!zz_manual_test_console())) goto error;
	if (RT_UNLIKELY(!zz_manual_test_temp_file())) goto error;

	/* Layer 006. */
	if (RT_UNLIKELY(!zz_manual_test_error_message())) goto error;

	/* Layer 007. */
	if (RT_UNLIKELY(!zz_manual_test_process())) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

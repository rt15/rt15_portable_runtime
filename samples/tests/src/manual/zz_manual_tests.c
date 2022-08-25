#include <rpr.h>

/* Layer 003. */
rt_s zz_manual_test_env_var();

/* Layer 004. */
rt_s zz_manual_test_file_path();
rt_s zz_manual_test_os_version();

/* Layer 005. */
rt_s zz_manual_test_temp_file();

/* Layer 006. */
rt_s zz_manual_test_error_message();

rt_s zz_manual_tests()
{
	rt_s ret;

	/* Layer 003. */
	if (!zz_manual_test_env_var()) goto error;

	/* Layer 004. */
	if (!zz_manual_test_file_path()) goto error;
	if (!zz_manual_test_os_version()) goto error;

	/* Layer 005. */
	if (!zz_manual_test_temp_file()) goto error;

	/* Layer 006. */
	if (!zz_manual_test_error_message()) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

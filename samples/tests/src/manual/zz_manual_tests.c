#include <rpr.h>

/* Layer 003. */
rt_s zz_manual_test_env_var();

/* Layer 004. */
rt_s zz_manual_test_file_path();

rt_s zz_manual_tests()
{
	rt_s ret;

	/* Layer 003. */
	if (!zz_manual_test_env_var()) goto error;

	/* Layer 004. */
	if (!zz_manual_test_file_path()) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

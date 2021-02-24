#include <stdio.h>
#include <stdlib.h>

#include <rpr.h>
#include <rpr_mem.h>
#include <rpr_main.h>

rt_s zz_test_char8();

rt_s zz_test()
{
	rt_s ret;

	if (!zz_test_char8())
		goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_un16 rpr_main(rt_n32 argc, rt_char *argv[])
{
	int ret;

	if (zz_test())
		ret = 0;
	else
		ret = 1;
	return ret;
}

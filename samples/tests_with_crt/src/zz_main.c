#include <stdio.h>
#include <stdlib.h>

#define RT_DEFINE_USE_CRT

#include <rpr.h>

rt_s zz_test()
{
	rt_s ret;

	printf("Testing...\n");

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

int main()
{
	int ret;

	if (zz_test())
		ret = 0;
	else
		ret = 1;
	return ret;
}

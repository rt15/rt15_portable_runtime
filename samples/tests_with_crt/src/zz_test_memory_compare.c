#include <rpr.h>

#include "zz_resources.h"
#include "zz_utils.h"

static rt_n zz_memory_compare(void *area1, void *area2, rt_un size)
{
	return memcmp(area1, area2, size);
}

rt_s zz_test_memory_compare(struct rt_output_stream *output_stream)
{
	struct rt_chrono chrono;
	rt_uchar8 area1[ZZ_BUFFER_SIZE];
	rt_uchar8 area2[ZZ_BUFFER_SIZE];
	rt_un i;
	rt_s ret = RT_FAILED;

	/* Initialize source buffer. */
	for (i = 0; i < ZZ_BUFFER_SIZE - 1; i++) {
		area1[i] = (rt_uchar8)(RT_MEMORY_MODULO_POWER_OF_TWO(i, 256));
		area2[i] = area1[i];
	}
	area1[ZZ_BUFFER_SIZE - 1] = 'b';
	area2[ZZ_BUFFER_SIZE - 1] = 'a';

	/* Test memcmp. */
	if (RT_UNLIKELY(!zz_start_chrono(&chrono)))
		goto end;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		if (RT_UNLIKELY(memcmp(area1, area2, ZZ_BUFFER_SIZE) <= 0))
			goto end;
	}
	if (RT_UNLIKELY(!zz_stop_chrono("memcmp", &chrono, output_stream)))
		goto end;

	/* Test memcmp in function. */
	if (RT_UNLIKELY(!zz_start_chrono(&chrono)))
		goto end;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		if (RT_UNLIKELY(zz_memory_compare(area1, area2, ZZ_BUFFER_SIZE) <= 0))
			goto end;
	}
	if (RT_UNLIKELY(!zz_stop_chrono("memcmp in function", &chrono, output_stream)))
		goto end;

	/* Test rt_memory_Compare. */
	if (RT_UNLIKELY(!zz_start_chrono(&chrono)))
		goto end;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		if (RT_UNLIKELY(rt_memory_compare(area1, area2, ZZ_BUFFER_SIZE) <= 0))
			goto end;
	}
	if (RT_UNLIKELY(!zz_stop_chrono("rt_memory_compare", &chrono, output_stream)))
		goto end;

	/* Test RT_MEMORY_COMPARE. */
	if (RT_UNLIKELY(!zz_start_chrono(&chrono)))
		goto end;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		if (RT_UNLIKELY(RT_MEMORY_COMPARE(area1, area2, ZZ_BUFFER_SIZE) <= 0))
			goto end;
	}
	if (RT_UNLIKELY(!zz_stop_chrono("RT_MEMORY_COMPARE", &chrono, output_stream)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

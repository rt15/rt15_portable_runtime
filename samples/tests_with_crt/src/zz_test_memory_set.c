#include <rpr.h>

#include "zz_resources.h"
#include "zz_utils.h"

static void *zz_memory_set(void *area, rt_n32 value, rt_un size)
{
	return memset(area, value, size);
}

rt_s zz_test_memory_set(struct rt_output_stream *output_stream)
{
	struct rt_chrono chrono;
	rt_uchar8 buffer[ZZ_BUFFER_SIZE];
	rt_n i;
	rt_s ret = RT_FAILED;

	/* Test memset. */
	if (RT_UNLIKELY(!zz_start_chrono(&chrono)))
		goto end;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		/* VC is inlining code based on ZZ_BUFFER_SIZE. */
		memset(buffer, 1, ZZ_BUFFER_SIZE);
	}
	if (RT_UNLIKELY(!zz_stop_chrono("memset", &chrono, output_stream)))
		goto end;

	/* Test memset in function. */
	if (RT_UNLIKELY(!zz_start_chrono(&chrono)))
		goto end;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		/* Avoid inlining and size prediction, should force CRT usage. */
		zz_memory_set(buffer, 2, ZZ_BUFFER_SIZE);
	}
	if (RT_UNLIKELY(!zz_stop_chrono("memset in function", &chrono, output_stream)))
		goto end;

	/* Test rt_memory_Set. */
	if (RT_UNLIKELY(!zz_start_chrono(&chrono)))
		goto end;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		rt_memory_set(buffer, 3, ZZ_BUFFER_SIZE);
	}
	if (RT_UNLIKELY(!zz_stop_chrono("rt_memory_set", &chrono, output_stream)))
		goto end;

	/* Test RT_MEMORY_SET. */
	if (RT_UNLIKELY(!zz_start_chrono(&chrono)))
		goto end;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		/* Intrinsic should be used, CRT otherwise. */
		RT_MEMORY_SET(buffer, 4, ZZ_BUFFER_SIZE);
	}
	if (RT_UNLIKELY(!zz_stop_chrono("RT_MEMORY_SET", &chrono, output_stream)))
		goto end;

	/* Ensure that destination is used. */
	for (i = 0; i < ZZ_BUFFER_SIZE; i++) {
		if (RT_UNLIKELY(buffer[i] != 4))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

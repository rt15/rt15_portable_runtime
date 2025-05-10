#include <rpr.h>

#include "zz_resources.h"
#include "zz_utils.h"

static void *zz_memory_zero(void *area, rt_un size)
{
	return memset(area, 0, size);
}

rt_s zz_test_memory_zero(struct rt_output_stream *output_stream)
{
	struct rt_chrono chrono;
	rt_uchar8 buffer[ZZ_BUFFER_SIZE];
	rt_n i;
	rt_s ret = RT_FAILED;

	/* Test memset 0. */
	if (RT_UNLIKELY(!zz_start_chrono(&chrono)))
		goto end;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		/* VC is inlining code based on ZZ_BUFFER_SIZE. */
		memset(buffer, 0, ZZ_BUFFER_SIZE);
	}
	if (RT_UNLIKELY(!zz_stop_chrono("memset 0", &chrono, output_stream)))
		goto end;

	/* Test memset 0 in function. */
	if (RT_UNLIKELY(!zz_start_chrono(&chrono)))
		goto end;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		/* Avoid inlining and size prediction, should force CRT usage. */
		zz_memory_zero(buffer, ZZ_BUFFER_SIZE);
	}
	if (RT_UNLIKELY(!zz_stop_chrono("memset 0 in function", &chrono, output_stream)))
		goto end;

	/* Test rt_memory_zero. */
	if (RT_UNLIKELY(!zz_start_chrono(&chrono)))
		goto end;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		rt_memory_zero(buffer, ZZ_BUFFER_SIZE);
	}
	if (RT_UNLIKELY(!zz_stop_chrono("rt_memory_zero", &chrono, output_stream)))
		goto end;

	/* Test RT_MEMORY_ZERO. */
	if (RT_UNLIKELY(!zz_start_chrono(&chrono)))
		goto end;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		/* Intrinsic should be used, CRT otherwise. */
		RT_MEMORY_ZERO(buffer, ZZ_BUFFER_SIZE);
	}
	if (RT_UNLIKELY(!zz_stop_chrono("RT_MEMORY_ZERO", &chrono, output_stream)))
		goto end;

	/* Ensure that destination is used. */
	for (i = 0; i < ZZ_BUFFER_SIZE; i++) {
		if (RT_UNLIKELY(buffer[i]))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

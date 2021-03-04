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
	rt_s ret;

	/* Test memset 0. */
	if (!zz_start_chrono(&chrono))
		goto error;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		/* VC is inlining code based on ZZ_BUFFER_SIZE. */
		memset(buffer, 0, ZZ_BUFFER_SIZE);
	}
	if (!zz_stop_chrono("memset 0", &chrono, output_stream))
		goto error;

	/* Test memset 0 in function. */
	if (!zz_start_chrono(&chrono))
		goto error;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		/* Avoid inlining and size prediction, should force CRT usage. */
		zz_memory_zero(buffer, ZZ_BUFFER_SIZE);
	}
	if (!zz_stop_chrono("memset 0 in function", &chrono, output_stream))
		goto error;

	/* Test rt_memory_zero. */
	if (!zz_start_chrono(&chrono))
		goto error;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		rt_memory_zero(buffer, ZZ_BUFFER_SIZE);
	}
	if (!zz_stop_chrono("rt_memory_zero", &chrono, output_stream))
		goto error;

	/* Test RT_MEMORY_ZERO. */
	if (!zz_start_chrono(&chrono))
		goto error;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		/* Intrinsic should be used, CRT otherwise. */
		RT_MEMORY_ZERO(buffer, ZZ_BUFFER_SIZE);
	}
	if (!zz_stop_chrono("RT_MEMORY_ZERO", &chrono, output_stream))
		goto error;

	/* Ensure that destination is used. */
	for (i = 0; i < ZZ_BUFFER_SIZE; i++) {
		if (buffer[i])
			goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

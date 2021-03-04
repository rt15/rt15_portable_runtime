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
	rt_s ret;

	/* Test memset. */
	if (!zz_start_chrono(&chrono))
		goto error;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		/* VC is inlining code based on ZZ_BUFFER_SIZE. */
		memset(buffer, 1, ZZ_BUFFER_SIZE);
	}
	if (!zz_stop_chrono("memset", &chrono, output_stream))
		goto error;

	/* Test memset in function. */
	if (!zz_start_chrono(&chrono))
		goto error;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		/* Avoid inlining and size prediction, should force CRT usage. */
		zz_memory_set(buffer, 2, ZZ_BUFFER_SIZE);
	}
	if (!zz_stop_chrono("memset in function", &chrono, output_stream))
		goto error;

	/* Test rt_memory_Set. */
	if (!zz_start_chrono(&chrono))
		goto error;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		rt_memory_set(buffer, 3, ZZ_BUFFER_SIZE);
	}
	if (!zz_stop_chrono("rt_memory_Set", &chrono, output_stream))
		goto error;

	/* Test RT_MEMORY_SET. */
	if (!zz_start_chrono(&chrono))
		goto error;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		/* Intrinsic should be used, CRT otherwise. */
		RT_MEMORY_SET(buffer, 4, ZZ_BUFFER_SIZE);
	}
	if (!zz_stop_chrono("RT_MEMORY_SET", &chrono, output_stream))
		goto error;

	/* Ensure that destination is used. */
	for (i = 0; i < ZZ_BUFFER_SIZE; i++) {
		if (buffer[i] != 4)
			goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

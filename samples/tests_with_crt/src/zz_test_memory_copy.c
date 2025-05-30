#include <rpr.h>

#include "zz_resources.h"
#include "zz_utils.h"

static void *zz_memory_copy(void *source, void *destination, rt_un size)
{
	return memcpy(destination, source, size);
}

rt_s zz_test_memory_copy(struct rt_output_stream *output_stream)
{
	struct rt_chrono chrono;
	rt_uchar8 source[ZZ_BUFFER_SIZE];
	rt_uchar8 destination[ZZ_BUFFER_SIZE];
	rt_n i;
	rt_s ret = RT_FAILED;

	/* Initialize source buffer. */
	for (i = 0; i < ZZ_BUFFER_SIZE; i++) {
		source[i] = (rt_uchar8)(RT_MEMORY_MODULO_POWER_OF_TWO(i, 256));
	}

	/* Test memcpy. */
	if (RT_UNLIKELY(!zz_start_chrono(&chrono)))
		goto end;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		/* VC is inlining code based on ZZ_BUFFER_SIZE. */
		memcpy(destination, source, ZZ_BUFFER_SIZE);
	}
	if (RT_UNLIKELY(!zz_stop_chrono("memcpy", &chrono, output_stream)))
		goto end;

	/* Test memcpy in function. */
	if (RT_UNLIKELY(!zz_start_chrono(&chrono)))
		goto end;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		/* Avoid inlining and size prediction, should force CRT usage. */
		zz_memory_copy(source, destination, ZZ_BUFFER_SIZE);
	}
	if (RT_UNLIKELY(!zz_stop_chrono("memcpy in function", &chrono, output_stream)))
		goto end;

	/* Test rt_memory_Copy. */
	if (RT_UNLIKELY(!zz_start_chrono(&chrono)))
		goto end;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		rt_memory_copy(source, destination, ZZ_BUFFER_SIZE);
	}
	if (RT_UNLIKELY(!zz_stop_chrono("rt_memory_copy", &chrono, output_stream)))
		goto end;

	/* Test RT_MEMORY_COPY. */
	if (RT_UNLIKELY(!zz_start_chrono(&chrono)))
		goto end;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		/* Intrinsic should be used, CRT otherwise. */
		RT_MEMORY_COPY(source, destination, ZZ_BUFFER_SIZE);
	}
	if (RT_UNLIKELY(!zz_stop_chrono("RT_MEMORY_COPY", &chrono, output_stream)))
		goto end;

	/* Ensure that destination is used. */
	if (RT_UNLIKELY(RT_MEMORY_COMPARE(source, destination, ZZ_BUFFER_SIZE)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

#include <rpr.h>

#include "zz_resources.h"
#include "zz_utils.h"

static void *zz_memory_move(void *source, void *destination, rt_un size)
{
	return memmove(destination, source, size);
}

/**
 * Initialize source buffer (abcc).
 */
static void zz_init_buffer(rt_uchar8 *buffer)
{
	rt_un i;

	for (i = 0; i < ZZ_BUFFER_SIZE / 4; i++)
		buffer[i] = 'a';
	for (i = ZZ_BUFFER_SIZE / 4; i < ZZ_BUFFER_SIZE / 2; i++)
		buffer[i] = 'b';
	for (i = ZZ_BUFFER_SIZE / 2; i < ZZ_BUFFER_SIZE; i++)
		buffer[i] = 'c';
}

/**
 * Check result buffer (abcc -> aabc, then aabc -> aaac as we call move in a loop).
 */
static rt_s zz_check_buffer(rt_uchar8 *buffer)
{
	rt_un i;
	rt_s ret;

	for (i = 0; i < ZZ_BUFFER_SIZE * 3 / 4; i++)
		if (buffer[i] != 'a') goto error;
	for (i = ZZ_BUFFER_SIZE * 3 / 4; i < ZZ_BUFFER_SIZE; i++)
		if (buffer[i] != 'c') goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_memory_move(struct rt_output_stream *output_stream)
{
	struct rt_chrono chrono;
	rt_uchar8 source[ZZ_BUFFER_SIZE];
	rt_uchar8 *destination;
	rt_n i;
	rt_s ret;

	/* We will copy ab on bc. */
	destination = &source[ZZ_BUFFER_SIZE / 4];

	/* Test memmove. */
	zz_init_buffer(source);
	if (!zz_start_chrono(&chrono))
		goto error;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		/* VC is inlining code based on ZZ_BUFFER_SIZE. */
		memmove(destination, source, ZZ_BUFFER_SIZE / 2);
	}
	if (!zz_stop_chrono("memmove", &chrono, output_stream))
		goto error;
	if (!zz_check_buffer(source))
		goto error;

	/* Test memmove in function. */
	zz_init_buffer(source);
	if (!zz_start_chrono(&chrono))
		goto error;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		/* Avoid inlining and size prediction, should force CRT usage. */
		zz_memory_move(source, destination, ZZ_BUFFER_SIZE / 2);
	}
	if (!zz_stop_chrono("memmove in function", &chrono, output_stream))
		goto error;
	if (!zz_check_buffer(source))
		goto error;

	/* Test rt_memory_Move. */
	zz_init_buffer(source);
	if (!zz_start_chrono(&chrono))
		goto error;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		rt_memory_move(source, destination, ZZ_BUFFER_SIZE / 2);
	}
	if (!zz_stop_chrono("rt_memory_Move", &chrono, output_stream))
		goto error;
	if (!zz_check_buffer(source))
		goto error;

	/* Test RT_MEMORY_MOVE. */
	zz_init_buffer(source);
	if (!zz_start_chrono(&chrono))
		goto error;
	for (i = 0; i < ZZ_TESTS_COUNT; i++) {
		/* Intrinsic should be used, CRT otherwise. */
		RT_MEMORY_MOVE(source, destination, ZZ_BUFFER_SIZE / 2);
	}
	if (!zz_stop_chrono("RT_MEMORY_MOVE", &chrono, output_stream))
		goto error;
	if (!zz_check_buffer(source))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

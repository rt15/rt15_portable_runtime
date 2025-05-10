#include <rpr.h>

/**
 * @file
 * Should consume 100% of the CPU on all logical cores for the test duration.
 */

#define ZZ_DATA_SIZE 1000000
#define ZZ_TEST_DURATION 30

static rt_b zz_continue = RT_TRUE;

static rt_un32 RT_STDCALL zz_test_perf_callback(RT_UNUSED void *parameter)
{
	rt_un *zz_data = (rt_un*)parameter;
	rt_un loop_count = 0;
	rt_un sum = 0;
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_un i;
	rt_s ret = RT_FAILED;

	while (zz_continue) {
		for (i = 0; i < ZZ_DATA_SIZE; i++) {
			sum += zz_data[i];
			if (!zz_continue)
				break;
		}
		loop_count++;
	}

	buffer_size = 0;
	if (RT_UNLIKELY(!rt_char_append_un(loop_count, 10, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append(_R(" (sum = "), 8, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append_un(sum, 10, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append(_R(")\n"), 2, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size))) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_perf(void)
{
	rt_un **zz_data = RT_NULL;
	rt_un logical_cpu_count;
	struct rt_thread *threads = RT_NULL;
	rt_b *thread_created = RT_NULL;
	struct rt_chrono chrono;
	rt_un micro_seconds;
	rt_un i, j;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_system_info_get_logical_cpu_count(&logical_cpu_count))) goto end;

	if (RT_UNLIKELY(!rt_static_heap_alloc((void**)&zz_data, logical_cpu_count * sizeof(rt_un*)))) goto end;
	for (i = 0; i < logical_cpu_count; i++)
		zz_data[i] = RT_NULL;

	for (i = 0; i < logical_cpu_count; i++) {
		if (RT_UNLIKELY(!rt_static_heap_alloc((void**)&zz_data[i], ZZ_DATA_SIZE * sizeof(rt_un))))
			goto end;
		for (j = 0; j < ZZ_DATA_SIZE; j++) {
			if (RT_UNLIKELY(!rt_random_get_unsigned_integer(&zz_data[i][j])))
				goto end;
		}
	}

	if (RT_UNLIKELY(!rt_static_heap_alloc((void**)&thread_created, logical_cpu_count * sizeof(rt_b)))) goto end;
	for (i = 0; i < logical_cpu_count; i++)
		thread_created[i] = RT_FALSE;

	if (RT_UNLIKELY(!rt_static_heap_alloc((void**)&threads, logical_cpu_count * sizeof(struct rt_thread)))) goto end;

	for (i = 0; i < logical_cpu_count; i++) {
		if (RT_UNLIKELY(!rt_thread_create(&threads[i], &zz_test_perf_callback, zz_data[i])))
			goto end;
		thread_created[i] = RT_TRUE;
	}

	if (RT_UNLIKELY(!rt_chrono_create(&chrono))) goto end;

	while (RT_TRUE) {
		rt_sleep_sleep(1);

		if (RT_UNLIKELY(!rt_chrono_get_duration(&chrono, &micro_seconds))) goto end;
		if (RT_UNLIKELY(micro_seconds >= ZZ_TEST_DURATION * 1000000)) break;
	}

	ret = RT_OK;
end:
	zz_continue = RT_FALSE;

	if (thread_created) {
		for (i = 0; i < logical_cpu_count; i++) {
			if (thread_created[i]) {
				if (RT_UNLIKELY(!rt_thread_join_and_check(&threads[i])))
					ret = RT_FAILED;
				if (RT_UNLIKELY(!rt_thread_free(&threads[i])))
					ret = RT_FAILED;
			}
		}
	}

	if (RT_UNLIKELY(!rt_static_heap_free((void**)&threads)))
		ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_static_heap_free((void**)&thread_created)))
		ret = RT_FAILED;

	if (zz_data) {
		for (i = 0; i < logical_cpu_count; i++) {
			if (zz_data[i]) {
				if (RT_UNLIKELY(!rt_static_heap_free((void**)&zz_data[i])))
					ret = RT_FAILED;
			}
		}
	}

	if (RT_UNLIKELY(!rt_static_heap_free((void**)&zz_data)))
		ret = RT_FAILED;

	return ret;
}

#include <rpr.h>

/**
 * Displays <tt>label</tt> followed by <tt>nanoseconds</tt> as seconds with 9 decimals.
 *
 * <p>
 * rt_char_append_un cannot be used directly with nanoseconds as rt_un is 32 bits on 32 bits systems.<br>
 * As a result, seconds and remaining nanoseconds are appended separately.
 * </p>
 */
static rt_s zz_manual_test_monotonic_clock_display(const rt_char *label, rt_un64 nanoseconds)
{
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_char digits[16];
	rt_un digits_size;
	rt_s ret = RT_FAILED;

	buffer_size = rt_char_get_size(label);
	if (RT_UNLIKELY(!rt_char_copy(label, buffer_size, buffer, RT_CHAR_HALF_BIG_STRING_SIZE))) goto end;

	if (RT_UNLIKELY(!rt_char_append_un((rt_un)(nanoseconds / 1000000000), 10, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append_char(_R('.'), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;

	/* Pad the remaining nanoseconds with zeros up to 9 digits. */
	digits_size = 0;
	if (RT_UNLIKELY(!rt_char_append_un((rt_un)(nanoseconds % 1000000000), 10, digits, 16, &digits_size))) goto end;
	if (RT_UNLIKELY(!rt_char_left_pad(digits, digits_size, _R('0'), 9, digits, 16, &digits_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append(digits, digits_size, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;

	if (RT_UNLIKELY(!rt_char_append(_R(" s\n"), 3, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;

	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size))) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_manual_test_monotonic_clock(void)
{
	rt_un64 before;
	rt_un64 after;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_monotonic_clock_get(&before))) goto end;
	rt_sleep_sleep(100);
	if (RT_UNLIKELY(!rt_monotonic_clock_get(&after))) goto end;

	if (RT_UNLIKELY(!zz_manual_test_monotonic_clock_display(_R("Monotonic clock before sleep = "), before))) goto end;
	if (RT_UNLIKELY(!zz_manual_test_monotonic_clock_display(_R("Monotonic clock after sleep = "), after))) goto end;
	if (RT_UNLIKELY(!zz_manual_test_monotonic_clock_display(_R("Monotonic clock difference (100 ms sleep) = "), after - before))) goto end;

	ret = RT_OK;
end:
	return ret;
}

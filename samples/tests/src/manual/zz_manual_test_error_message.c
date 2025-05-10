#include <rpr.h>

static rt_s zz_manual_test_display_error_message(const rt_char *error_name, rt_un error_number)
{
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret = RT_FAILED;

	rt_error_set_last(error_number);

	buffer_size = rt_char_get_size(error_name);
	if (RT_UNLIKELY(!rt_char_copy(error_name, buffer_size, buffer, RT_CHAR_HALF_BIG_STRING_SIZE))) goto end;
	if (RT_UNLIKELY(!rt_char_append(_R(" = "), 3, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_error_message_append_last(buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(rt_char_get_size(buffer) != buffer_size)) goto end;
	if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size))) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_manual_test_error_message(void)
{
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_console_write_str(_R("## Error messages:\n")))) goto end;

	/* Intentionally generate an error (RT_ERROR_INSUFFICIENT_BUFFER). */
	if (RT_UNLIKELY(rt_char_copy(_R("Too long string."), 16, buffer, 3)))
		goto end;

	if (RT_UNLIKELY(!rt_error_message_write_last(RT_NULL))) goto end;
	if (RT_UNLIKELY(!rt_error_message_write_last(_R("Normal error: ")))) goto end;

	if (RT_UNLIKELY(!rt_last_error_message_set_with_last_error())) goto end;
	if (RT_UNLIKELY(!rt_last_error_message_write(_R("Normal error too: ")))) goto end;

	if (RT_UNLIKELY(!zz_manual_test_display_error_message(_R("RT_ERROR_SUCCESS"),             RT_ERROR_SUCCESS)))             goto end;
	if (RT_UNLIKELY(!zz_manual_test_display_error_message(_R("RT_ERROR_INSUFFICIENT_BUFFER"), RT_ERROR_INSUFFICIENT_BUFFER))) goto end;
	if (RT_UNLIKELY(!zz_manual_test_display_error_message(_R("RT_ERROR_BAD_ARGUMENTS"),       RT_ERROR_BAD_ARGUMENTS)))       goto end;
	if (RT_UNLIKELY(!zz_manual_test_display_error_message(_R("RT_ERROR_NOT_ENOUGH_MEMORY"),   RT_ERROR_NOT_ENOUGH_MEMORY)))   goto end;
	if (RT_UNLIKELY(!zz_manual_test_display_error_message(_R("RT_ERROR_FILE_ALREADY_EXISTS"), RT_ERROR_FILE_ALREADY_EXISTS))) goto end;
	if (RT_UNLIKELY(!zz_manual_test_display_error_message(_R("RT_ERROR_ALREADY_EXISTS"),      RT_ERROR_ALREADY_EXISTS)))      goto end;
	if (RT_UNLIKELY(!zz_manual_test_display_error_message(_R("RT_ERROR_FUNCTION_FAILED"),     RT_ERROR_FUNCTION_FAILED)))     goto end;
	if (RT_UNLIKELY(!zz_manual_test_display_error_message(_R("RT_ERROR_ARITHMETIC_OVERFLOW"), RT_ERROR_ARITHMETIC_OVERFLOW))) goto end;
	if (RT_UNLIKELY(!zz_manual_test_display_error_message(_R("RT_ERROR_WOULD_BLOCK"),         RT_ERROR_WOULD_BLOCK)))         goto end;
	if (RT_UNLIKELY(!zz_manual_test_display_error_message(_R("RT_ERROR_SOCKET_WOULD_BLOCK"),  RT_ERROR_SOCKET_WOULD_BLOCK)))  goto end;
	if (RT_UNLIKELY(!zz_manual_test_display_error_message(_R("RT_ERROR_FILE_NOT_FOUND"),      RT_ERROR_FILE_NOT_FOUND)))      goto end;
	if (RT_UNLIKELY(!zz_manual_test_display_error_message(_R("RT_ERROR_DIR_NOT_FOUND"),       RT_ERROR_DIR_NOT_FOUND)))       goto end;

	if (RT_UNLIKELY(!rt_console_write_str_with_size(_R("\n"), 1))) goto end;

	ret = RT_OK;
end:
	return ret;
}

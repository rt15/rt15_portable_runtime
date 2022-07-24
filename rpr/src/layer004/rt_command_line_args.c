#include "layer004/rt_command_line_args.h"

#include "layer003/rt_char.h"

/**
 * Search the first occurence of <tt>searched</tt> in <tt>strings</tt>.
 *
 * <p>
 * Should not be used with large arrays.<br>
 * Use sorted array and quicksort for large arrays.
 * </p>
 *
 * @param strings Null terminated array of null terminated strings. Can be RT_NULL.
 * @return index of <tt>searched</tt> in <tt>strings</tt> or RT_TYPE_MAX_UN if not found.
 */
static rt_un rt_command_line_args_search_string_in_strings(const rt_char *strings[], const rt_char *searched)
{
	rt_un result;
	rt_un searched_size;
	rt_un i;

	result = RT_TYPE_MAX_UN;

	if (strings) {
		searched_size = rt_char_get_size(searched);
		i = 0;
		while (strings[i]) {
			if (rt_char_equals(strings[i], rt_char_get_size(strings[i]), searched, searched_size)) {
				/* We found the string. */
				result = i;
				break;
			}
			i++;
		}
	}

	return result;
}

/**
 * Take as input one of the <tt>arg</tt> and analyze it, finding out its type.<br>
 * It can be a short or long option, a non-option, or the end of options (--).
 *
 * <p>
 * Beware that an argument alone cannot be analyzed as it can be an option value.
 * </p>
 *
 * @param arg An argument from argv.
 * @param arg_type Returns the type of argument.
 * @param options_buffer In case of short option receive all concatenated short options plus possible value. In case of long option receive the long option name.
 * @param options_buffer_size Size of the string in <tt>options_buffer</tt> if any. Output parameter.
 * @param value In case of long option, point on the value if it was concatenated with option name and '='.
 */
static rt_s rt_command_line_args_analyze(const rt_char *arg, enum rt_command_line_args_type *arg_type,
					 rt_char *options_buffer, rt_un options_buffer_capacity, rt_un *options_buffer_size,
					 const rt_char **value)
{
	rt_un arg_size = rt_char_get_size(arg);
	rt_un i;
	rt_s ret;

	if (arg_size <= 1) {
		/* Empty string or only one character like 'a' or '-'. */
		*arg_type = RT_COMMAND_LINE_ARGS_TYPE_NON_OPTION;
		*value = arg;
	} else {
		if (arg[0] == '-') {
			if (arg[1] == '-') {
				if (arg_size == 2) {
					/* '--' -> end of options. */
					*arg_type = RT_COMMAND_LINE_ARGS_TYPE_END_OF_OPTIONS;
					*value = RT_NULL;
				} else {
					/* '--XXXX', long option. */
					*arg_type = RT_COMMAND_LINE_ARGS_TYPE_LONG;

					/* Search for equals. */
					for (i = 0; i < arg_size; i++) {
						if (arg[i] == _R('='))
							break;
					}
					/* Is there an equals sign? */
					if (i != arg_size) {
						/* Copy until equals sign, skipping '--' at the beginning. */
						*options_buffer_size = i - 2;
						if (!rt_char_copy(&arg[2], *options_buffer_size, options_buffer, options_buffer_capacity))
							goto error;

						/* Value is after equals sign. */
						*value = &arg[i + 1];
					} else {
						/* Copy the whole arg without '--'. */
						*options_buffer_size = arg_size - 2;
						if (!rt_char_copy(&arg[2], *options_buffer_size, options_buffer, options_buffer_capacity))
							goto error;

						/* No value directly concatenated after the option and '='. */
						*value = RT_NULL;
					}
				}
			} else {
				/* '-XXXX', short option. */
				/* Simply return the short option, maybe concatenated with some others short options and maybe concatenated with a value. */
				*arg_type = RT_COMMAND_LINE_ARGS_TYPE_SHORT;

				*options_buffer_size = arg_size - 1;
				if (!rt_char_copy(&arg[1], *options_buffer_size, options_buffer, options_buffer_capacity))
					goto error;

				*value = RT_NULL;
			}
		} else {
			/* No dash -> non-option. */
			*arg_type = RT_COMMAND_LINE_ARGS_TYPE_NON_OPTION;
			*value = arg;
		}
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_command_line_args_parse(rt_un *argc, const rt_char *argv[], rt_command_line_args_parse_callback_t callback, void *context,
				const rt_char *short_options_without_arg,  const rt_char *short_options_with_optional_arg,  const rt_char *short_options_with_arg,
				const rt_char *long_options_without_arg[], const rt_char *long_options_with_optional_arg[], const rt_char *long_options_with_arg[],
				rt_un *non_options_index)
{
	enum rt_command_line_args_type arg_type;
	rt_char options_buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un options_buffer_size;
	const rt_char *value;
	rt_char short_option;
	rt_b valid;
	enum rt_command_line_args_value_cardinality value_cardinality;
	rt_un non_options = RT_TYPE_MAX_UN;
	const rt_char *non_option;
	rt_un last_option;
	const rt_char *end_of_options;
	rt_un i;
	rt_un j;
	rt_un k;
	rt_s ret;

	/* There might be no non-options. */
	*non_options_index = RT_TYPE_MAX_UN;

	for (i = 1; i < *argc; i++) {
		options_buffer_size = 0;

		if (!rt_command_line_args_analyze(argv[i], &arg_type,
						  options_buffer,   RT_CHAR_HALF_BIG_STRING_SIZE, &options_buffer_size,
						  &value))
			goto error;

		if (arg_type == RT_COMMAND_LINE_ARGS_TYPE_SHORT) {
			j = 0;
			while (j < options_buffer_size) {
				short_option = options_buffer[j];

				if (short_options_without_arg != RT_NULL &&
				    rt_char_search_char(short_options_without_arg, short_option) != RT_TYPE_MAX_UN) {
					valid = RT_TRUE;
					value_cardinality = RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_NONE;
					value = RT_NULL;
					/* No value, we will switch to the next character. */
				} else if (short_options_with_optional_arg != RT_NULL &&
					   rt_char_search_char(short_options_with_optional_arg, short_option) != RT_TYPE_MAX_UN) {
					valid = RT_TRUE;
					value_cardinality = RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_OPTIONAL;
					/* Optional available values for short options are always concatenated to the option. */
					if (j == options_buffer_size - 1) {
						value = RT_NULL;
					} else {
						value = &options_buffer[j + 1];
						/* Break the loop. */
						j = options_buffer_size;
					}
				} else if (short_options_with_arg != RT_NULL &&
					   rt_char_search_char(short_options_with_arg, short_option) != RT_TYPE_MAX_UN) {
					valid = RT_TRUE;
					value_cardinality = RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_REQUIRED;

					/* The mandatory value can be either concatenated or not. */
					if (j == options_buffer_size - 1) {
						/* Retrieve next argument as value if available. */
						if (i + 1 < *argc) {
							i++;
							value = argv[i];
						}
					} else {
						/* Concatenated value. */
						value = &options_buffer[j + 1];
						/* Break the loop. */
						j = options_buffer_size;
					}
				} else {
					valid = RT_FALSE;
					value_cardinality = RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_NONE;
					value = RT_NULL;
				}

				if (!callback(arg_type, valid, short_option, RT_NULL, value_cardinality, value, context))
					goto error;
				j++;
			}
		} else if (arg_type == RT_COMMAND_LINE_ARGS_TYPE_LONG) {
			if (rt_command_line_args_search_string_in_strings(long_options_without_arg, options_buffer) != RT_TYPE_MAX_UN) {
				valid = RT_TRUE;
				value_cardinality = RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_NONE;
			} else if (rt_command_line_args_search_string_in_strings(long_options_with_optional_arg, options_buffer) != RT_TYPE_MAX_UN) {
				valid = RT_TRUE;
				value_cardinality = RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_OPTIONAL;
			} else if (rt_command_line_args_search_string_in_strings(long_options_with_arg, options_buffer) != RT_TYPE_MAX_UN) {
				valid = RT_TRUE;
				value_cardinality = RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_REQUIRED;

				/* If the value has not been provided with equals while mandatory (Optional arguments needs '='). */
				if (!value) {
					/* We do need a value, just peek it if available. */
					if (i + 1 < *argc) {
						i++;
						value = argv[i];
					}
				}
			} else {
				valid = RT_FALSE;
				value_cardinality = RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_NONE;
			}

			if (!callback(arg_type, valid, 0, options_buffer, value_cardinality, value, context))
				goto error;

		} else if (arg_type == RT_COMMAND_LINE_ARGS_TYPE_NON_OPTION) {
			/* This is a non-option that can be after all options or that can be between options. */
			/* It must be pushed after all options and after all previously pushed non-options. */

			/* Remember non-option. */
			non_option = argv[i];

			/* If no non-options have been pushed yet. */
			if (non_options == RT_TYPE_MAX_UN) {
				/* We must find the last option. */

				last_option = -1;
				for (k = i + 1; k < *argc; k++) {
					if (!rt_command_line_args_analyze(argv[k], &arg_type,
									  options_buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &options_buffer_size,
									  &value))
						goto error;

					if (arg_type == RT_COMMAND_LINE_ARGS_TYPE_SHORT) {
						last_option = k;

						j = 0;
						while (j < options_buffer_size) {
							short_option = options_buffer[j];

							if (short_options_with_optional_arg != RT_NULL &&
							    rt_char_search_char(short_options_with_optional_arg, short_option) != RT_TYPE_MAX_UN) {
								/* Optional value should be concatenated, skip it. */
								break;
							} else if (short_options_with_arg != RT_NULL &&
								   rt_char_search_char(short_options_with_arg, short_option) != RT_TYPE_MAX_UN) {
								/* If the mandatory value is not concatenated. */
								if (j == options_buffer_size - 1) {
									/* Skip the argument. */
									k++;
									last_option = k;
								} else {
									/* The other letters are the mandatory argument. */
									break;
								}
							}
							j++;
						}
					} else if (arg_type == RT_COMMAND_LINE_ARGS_TYPE_LONG) {
						/* We skip an argument only if it is an option with arg. Options with optional arguments require equals. */
						if (rt_command_line_args_search_string_in_strings(long_options_with_arg, options_buffer) != RT_TYPE_MAX_UN) {
							if (!value) {
								/* Skip the argument if not provided yet using '='. */
								k++;
							}
						}
						last_option = k;
					} else if (arg_type == RT_COMMAND_LINE_ARGS_TYPE_END_OF_OPTIONS) {
						/* There won't be options after '--'. */
						break;
					}
				}

				/* If there is no last option. */
				if (last_option == -1) {
					/* There is no remaining options. */
					/* Adjust non-options index to current non-option if needed. */
					if (*non_options_index > i) {
						*non_options_index = i;
					}

					/* We do not break as there might be still an end of options to skip. */
				} else {
					/* Shift arguments and replace last option. */

					for (k = i; k < last_option; k++) {
						argv[k] = argv[k + 1];
					}

					/* Put the non-option at the end. */
					argv[k] = non_option;

					/* Adjust non-options index to current non-option if needed. */
					if (*non_options_index > k) {
						*non_options_index = k;
					}

					/* Following non-options will be pushed after this one. */
					non_options = k;

					/* We replaced current argument by next argument. */
					i--;
				}
			} else {
				/* We have already pushed a non-option at non_options. */
				/* We must shift arguments until non_options included and replace it. */

				/* If the non-option has not be handled already. */
				if (i < *non_options_index) {
					/* Shift until last pushed non-option. */
					for (k = i; k < non_options; k++) {
						argv[k] = argv[k + 1];
					}

					/* Put the non-option at the end. */
					argv[k] = non_option;

					/* First non-option has been shifted. */
					(*non_options_index)--;

					/* We replaced current argument by next argument. */
					i--;
				}
			}
		} else {
			/* RT_COMMAND_LINE_ARGS_TYPE_END_OF_OPTIONS */
			/* All remaining arguments are non-options that must be shifted to the left. */
			/* The end of option '--' must be put at the end of argv and argc must be decremented. */

			/* Remember end of option. */
			end_of_options = argv[i];

			/* Shift remaining arguments to the left. */
			for (k = i; k < *argc - 1; k++) {
				argv[k] = argv[k + 1];
			}

			/* Put end of options at the end. */
			argv[k] = end_of_options;

			/* Skip end of options. */
			(*argc)--;

			/* Adjust non options index. */
			if (*non_options_index > i) {
				*non_options_index = i;
			}

			/* All arguments have been handled. */
			break;
		}
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

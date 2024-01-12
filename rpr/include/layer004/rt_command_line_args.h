#ifndef RT_COMMAND_LINE_ARGS_H
#define RT_COMMAND_LINE_ARGS_H

#include "layer000/rt_types.h"

/**
 * @file
 * Parse command line arguments.
 *
 * <p>
 * Arguments are:
 * </p>
 * <ul>
 *   <li>Short options (One letter like -a).</li>
 *   <li>Long options (Dash separated words like --block-size).</li>
 *   <li>Non-options like u+rx or /tmp.</li>
 * </ul>
 *
 * <p>
 * Options can have no argument, an optional argument or a required argument.<br>
 * Arguments are re-ordered so that options precede non-options. 'mv src -f dest' become 'mv -f src dest'<br>
 * Options can appear multiple times.<br>
 * A single dash '-' is a non-option typically used to represent one of the standard input streams.<br>
 * Two dashes '--' terminates all options, any following arguments are considered non-option (Including any other '--').
 * </p>
 *
 * <p>
 * For short options:<br>
 * Value of an option can be separated from the option by a space or not: 'touch -r file1 file2' or 'touch -rfile1 file2'.<br>
 * Options can be concatenated after a single dash and can finish with an option with an optional/mandatory value: 'rm -rf dir', 'touch -mrRef.txt New.txt', 'touch -mr Ref.txt New.txt'.<br>
 * If the value is optional and available, it must be concatenated with the option: 'awk -dvars.txt '{ print }' readme.txt' is valid but 'awk -d vars.txt '{ print }' readme.txt' is not.<br>
 * When options are concatenated, all letters after the first option with optional/mandatory value are considered to be the value of this option (There is no more concatenated options): 'awk -PdO '{ print }' readme.txt' print variables in O file.
 * </p>
 *
 * <p>
 * For long options:<br>
 * Argument can be separated by a space or an equal: '--var-name=value' or '--var-name value'.<br>
 * Notice that 'ls --color never' search in never folder while 'ls --color=never' deactivate colors (Color option is defined as --color[=WHEN]).<br>
 * So when a long option has an optional argument, '=' must be used to set the argument.<br>
 * <a href="https://www.gnu.org/prep/standards/html_node/Option-Table.html#Option-Table">GNU classical long options.</a>
 * </p>
 *
 * <p>
 * Some examples:
 * </p>
 * <pre>
 * ps -u, ps u -> Display the user.
 * ps -u root -> Display all processes for root user.
 * rm -dv temp -> Remove temp directory in verbose mode.
 * mkdir temp1 temp2/rm -d temp1 temp2 -> Create and remove to directories.
 * touch -- -name -> Create -name file.
 * rm -r -r -R --recursive foo -> You can use same option several times.
 * ps -u oracle -u root u -> Display processes of root and oracle users with user names.
 * ls -T5 -> 5 tabs for ls command.
 * </pre>
 */

enum rt_command_line_args_value_cardinality {
	RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_NONE,
	RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_OPTIONAL,
	RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_REQUIRED
};

enum rt_command_line_args_type {
	RT_COMMAND_LINE_ARGS_TYPE_SHORT,
	RT_COMMAND_LINE_ARGS_TYPE_LONG,
	RT_COMMAND_LINE_ARGS_TYPE_NON_OPTION,
	RT_COMMAND_LINE_ARGS_TYPE_END_OF_OPTIONS
};

/**
 * @param arg_type RT_COMMAND_LINE_ARGS_TYPE_SHORT or RT_COMMAND_LINE_ARGS_TYPE_LONG.
 * @param valid Whether the option has been recognized. Value cardinality is not taken in account.
 * @param short_option Short option letter if arg_type is RT_COMMAND_LINE_ARGS_TYPE_SHORT.
 * @param long_option Long option name if arg_type is RT_COMMAND_LINE_ARGS_TYPE_LONG.
 * @param value_cardinality One of RT_COMMAND_LINE_ARGS_VALUE_CARDINALITY_XXXX.
 * @param value Value for the option, can be null even for options with an argument. Can be valued even for options with no args if the user wrongly put a value.
 */
typedef rt_s (*rt_command_line_args_parse_callback_t)(enum rt_command_line_args_type arg_type, rt_b valid, rt_char short_option, const rt_char *long_option,
						enum rt_command_line_args_value_cardinality value_cardinality, const rt_char *value, void *context);

/**
 *
 * <p>
 * First argument is ignored as it should be the process name.
 * </p>
 *
 * <p>
 * Long options must not contain prepended '--'.
 * </p>
 *
 * @param argc Can be updated if one of the argument is '--'.
 * @param argv Beware that arguments can be re-ordered so watch out for concurrency and writing rights.
 * @param short_options_without_arg Short options without argument. Can be RT_NULL.
 * @param short_options_with_optional_arg Short options with optional argument. Can be RT_NULL.
 * @param short_options_with_arg Short options with required argument. Can be RT_NULL.
 * @param long_options_without_arg Long options without argument. Can be RT_NULL.
 * @param long_options_with_optional_arg Long options with optional argument. Can be RT_NULL.
 * @param long_options_with_arg Long options without argument. Can be RT_NULL.
 * @param non_options_index Index in argv of the first non-option argument. All non-options are pushed at the end of argv. Set to RT_TYPE_MAX_UN if there are no non-options.
 */
RT_API rt_s rt_command_line_args_parse(rt_un *argc, const rt_char *argv[], rt_command_line_args_parse_callback_t callback, void *context,
				const rt_char *short_options_without_arg,  const rt_char *short_options_with_optional_arg,  const rt_char *short_options_with_arg,
				const rt_char *long_options_without_arg[], const rt_char *long_options_with_optional_arg[], const rt_char *long_options_with_arg[],
				rt_un *non_options_index);

#endif /* RT_COMMAND_LINE_ARGS_H */

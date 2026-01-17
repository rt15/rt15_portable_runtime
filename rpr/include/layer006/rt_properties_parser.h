#ifndef RT_PROPERTIES_PARSER_H
#define RT_PROPERTIES_PARSER_H

#include "layer000/rt_types.h"

/**
 * @file
 * See wikipedia for a description of the properties file format.
 *
 * <p>
 * The callback will always be called with a (possibly empty) value after a key.<br>
 * As the separator is optional, the callback might not be called with a separator between a key and a value.<br>
 * The input string is not modified, so escape characters are not removed from keys, unicode sequences are not converted, multi-line values are not merged, etc.
 * </p>
 *
 * <p>
 * Spaces before and after the key are ignored.<br>
 * Spaces before the value are ignored. Trailing spaces are not.
 * </p>
 */

enum rt_properties_parser_part_type {
	RT_PROPERTIES_PARSER_PART_TYPE_KEY,
	RT_PROPERTIES_PARSER_PART_TYPE_SEPARATOR,
	RT_PROPERTIES_PARSER_PART_TYPE_VALUE,
	RT_PROPERTIES_PARSER_PART_TYPE_BLANKS,
	RT_PROPERTIES_PARSER_PART_TYPE_COMMENT
};

typedef rt_s (*rt_properties_parser_callback_t)(enum rt_properties_parser_part_type part_type, const rt_char *str, rt_un index, rt_un size, void *context);

RT_API rt_s rt_properties_parser_parse(const rt_char *str, rt_un str_size, rt_properties_parser_callback_t callback, void *context);

#endif /* RT_PROPERTIES_PARSER_H */

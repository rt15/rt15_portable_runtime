#ifndef RT_FILE_PATH_H
#define RT_FILE_PATH_H

#include "layer000/rt_types.h"
#include "layer003/rt_char.h"

/**
 * Maximum file depth supported by the library.<br>
 * Many Windows programs are limited to MAX_PATH, 260 characters.
 *
 * <p>
 * This is set so that a function can have 2 local variable paths and few other things without reaching 4k size.
 * </p>
 */
#define RT_FILE_PATH_SIZE RT_CHAR_HALF_BIG_STRING_SIZE

/**
 * Maximum length for a file or directory name (Without path).<br>
 * The limit on an NTFS file system is 256.
 */
#define RT_FILE_PATH_NAME_SIZE 256

#ifdef RT_DEFINE_WINDOWS
#define RT_FILE_PATH_SEPARATOR L'\\'
#define RT_FILE_PATH_IS_SEPARATOR(X) (X == L'\\' || X == L'/')
#else
#define RT_FILE_PATH_SEPARATOR '/'
#define RT_FILE_PATH_IS_SEPARATOR(X) (X == '/')
#endif

enum rt_file_path_type {
	RT_FILE_PATH_TYPE_NONE,
	RT_FILE_PATH_TYPE_DIR,
	RT_FILE_PATH_TYPE_FILE
};

typedef rt_s (*rt_file_path_browse_callback_t)(const rt_char *path, rt_un type, void *context);

/**
 * @param children_first If RT_TRUE, the callback is called with children first.
 */
rt_s rt_file_path_browse(const rt_char *dir_path, rt_file_path_browse_callback_t callback, rt_b recursively, rt_b children_first, void *context);

/**
 * @param buffer_size The number of characters in the buffer (out parameter).
 */
rt_s rt_file_path_get_current_dir(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

rt_s rt_file_path_set_current_dir(const rt_char *dir_path);

/**
 * Append a separator to <tt>dir_path</tt> if there is not already one.<br>
 * Should always be used as "/" is a valid path.<br>
 * So we cannot blindly add a separator to a path.
 *
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
rt_s rt_file_path_append_separator(rt_char *dir_path, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * Convert input <tt>path</tt> into a full path.<br>
 * A full path starts from the root and does not contain current directory "." or parent directory "..".
 *
 * <p>
 * On Windows, if <tt>path</tt> starts with a separator, then it is an absolute path from the current drive.<br>
 * <tt>\Directory</tt> results in <tt>d:\Directory</tt>.
 * </p>
 *
 * <p>
 * On Windows, paths without separators after the drive like <tt>c:Directory</tt> means a relative path from the current directory of the <tt>c:</tt> drive.<br>
 * This kind of path are not common and available only in the command prompt because they are based on environment variable like %=c:%.
 * </p>
 *
 * <p>
 * On Windows, a full path and an absolute path are two different things:
 * </p>
 * <ul>
 * <li>A full path does not depend on the current drive or the current directory.</li>
 * <li>An absolute path does not depend on the current directory but can depend of the current drive.</li>
 * </ul>
 * <p>
 * For example <tt>\Directory</tt> is an absolute path from the current drive, so not a full path.
 * </p>
 *
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
rt_s rt_file_path_full(rt_char *path, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * Can be called to check whether calling <tt>rt_file_path_namespace</tt> make sense.
 */
rt_b rt_file_path_is_namespaced(const rt_char *path);

/**
 * Many Win32 API functions must be called with namespace prefixed paths to handles paths longer than MAX_PATH.
 *
 * <p>
 * There are only two prefixes:
 * </p>
 * <ul>
 * <li>"\\?\" for files.</li>
 * <li>"\\.\" for devices.</li>
 * </ul>
 *
 * <p>
 * A file can be:
 * </p>
 * <ul>
 * <li>In a volume of the local computer: "C:\Directory" must be "\\?\C:\Directory"</li>
 * <li>In a shared directory of another computer (UNC path): "\\server\Directory" must be "\\?\UNC\server\Directory".</li>
 * </ul>
 *
 * <p>
 * The namespaced paths must be full path, so <tt>rt_file_path_namespace</tt> calls <tt>rt_file_path_full</tt>.<br>
 * Also for namespaced paths, slash "/" is not supported as separator. <tt>rt_file_path_namespace</tt> replaces slashes by backslashes.
 * </p>
 *
 * <p>
 * Namespaced paths must not use the current directory "." or the parent directory "..".
 * </p>
 *
 * <p>
 * This is no more mandatory under Windows 10 configured with a specific registry key (not by default).
 * </p>
 *
 * @param buffer_size In/out parameter.
 */
rt_s rt_file_path_namespace(rt_char *path, rt_un buffer_capacity, rt_un *buffer_size);


rt_s rt_file_path_strip_namespace(rt_char *path, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * Determine if <tt>path</tt> is referencing a file, a directory, or nothing.
 */
rt_b rt_file_path_get_type(const rt_char *path, enum rt_file_path_type *type);

/**
 * Find index of the last interesting separator.<br>
 * Trailing separators are ignored.<br>
 * Return RT_TYPE_MAX_UN if the path does not have separators or ends with separators.<br>
 * Convenient function used internally to compute:
 * <ul>
 * <li>The parent directory.</li>
 * <li>The file name</li>
 * </ul>
 */
rt_un rt_file_path_get_last_separator_index(const rt_char *path, rt_un path_size);

/**
 * First compute the full path then the parent directory of this full path.
 *
 * <p>
 * By classical convention, the parent of the root folder is the root folder.
 * </p>
 *
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
rt_s rt_file_path_get_parent(rt_char *path, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * Return the path to the current executable file path.
 *
 * @param buffer_size The number of characters in the buffer (out parameter).
 */
rt_s rt_file_path_get_executable_path(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 * Return the file or directory name from given <tt>path</tt>.
 *
 * <p>
 * This function works similarly to Linux <tt>basename</tt>.<br>
 * Under Visual Studio, similar function is <tt>_wsplitpath_s</tt>, but it returns an empty fname for <tt>c:\foo\</tt>, considering it a directory.
 * </p>
 *
 * @param buffer_size The number of characters in the buffer (out parameter).
 */
rt_s rt_file_path_get_name(const rt_char *path, rt_un path_size, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);


/**
 *
 * <p>
 * Under Windows, GetTempPath, so TMP/TEMP/USERPROFILE environment variable or Windows directory.<br>
 * Under Linux, TMPDIR environment variable, P_tmpdir macro or /tmp.
 * </p>
 *
 * @param buffer_size The number of characters in the buffer (out parameter).
 */
rt_s rt_file_path_get_temp_dir(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 *
 * <p>
 * Samples:
 * <tt>/home/username</tt><br>
 * <tt>C:\Users\UserName</tt>br>
 * </p>
 *
 * @param buffer_capacity Under Windows, must be at least MAX_PATH (260).
 * @param buffer_size The number of characters in the buffer (out parameter).
 */
rt_s rt_file_path_get_home_dir(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

/**
 *
 * <p>
 * The application name is used as is under Windows.<br>
 * It is converted to lower case under Linux.
 * </p>
 *
 * <p>
 * Samples:
 * <tt>/home/username/.codeblocks</tt><br>
 * <tt>C:\Users\UserName\AppData\Roaming\CodeBlocks</tt><br>
 * </p>
 *
 * <p>
 * The returned directory should not exist if you have not created it already.
 * </p>
 *
 * @param application_name Should be provided in CamelCase.
 * @param buffer_capacity Under Windows, must be at least MAX_PATH (260).
 * @param buffer_size The number of characters in the buffer (out parameter).
 */
rt_s rt_file_path_get_application_data_dir(const rt_char *application_name, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

#endif /* RT_FILE_PATH_H */

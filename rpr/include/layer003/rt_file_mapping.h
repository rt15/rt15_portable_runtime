#ifndef RT_FILE_MAPPING_H
#define RT_FILE_MAPPING_H

#include <layer000/rt_types.h>

struct rt_file_mapping {
#ifdef RT_DEFINE_WINDOWS
	rt_h file_handle;
	rt_h file_mapping_handle;
	rt_un initial_file_size;
	rt_un max_size;
#else
	rt_n32 file_descriptor;
#endif
	rt_un file_size;
};

struct rt_file_mapping_view {
	struct rt_file_mapping *file_mapping;
	void *area;
	rt_un view_size;
};

enum rt_file_mapping_right {
	RT_FILE_MAPPING_RIGHT_READ = 1,
	RT_FILE_MAPPING_RIGHT_READ_WRITE,
	RT_FILE_MAPPING_RIGHT_EXECUTE_READ,
	RT_FILE_MAPPING_RIGHT_EXECUTE_READ_WRITE
};

/**
 * @param max_size Can be 0 to map the whole file.
 */
RT_API rt_s rt_file_mapping_create(struct rt_file_mapping *file_mapping, const rt_char *file_path, enum rt_file_mapping_right file_mapping_right, rt_un max_size);

RT_API rt_s rt_file_mapping_set_file_size(struct rt_file_mapping *file_mapping, rt_un size);

RT_API rt_s rt_file_mapping_free(struct rt_file_mapping *file_mapping);

/**
 * @param size Can be 0 to map from <tt>offset</tt> to the end of the file.
 */
RT_API rt_s rt_file_mapping_view_create(struct rt_file_mapping_view *file_mapping_view, struct rt_file_mapping *file_mapping, enum rt_file_mapping_right file_mapping_right, rt_un offset, rt_un size);

RT_API rt_s rt_file_mapping_view_flush(struct rt_file_mapping_view *file_mapping_view, void *area, rt_un size);

RT_API rt_s rt_file_mapping_view_free(struct rt_file_mapping_view *file_mapping_view);

#endif /* RT_FILE_MAPPING_H */

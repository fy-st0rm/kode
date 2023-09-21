#ifndef __ISO_COMMONS_H__
#define __ISO_COMMONS_H__

#include "iso_util/iso_includes.h"
#include "iso_util/iso_defines.h"
#include "iso_util/iso_log.h"
#include "iso_util/iso_str.h"

/*
 * @brief Enum of the graphics data type
 */

typedef enum {
	ISO_GRAPHICS_FLOAT,
	ISO_GRAPHICS_INT,
	ISO_GRAPHICS_UNSIGNED_INT,
	ISO_GRAPHICS_UNSIGNED_BYTE
} iso_graphics_data_type;


/*
 * @brief Enum for the texture format
 */

typedef enum {
	ISO_RGB,
	ISO_RGBA
} iso_graphics_texture_fmt;

/*
 * @brief Enum of the buffer usage types.
 */

typedef enum {
	ISO_GRAPHICS_STREAM,
	ISO_GRAPHICS_STATIC,
	ISO_GRAPHICS_DYNAMIC
} iso_graphics_buffer_usage_type;

/*
 * @brief Struct that holds the data to update the buffer
 * @mem start_sz = Starting size of the data
 * @mem end_sz   = Ending size of the data
 * @mem data     = Pointer to the data
 */

typedef struct {
	size_t start_sz, end_sz;
	void* data;
} iso_graphics_buffer_update_def;


/*
 * @brief Function to get size of iso graphics data types
 * @param type = iso graphics data type
 * @return Returns the size of data type
 */

static size_t iso_graphics_get_type_size(iso_graphics_data_type type) {
	size_t sz = 0;
	switch (type) {
		case ISO_GRAPHICS_FLOAT        : sz = sizeof(f32); break;
		case ISO_GRAPHICS_INT          : sz = sizeof(i32); break;
		case ISO_GRAPHICS_UNSIGNED_INT : sz = sizeof(u32); break;
		case ISO_GRAPHICS_UNSIGNED_BYTE: sz = sizeof(u8); break;
		default: iso_assert(0, "Unknown ISO_GRAPHICS_DATA_TYPE: %d\n", type); break;
	}
	return sz;
}

#endif //__ISO_COMMONS_H__

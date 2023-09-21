#ifndef __ISO_GRAPHICS_VERTEX_BUFFER_H__
#define __ISO_GRAPHICS_VERTEX_BUFFER_H__

#include "iso_graphics/iso_graphics_types/iso_commons.h"

/*
 * @brief Structure that holds definition for vertex buffer and index buffer
 * @mem data  = Pointer to the data
 * @mem size  = Size of the data
 * @mem usage = Buffer usage type
 */

typedef struct {
	void*  data;
	size_t size;
	iso_graphics_buffer_usage_type usage;
} iso_graphics_vertex_buffer_def;

/*
 * @brief Vertex Buffer object
 * @mem id   = Id of the buffer
 * @mem name = Name of the buffer
 */

typedef struct {
	u32 id;
} iso_graphics_vertex_buffer;

ISO_API iso_graphics_vertex_buffer* iso_graphics_vertex_buffer_new();
ISO_API void iso_graphics_vertex_buffer_delete(iso_graphics_vertex_buffer* vbo);

#endif // __ISO_GRAPHICS_VERTEX_BUFFER_H__

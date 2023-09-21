#ifndef __FONT_RENDERER_H__
#define __FONT_RENDERER_H__

#include <isolate.h>
#include <SDL2/SDL_ttf.h>

// Shaders
static const iso_str vert_shader = ""
"#version 440 core\n"
"layout (location=0) in vec3 pos;\n"
"layout (location=1) in vec4 color;\n"
"layout (location=2) in vec2 tex_coord;\n"
"uniform mat4 mvp;\n"
"out vec4 o_color;\n"
"out vec2 o_tex_coord;\n"
"void main() {\n"
"  o_color = color;\n"
"  o_tex_coord = tex_coord;\n"
"  gl_Position = mvp * vec4(pos, 1.0f);\n"
"}\n";

static const iso_str frag_shader = ""
"#version 440 core\n"
"layout (location=0) out vec4 color;\n"
"uniform sampler2D char_texture;\n"
"in vec4 o_color;\n"
"in vec2 o_tex_coord;\n"
"void main() {\n"
"  color = texture(char_texture, o_tex_coord) * o_color;\n"
"}\n";


#define VERTEX_STRIDE 9
typedef struct {
	iso_vec3 pos;
	iso_vec4 color;
	iso_vec2 tex_coord;
} vertex;

#define ASCII_LOWER    32
#define ASCII_HIGHER   127
#define KODE_GLYPH_CAP (ASCII_HIGHER - ASCII_LOWER) + 1
typedef struct {
	iso_texture* texture;
	iso_vec2 size;
} kode_glyph;

typedef struct {
	iso_str font_path;
	u32 font_size;
} kode_font_def;

typedef struct {
	TTF_Font* sdl_font;

	iso_str font_path;
	u32 font_size;

	iso_hmap(char*, kode_glyph, KODE_GLYPH_CAP) glyphs;
} kode_font;

typedef struct {
	iso_app* app;
	iso_vertex_buffer* vbo;
	iso_index_buffer* ibo;
	iso_shader* shader;
	iso_render_pipeline* pip;
	iso_camera* cam;

	u32 max_quad_cnt;
	u32 max_vertex_cnt;
	u32 max_index_cnt;

	size_t vbo_size;
	size_t ibo_size;

	vertex* buffer;

	kode_font* font;
} font_renderer;

font_renderer* font_renderer_new(iso_app* app, u32 max_quad_cnt, kode_font_def font_def);
void font_renderer_delete(font_renderer* ren);
void font_renderer_update(font_renderer* ren);
void font_renderer_render_text(
	font_renderer* ren,
	const iso_str text,
	iso_vec2 pos,
	f32 scale
);

#endif // __FONT_RENDERER_H__
#ifndef __KODE_FONT_H__
#define __KODE_FONT_H__

#include <isolate.h>
#include <SDL2/SDL_ttf.h>

#define ASCII_LOWER    32
#define ASCII_HIGHER   127
#define KODE_GLYPH_CAP (ASCII_HIGHER - ASCII_LOWER) + 1

typedef struct {
	iso_vec4 tex_coord;
	iso_vec2 size;
} kode_glyph;

typedef struct {
	iso_str font_path;
	u32 font_size;
} kode_font_def;

typedef struct {
	iso_str font_path;
	u32 font_size;

	TTF_Font* sdl_font;
	iso_texture* atlas;

	iso_hmap(char, kode_glyph, KODE_GLYPH_CAP) glyphs;
} kode_font;


kode_font* kode_font_new(kode_font_def def);
void kode_font_delete(kode_font* font);


#endif //__KODE_FONT_H__
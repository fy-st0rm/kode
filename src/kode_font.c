#include "kode_font.h"
#include "iso_graphics/iso_opengl_backend/iso_gl_texture.h"

iso_vec2 generate_atlas_size(kode_font* font) {
	iso_vec2 size = {0};

	for (char c = ASCII_LOWER; c < ASCII_HIGHER; c++) {
		char text[2] = " \0";
		text[0] = c;

		SDL_Color color = { 255, 255, 255, 255 };
		SDL_Surface* surface = iso_sdl_check_ptr(
			TTF_RenderText_Blended(font->sdl_font, text, color)
		);

		size.x += surface->w;

		if (size.y < surface->h)
			size.y = surface->h;

		SDL_FreeSurface(surface);
	}
	return size;
}

void kode_font_init_glyphs(kode_font* font) {
	iso_vec2 atlas_size = generate_atlas_size(font);

	// Generating atlas
	iso_texture_from_data_def atlas_def = {
		.name   = "atlas",
		.pixels = NULL,
		.fmt    = ISO_RGBA,
		.width  = atlas_size.x,
		.height = atlas_size.y,
		.filter = {
			.min  = ISO_TEXTURE_FILTER_NEAREST,
			.mag  = ISO_TEXTURE_FILTER_NEAREST
		}
	};
	font->atlas = iso_texture_new_from_data(atlas_def);
//	iso_texture_bind(font->atlas);

	u32 x_offset = 0;
	for (char c = ASCII_LOWER; c < ASCII_HIGHER; c++) {
		char text[2] = " \0";
		text[0] = c;

		SDL_Color color = { 255, 255, 255, 255 };
		SDL_Surface* surface = iso_sdl_check_ptr(
			TTF_RenderText_Blended(font->sdl_font, text, color)
		);
		iso_gl_flip_surface(surface);
		iso_gl_pack_surface(surface);

		// Updating atlas
		iso_texture_update_def up_def = {
			.pixels = surface->pixels,
			.lod = 0,
			.x_offset = x_offset,
			.y_offset = 0,
			.width = surface->w,
			.height = surface->h,
			.format = GL_RGBA,
			.type = ISO_UNSIGNED_BYTE
		};

		iso_texture_update(font->atlas, up_def);

		// Creating texture coordinates
		kode_glyph glyph = {
			.tex_coord = (iso_vec4) {
				.x = x_offset / atlas_size.x,
				.y = 0,
				.z = surface->w / atlas_size.x,
				.w = 1
			},
			.size = (iso_vec2) {
				.x = surface->w,
				.y = surface->h,
			},
		};

		iso_hmap_add(font->glyphs, c, glyph);

		x_offset += surface->w;
		SDL_FreeSurface(surface);
	}
}

kode_font* kode_font_new(kode_font_def def) {
	kode_font* font = iso_alloc(sizeof(kode_font));

	font->font_path = iso_str_new(def.font_path);
	font->font_size = def.font_size;

	font->sdl_font = iso_sdl_check_ptr(TTF_OpenFont(
		font->font_path,
		font->font_size
	));

	kode_font_init_glyphs(font);

	iso_log_sucess("Sucessfully loaded font: `%s: %d`\n",
		font->font_path, font->font_size);

	return font;
}

void kode_font_delete(kode_font* font) {
	for (char c = ASCII_LOWER; c < ASCII_HIGHER; c++) {
		kode_glyph glyph;

		char name[2] = " \0";
		name[0] = c;

		iso_hmap_get(font->glyphs, c, glyph);
	}

	iso_texture_delete(font->atlas);
	iso_str_delete(font->font_path);
	iso_hmap_delete(font->glyphs);
	TTF_CloseFont(font->sdl_font);
	TTF_Quit();

	iso_free(font);
}

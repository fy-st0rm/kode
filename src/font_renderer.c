#include "font_renderer.h"
#include "iso_graphics/iso_opengl_backend/iso_gl_texture.h"

void init_isolate_buffers(font_renderer* ren) {
	iso_sdl_check(TTF_Init());

	iso_vertex_buffer_def vbo_def = {
		.name = "vbo",
		.size = ren->vbo_size,
		.data = NULL,
		.usage = ISO_BUFFER_DYNAMIC
	};
	ren->vbo = iso_vertex_buffer_new(vbo_def);

	u32 indices[ren->max_index_cnt];
	for (int offset = 0, i = 0; i < ren->max_index_cnt; i += 6) {
		indices[0 + i] = 0 + offset;
		indices[1 + i] = 1 + offset;
		indices[2 + i] = 2 + offset;
		indices[3 + i] = 2 + offset;
		indices[4 + i] = 3 + offset;
		indices[5 + i] = 0 + offset;
		offset += 4;
	}

	iso_index_buffer_def ibo_def = {
		.name = "ibo",
		.size = ren->ibo_size,
		.data = indices,
		.usage = ISO_BUFFER_STATIC
	};
	ren->ibo = iso_index_buffer_new(ibo_def);

	iso_shader_def shader_def = {
		.name = "shader",
		.v_src = vert_shader,
		.f_src = frag_shader,
		.load_type = ISO_SHADER_FROM_STR
	};
	ren->shader = iso_shader_new(shader_def);

	iso_render_pipeline_def pip_def = {
		.name = "pip",
		.draw_type = ISO_TRIANGLES,
		.render_type = ISO_RENDER_USING_IBO,

		.buffers = {
			.vbo = ren->vbo,
			.ibo = ren->ibo,
			.shader = ren->shader
		},

		.layout_cnt = 3,
		.layout  = (iso_vertex_layout_def[]) {
			{ .amt = 3, .type = ISO_FLOAT },
			{ .amt = 4, .type = ISO_FLOAT },
			{ .amt = 2, .type = ISO_FLOAT },
		}	
	};
	ren->pip = iso_render_pipeline_new(pip_def);

	iso_ortho_camera_def cam_def = {
		.name = "cam",
		.pos  = (iso_vec3) { 0, 0, 0 },
		.rot  = (iso_rotation) {
			.angle = 0.0f,
			.axes = (iso_vec3) { 0, 0, 1 }
		},
		.viewport = (iso_camera_ortho_viewport_def) {
			.left = 0.0f,
			.right = 800.0f,
			.top = 0.0f,
			.bottom = 600.0f,
			.near = -1.0f,
			.far = 100.0f
		}
	};
	ren->cam = iso_ortho_camera_new(ren->app->camera_manager, cam_def);
}

void generate_glyphs(font_renderer* ren) {
	for (char c = ASCII_LOWER; c < ASCII_HIGHER; c++) {
		char text[2] = " \0";
		text[0] = c;

		SDL_Color color = { 255, 255, 255, 255 };
		SDL_Surface* surface = iso_sdl_check_ptr(TTF_RenderText_Blended(
			ren->font->sdl_font, text, color));
		iso_gl_flip_surface(surface);
		iso_gl_pack_surface(surface);

		iso_texture_from_data_def td = {
			.name   = text,
			.pixels = surface->pixels,
			.fmt    = ISO_RGBA,
			.width  = surface->w,
			.height = surface->h,
			.filter = {
				.min  = ISO_TEXTURE_FILTER_LINEAR,
				.mag  = ISO_TEXTURE_FILTER_NEAREST
			}
		};
		iso_texture* texture = iso_texture_new_from_data(td);

		kode_glyph glyph = {
			.texture = texture,
			.size = (iso_vec2) {
				.x = surface->w,
				.y = surface->h,
			},
		};
		iso_hmap_add(ren->font->glyphs, glyph.texture->name, glyph);

		SDL_FreeSurface(surface);
	}

	/*
	iso_shader_bind(ren->shader);
	u32 samplers[32];
	for (u32 i = 0; i < 32; i++) samplers[i] = i;

	iso_uniform_def tex_uni_def = {
		.name = "char_texture",
		.type = ISO_UNIFORM_SAMPLER2D,
		.data = &(iso_uniform_sampler_def) {
			.count = 32,
			.samplers = samplers
		}
	};
	iso_shader_uniform_set(ren->shader, tex_uni_def); 
	*/
}

void delete_glyphs(font_renderer* ren) {
	for (char c = ASCII_LOWER; c < ASCII_HIGHER; c++) {
		kode_glyph glyph;

		char name[2] = " \0";
		name[0] = c;

		iso_hmap_get(ren->font->glyphs, name, glyph);
		iso_texture_delete(glyph.texture);
	}
}

void init_font(font_renderer* ren, kode_font_def font_def) {
	ren->font = iso_alloc(sizeof(kode_font));

	ren->font->font_path = iso_str_new(font_def.font_path);
	ren->font->font_size = font_def.font_size;

	ren->font->sdl_font = iso_sdl_check_ptr(TTF_OpenFont(
		ren->font->font_path,
		ren->font->font_size)
	);

	iso_log_sucess("Sucessfully loaded font: `%s: %d`\n",
		ren->font->font_path, ren->font->font_size);

	generate_glyphs(ren);

	//TODO: Create texture atlas
	//TODO: Setting up the samplers
}

font_renderer* font_renderer_new(iso_app* app, u32 max_quad_cnt, kode_font_def font_def) {
	font_renderer* ren = iso_alloc(sizeof(font_renderer));

	ren->app = app;

	ren->max_quad_cnt   = max_quad_cnt;
	ren->max_vertex_cnt = max_quad_cnt * 4;
	ren->max_index_cnt  = max_quad_cnt * 6;
	ren->vbo_size       = max_quad_cnt * 4 * sizeof(vertex);
	ren->ibo_size       = max_quad_cnt * 6 * sizeof(u32);

	ren->buffer = iso_alloc(ren->vbo_size);
	init_isolate_buffers(ren);
	init_font(ren, font_def);
	return ren;
}

void font_renderer_delete(font_renderer* ren) {
	iso_vertex_buffer_delete(ren->vbo);
	iso_index_buffer_delete(ren->ibo);
	iso_shader_delete(ren->shader);
	iso_render_pipeline_delete(ren->pip);
	iso_ortho_camera_delete(ren->app->camera_manager, "cam");

	TTF_CloseFont(ren->font->sdl_font);
	iso_str_delete(ren->font->font_path);
	delete_glyphs(ren);
	iso_hmap_delete(ren->font->glyphs);
	iso_free(ren->font);

	iso_free(ren->buffer);
	iso_free(ren);

	TTF_Quit();
}

void font_renderer_update(font_renderer* ren) {
	iso_render_pipeline_begin(ren->pip);

	f32 vertices[] = {
		 50.0f, 50.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		650.0f, 50.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		650.0f,250.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		 50.0f,250.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	};

	iso_buffer_update_def up_def = {
		.start_sz = 0,
		.end_sz = sizeof(vertices),
		.data = vertices
	};
	iso_vertex_buffer_update(ren->vbo, up_def);

	char* name = "A";

	kode_glyph glyph;
	iso_hmap_get(ren->font->glyphs, name, glyph);

	iso_texture_bind(glyph.texture);

	int samplers[] = { glyph.texture->id };
	iso_uniform_def tex_uni_def = {
		.name = "char_texture",
		.type = ISO_UNIFORM_SAMPLER2D,
		.data = &(iso_uniform_sampler_def) {
			.count = 1,
			.samplers = samplers
		}
	};
	iso_shader_uniform_set(ren->shader, tex_uni_def); 

	iso_ortho_camera_update(ren->app->camera_manager, "cam");
	iso_uniform_def uni_def = {
		.name = "mvp",
		.type = ISO_UNIFORM_MAT4,
		.data = &ren->cam->mvp
	};
	iso_shader_uniform_set(ren->shader, uni_def);

	iso_render_pipeline_end(ren->pip, 6);
}

void font_renderer_render_text(
	font_renderer* ren,
	const iso_str text,
	iso_vec2 pos,
	f32 scale
) {
	for (u32 i = 0; i < iso_str_len(text); i++) {
		iso_render_pipeline_begin(ren->pip);
		//TOOD: Camera static, Probably ok to call this startup
		iso_ortho_camera_update(ren->app->camera_manager, "cam");
		iso_uniform_def uni_def = {
			.name = "mvp",
			.type = ISO_UNIFORM_MAT4,
			.data = &ren->cam->mvp
		};
		iso_shader_uniform_set(ren->shader, uni_def);

		char c = iso_str_char_at(text, i);
		char name[2] = " \0";
		name[0] = c;

		kode_glyph glyph;
		iso_hmap_get(ren->font->glyphs, name, glyph);

		iso_texture_bind(glyph.texture);

		int samplers[] = { glyph.texture->id };
	
		iso_uniform_def tex_uni_def = {
			.name = "char_texture",
			.type = ISO_UNIFORM_SAMPLER2D,
			.data = &(iso_uniform_sampler_def) {
				.count = 1,
				.samplers = samplers
			}
		};
		iso_shader_uniform_set(ren->shader, tex_uni_def); 

		f32 xpos = pos.x;
		f32 ypos = pos.y;
		f32 w = glyph.size.x * scale;
		f32 h = glyph.size.y * scale;

		f32 vertices[] = {
			xpos    , ypos    , 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
			xpos + w, ypos    , 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			xpos + w, ypos + h, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			xpos    , ypos + h, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		};

		iso_buffer_update_def up_def = {
			.start_sz = 0,
			.end_sz = sizeof(vertices),
			.data = vertices
		};
		iso_vertex_buffer_update(ren->vbo, up_def);

		iso_render_pipeline_end(ren->pip, 6);

		pos.x += w;
	}	
}

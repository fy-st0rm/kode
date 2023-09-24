#include "font_renderer.h"

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
			.axes = (iso_vec3) { 0, 0, 0 }
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

font_renderer* font_renderer_new(iso_app* app, u32 max_quad_cnt, kode_font_def font_def) {
	font_renderer* ren = iso_alloc(sizeof(font_renderer));

	ren->app = app;

	ren->max_quad_cnt   = max_quad_cnt;
	ren->max_vertex_cnt = max_quad_cnt * 4;
	ren->max_index_cnt  = max_quad_cnt * 6;
	ren->vbo_size       = max_quad_cnt * 4 * sizeof(vertex);
	ren->ibo_size       = max_quad_cnt * 6 * sizeof(u32);

	ren->buffer = iso_alloc(ren->vbo_size);
	ren->vertex_cnt = 0;

	init_isolate_buffers(ren);

	// Initializing font
	ren->font = kode_font_new(font_def);

	iso_shader_bind(ren->shader);
	int samplers[] = { ren->font->atlas->id };
	iso_uniform_def tex_uni_def = {
		.name = "tex_atlas",
		.type = ISO_UNIFORM_SAMPLER2D,
		.data = &(iso_uniform_sampler_def) {
			.count = 1,
			.samplers = samplers
		}
	};
	iso_shader_uniform_set(ren->shader, tex_uni_def); 
	return ren;
}

void font_renderer_delete(font_renderer* ren) {
	iso_vertex_buffer_delete(ren->vbo);
	iso_index_buffer_delete(ren->ibo);
	iso_shader_delete(ren->shader);
	iso_render_pipeline_delete(ren->pip);
	iso_ortho_camera_delete(ren->app->camera_manager, "cam");

	kode_font_delete(ren->font);
	iso_free(ren->buffer);
	iso_free(ren);
}

void font_renderer_push_text(
	font_renderer* ren,
	const iso_str text,
	iso_vec2 pos,
	f32 scale
) {
	for (u32 i = 0; i < iso_str_len(text); i++) {
		char c = iso_str_char_at(text, i);
		kode_glyph glyph;
		iso_hmap_get(ren->font->glyphs, c, glyph);

		f32 xpos = pos.x;
		f32 ypos = pos.y;
		f32 w = glyph.size.x;
		f32 h = glyph.size.y;

		f32 tx = glyph.tex_coord.x;
		f32 ty = glyph.tex_coord.y;
		f32 tw = glyph.tex_coord.z;
		f32 th = glyph.tex_coord.w;

		if (ren->vertex_cnt >= ren->max_vertex_cnt) {
			font_renderer_update(ren);
		}

		ren->buffer[ren->vertex_cnt++] = (vertex) { 
			xpos    , ypos    , 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, tx     , ty + th,
		};
		ren->buffer[ren->vertex_cnt++] = (vertex) { 
			xpos + w, ypos    , 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, tx + tw, ty + th,
		};
		ren->buffer[ren->vertex_cnt++] = (vertex) { 
			xpos + w, ypos + h, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, tx + tw, ty,
		};
		ren->buffer[ren->vertex_cnt++] = (vertex) { 
			xpos    , ypos + h, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, tx     , ty,
		};
		pos.x += w;
	}	
}

void font_renderer_update(font_renderer* ren) {
	u32 quads = ren->vertex_cnt / 4;
	u32 indices = quads * 6;

	iso_render_pipeline_begin(ren->pip);

	//TOOD: Camera static, Probably ok to call this startup
	iso_ortho_camera_update(ren->app->camera_manager, "cam");
	iso_uniform_def uni_def = {
		.name = "mvp",
		.type = ISO_UNIFORM_MAT4,
		.data = &ren->cam->mvp
	};
	iso_shader_uniform_set(ren->shader, uni_def);
	iso_texture_bind(ren->font->atlas);

	iso_buffer_update_def up_def = {
		.start_sz = 0,
		.end_sz = ren->vertex_cnt * sizeof(vertex),
		.data = ren->buffer 
	};
	iso_vertex_buffer_update(ren->vbo, up_def);

	iso_render_pipeline_end(ren->pip, indices);

	memset(ren->buffer, 0, ren->vbo_size);
	ren->vertex_cnt = 0;
}

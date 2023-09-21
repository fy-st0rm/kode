#include "kode.h"

void kode_new(iso_scene* scene) {
	kode* kd = scene->scene_data;

	kode_font_def fd = {
		.font_path = "../../fonts/Terminess.ttf",
		.font_size = 20
	};
	kd->ren = font_renderer_new(kd->app, 10, fd);
}	

void kode_delete(iso_scene* scene) {
	kode* kd = scene->scene_data;
	font_renderer_delete(kd->ren);
	iso_free(kd);
}

void kode_on_entry(iso_scene* scene) {
}

void kode_on_exit(iso_scene* scene) {
}

void kode_on_update(iso_scene* scene, f32 dt) {
	kode* kd = scene->scene_data;
	iso_app* app = kd->app;
	
	iso_window_clear(app->window, (iso_vec4) { 0, 0.3, 0.3, 1 });
	//font_renderer_update(kd->ren);
	iso_str text = iso_str_new("Hello world!!");
	font_renderer_render_text(
		kd->ren,
		text,
		(iso_vec2) { 100.0f, 100.0f },
		2
	);
	iso_str_delete(text);
}

void kode_on_event(iso_scene* scene, SDL_Event event, f32 dt) {
}

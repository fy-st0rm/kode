#include "kode.h"

void kode_new(iso_scene* scene) {
	kode* kd = scene->scene_data;

	kode_font_def fd = {
		.font_path = "../../fonts/Caskaydia.otf",
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
	iso_str text = iso_str_new("My Name is Slok Pradhan.");
	font_renderer_push_text(
		kd->ren,
		text,
		(iso_vec2) { 0.0f, 0.0f },
		-10
	);
	iso_str_delete(text);
	font_renderer_update(kd->ren);
}

void kode_on_event(iso_scene* scene, SDL_Event event, f32 dt) {
}

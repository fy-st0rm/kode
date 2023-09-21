#include <isolate.h>
#include "kode.h"

iso_app_def iso_init() {
	iso_window_def win_def = {
		.title = "Kode",
		.width = 800,
		.height = 600
	};

	iso_graphics_def grap_def = {
		.api = ISO_OPENGL_API
	};

	return (iso_app_def) {
		.window_def = win_def,
		.graphics_def = grap_def,
		.fps = 60.0f
	};
}

void iso_start(iso_app* app) {
	kode* kd = iso_alloc(sizeof(kode));
	kd->app = app;

	iso_scene_def scene = {
		.name = "kode",
		.scene_data = kd,

		.new = kode_new,
		.delete = kode_delete,
		.on_entry = kode_on_entry,
		.on_exit = kode_on_exit,
		.on_update = kode_on_update,
		.on_event = kode_on_event	
	};

	iso_scene_new(app->scene_manager, scene);
}

void iso_exit(iso_app* app) {
	iso_scene_delete(app->scene_manager, "kode");
}

i32 main(i32 argc, char** argv) {
	iso_entry_def entry = {
		.iso_init  = iso_init,
		.iso_start = iso_start,
		.iso_exit  = iso_exit
	};
	return iso_run(argc, argv, entry);
}
#ifndef __KOD_H__
#define __KOD_H__

#include <isolate.h>
#include "font_renderer.h"

typedef struct {
	iso_app* app;
	font_renderer* ren;
} kode;

void kode_new(iso_scene* scene);
void kode_delete(iso_scene* scene);
void kode_on_entry(iso_scene* scene);
void kode_on_exit(iso_scene* scene);
void kode_on_update(iso_scene* scene, f32 dt);
void kode_on_event(iso_scene* scene, SDL_Event event, f32 dt); 

#endif //__KOD_H__

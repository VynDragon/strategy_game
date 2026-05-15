#include <zephyr/kernel.h>
#include <zephyr/random/random.h>

#include <stdlib.h>

#include <lvgl.h>

#include "engine.h"
#include "generated_objects.h"
#include "utility.h"
#include "filters.h"

#include "tv_scene/map.h"

#include "ui_style.h"

#include "misc_light_round.h"

#include "models/cube.h"

typedef struct TV_Scene_Data_s {
	Engine_Object			*player;
	lv_obj_t				*ui;
} TV_Scene_Data;

#define ANIM_0(i, _) {.type = 2, .transform = {.translation = {.x = 0, .y = 0, .z = 0}, .rotation = {0}, .scale = {0}}}

// static const ObjectProcess_FrameArray_Frame camera_animation_impl_frames[] = {
// 	LISTIFY(100, ANIM_0, (,)),
// };

static Engine_Object *tv_light_objects[1];

static void tv_light_animation_pf(Engine_Object *object, void* data, uint64_t frame_id)
{
	object->visual.transform.scale.x = L3_F * ((sys_rand8_get() >> 6) + 2);
}

static Animation tv_light_animation = ANIMATION_INIT(
	tv_light_objects,
	((void*[]){(void*)NULL}),
	((AnimationObjectProcess[]) {tv_light_animation_pf}),
	1, 4,
	2,
	true
);

// #define CAM_ANIM_0(i, _) {.type = 2, .transform = {.translation = {.x = 0, .y = 0, .z = 0}, .rotation = {0}, .scale = {0}}}
//
// static const ObjectProcess_FrameArray_Frame camera_animation_impl_frames[] = {
// 	LISTIFY(100, CAM_ANIM_0, (,)),
// };
//
// static const ObjectProcess_FrameArray camera_animation_impl = {
// 	.len = ARRAY_SIZE(camera_animation_impl_frames),
// 	.loop = false,
// 	.frames = camera_animation_impl_frames,
// };
//
// static void camera_animation_focus_pf(Engine_Object *object, void* data, uint64_t frame_id)
// {
// 	TV_Scene_Data *scene_data = (TV_Scene_Data*)data;
//
// 	L3_Camera *camera = engine_getcamera();
// 	//if (frame_id % 2 == 0) {
// 		engine_pop_filter();
// 	//}
// 	if (frame_id == 0) {
// 		camera->focalLength = 130;
// 		engine_append_filter(filter_pixelize_2);
// 		engine_append_filter(filter_pixelize_4);
// 		engine_append_filter(filter_pixelize_6);
// 		engine_append_filter(filter_pixelize_8);
// 	}
// 	if (frame_id == 4) {
// 		camera->focalLength = 150;
// 		engine_append_filter(filter_pixelize_2);
// 		engine_append_filter(filter_pixelize_4);
// 		engine_append_filter(filter_pixelize_6);
// 		engine_append_filter(filter_pixelize_8);
// 		lv_obj_set_size(scene_data->rect, CONFIG_RESOLUTION_X / 3.5, CONFIG_RESOLUTION_Y / 3.5);
// 			lv_obj_center(scene_data->rect);
// 	}
// 	if (frame_id == 8) {
// 		camera->focalLength = 180;
// 		engine_append_filter(filter_pixelize_2);
// 		engine_append_filter(filter_pixelize_4);
// 		engine_append_filter(filter_pixelize_6);
// 		engine_append_filter(filter_pixelize_8);
// 		lv_obj_set_size(scene_data->rect, CONFIG_RESOLUTION_X / 3, CONFIG_RESOLUTION_Y / 3);
// 			lv_obj_center(scene_data->rect);
// 	}
// 	if (frame_id == 16) {
// 		camera->focalLength = 400;
// 		L3_transform3DSet(22.5 * L3_F,-7.5*L3_F,-75*L3_F,0,0,0,L3_F,L3_F,L3_F,&(object->visual.transform));
// 		engine_append_filter(filter_pixelize_2);
// 		engine_append_filter(filter_pixelize_4);
// 		engine_append_filter(filter_pixelize_4);
// 		engine_append_filter(filter_pixelize_6);
// 		engine_append_filter(filter_pixelize_6);
// 		engine_append_filter(filter_pixelize_8);
// 		engine_append_filter(filter_pixelize_8);
// 		lv_obj_set_size(scene_data->rect, CONFIG_RESOLUTION_X / 1.2, CONFIG_RESOLUTION_Y / 1.2);
// 		lv_obj_center(scene_data->rect);
// 	}
// 	lv_obj_align_to(scene_data->text, scene_data->rect, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
// 	lv_label_set_text_fmt(scene_data->text2, "%x%x", sys_rand32_get(), sys_rand32_get());
// 	lv_obj_align_to(scene_data->text2, scene_data->rect, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
// }
//
// static Engine_Object *camera_animation_objects[2];
// static void *camera_animation_data[2] = {
// 	(void*)&camera_animation_impl,
// 	NULL,
// };
//
// static Animation camera_animation = ANIMATION_INIT(
// 	camera_animation_objects,
// 	camera_animation_data,
// 	((AnimationObjectProcess[]) {utility_animation_objectprocess_framearray, camera_animation_focus_pf}),
// 	2, 8,
// 	ARRAY_SIZE(camera_animation_impl_frames),
// 	false
// );

static TV_Scene_Data tv_scene_data = {0};

static void tv_scene_init(Engine_Scene *scene)
{
	TV_Scene_Data *scene_data = (TV_Scene_Data*)scene->data;

	L3_normallight_vector((L3_Vec4){0,3*L3_F/4,1*L3_F/4,L3_F});

	scene_data->ui = lv_obj_create(lv_screen_active());
	lv_obj_set_pos(scene_data->ui, 0, 0);
	lv_obj_set_size(scene_data->ui, CONFIG_RESOLUTION_X, CONFIG_RESOLUTION_Y);
	lv_obj_add_style(scene_data->ui, &ui_style, LV_PART_MAIN);

	// Engine_Object *light = engine_add_object(misc_light_round);
	// light->visual.config.visible = L3_VISIBLE_BILLBOARD |  L3_VISIBLE_BILLBOARD_ADDITIVE | L3_VISIBLE_BILLBOARD_ZSLOW | L3_VISIBLE_BILLBOARD_FRONTIFY;
	// L3_transform3DSet(1 * L3_F, 1.2*L3_F, 0*L3_F,0,0,0,2*L3_F,2*L3_F,2*L3_F,&(light->visual.transform));

	Engine_Object tmp = {0};
	// L3_transform3DSet(2.5 * L3_F, 1.5*L3_F, 2.5*L3_F,0,0,0,16*L3_F,1*L3_F,1*L3_F,&(tmp.visual.transform));
	// tmp.view_range = 32 * L3_F;
	// tmp.visual_type = ENGINE_VISUAL_LIGHT;
	// tmp.visual.solid_color = 0xFF;
	// engine_add_object(tmp);

	L3_transform3DSet(1.8 * L3_F, 1.3*L3_F, 0*L3_F,0,0,0,4*L3_F,16*L3_F,0.5*L3_F,&(tmp.visual.transform));
	tmp.view_range = 32 * L3_F;
	tmp.visual_type = ENGINE_VISUAL_LIGHT;
	tmp.visual.solid_color = 0x80;
	tv_light_objects[0] = engine_add_object(tmp);
/*
	L3_transform3DSet(-1 * L3_F, 2*L3_F, -1.54*L3_F,0,0,0,4*L3_F,16*L3_F,0.5*L3_F,&(tmp.visual.transform));
	tmp.view_range = 32 * L3_F;
	tmp.visual_type = ENGINE_VISUAL_LIGHT;
	tmp.visual.solid_color = 0x80;
	engine_add_object(tmp);*/

	// Engine_Object tmp = {0};
	// L3_transform3DSet(30 * L3_F,-10*L3_F,-100*L3_F,0,0,0,L3_F,L3_F,L3_F,&(tmp.visual.transform));
	// tmp.view_range = 1 * L3_F;
	// tmp.visual_type = ENGINE_VISUAL_NOTHING;
	// camera_animation_objects[0] = engine_add_object(tmp);
	// camera_animation_objects[1] = camera_animation_objects[0];
}

L3_Unit eh = 32;

static void tv_scene_pf(Engine_Scene *scene)
{
	TV_Scene_Data *scene_data = (TV_Scene_Data*)scene->data;

	utility_animation_process(&tv_light_animation);


	// L3_Camera *camera = engine_getcamera();
	// camera->transform = camera_animation_objects[0]->visual.transform;
}

static void tv_scene_deinit(Engine_Scene *scene)
{
	TV_Scene_Data *scene_data = (TV_Scene_Data*)scene->data;

	lv_obj_clean(scene_data->ui);
	lv_obj_del_async(scene_data->ui);
}

static Filter_f tv_scene_filters[] = {
	//filter_bloom,
};

Engine_Scene tv_scene = {
	.pf = tv_scene_pf,
	.statics = NULL,
	.statics_count = 0,
	.inf = tv_scene_init,
	.dif = tv_scene_deinit,
	.data = &tv_scene_data,
	.filters = tv_scene_filters,
	.filters_count = ARRAY_SIZE(tv_scene_filters),
	.statics = map,
	.statics_count = sizeof(map) / sizeof(*map),
};

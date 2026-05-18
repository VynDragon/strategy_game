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
	Engine_Object			*tv;
	L3_Model3D				tv_model;
	const L3_Texture 		*tv_model_textures[2];
	size_t					tv_frame_index;
	Engine_Object			*tv_light;
	lv_obj_t				*text;
	size_t					text_index;
} TV_Scene_Data;
/*
static Engine_Object *tv_light_objects[1];

static void tv_light_animation_pf(Engine_Object *object, void* data, uint64_t frame_id)
{
	object->visual.transform.scale.x = L3_F * ((sys_rand8_get() >> 6) + 2);
}

static Animation tv_light_animation = ANIMATION_INIT(
	tv_light_objects,
	((void*[]){(void*)NULL}),
	((AnimationObjectProcess[]) {tv_light_animation_pf}),
	1, 2,
	2,
	true
);*/

#define CAM_ANIM_0(i, _) {.type = FRAMEARRAY_FRAME_RELATIVE, .transform = {.translation = {.x = 0, .y = 0, .z = 0}, .rotation = {0}, .scale = {0}}}
#define CAM_ANIM_1(i, _) {.type = FRAMEARRAY_FRAME_RELATIVE, .transform = {.translation = {.x = 0, .y = -1, .z = 0}, .rotation = {0}, .scale = {0}}}
#define CAM_ANIM_2(i, _) {.type = FRAMEARRAY_FRAME_RELATIVE, .transform = {.translation = {.x = 0, .y = 0, .z = 2}, .rotation = {0}, .scale = {0}}}
#define CAM_ANIM_3(i, _) {.type = FRAMEARRAY_FRAME_RELATIVE, .transform = {.translation = {.x = -2, .y = 0, .z = 0}, .rotation = {0}, .scale = {0}}}

static const ObjectProcess_FrameArray_Frame camera_animation_impl_frames[] = {
	LISTIFY(720, CAM_ANIM_1, (,)),
	{.type = FRAMEARRAY_FRAME_ABSOLUTE, .transform = {.translation = {.x = 1.0*L3_F, .y = 0.56 * L3_F, .z = -1.2 * L3_F}, .rotation = {.x = 0, .y = -0.25*L3_F, .z = 0}, .scale = {.x = L3_F, .y = L3_F, .z = L3_F}}},
	LISTIFY(369, CAM_ANIM_2, (,)),
	{.type = FRAMEARRAY_FRAME_ABSOLUTE, .transform = {.translation = {.x = 1.0*L3_F, .y = 1.2 * L3_F, .z = 0 * L3_F}, .rotation = {.x = 0, .y = -0.25*L3_F, .z = 0}, .scale = {.x = L3_F, .y = L3_F, .z = L3_F}}},
	LISTIFY(800, CAM_ANIM_3, (,)),
};

static const ObjectProcess_FrameArray camera_animation_impl = {
	.len = ARRAY_SIZE(camera_animation_impl_frames),
	.loop = false,
	.frames = camera_animation_impl_frames,
};

static Engine_Object *camera_animation_objects[3];
static void *camera_animation_data[3] = {
	(void*)&camera_animation_impl,
	NULL,
	NULL,
};

static void *text_animation_data[3] = {
	NULL,
	NULL,
	NULL,
};

static const char *text_frames[] =
{
	"                                ",
	"Yesterday, unidentified radio",
	"emissions",
	"were tracked back to an object",
	"dubbed '2056 EG'",
	"The object, several hundred",
	"meters in scale, was initially",
	"thought to be a passing asteroid",
	"but recent observations noticed",
	"unusual movement patterns,",
	"which were soon followed by",
	"reduction of its velocity and",
	"broadband radiation",
	"of light and radiofrequencies",
	"Speculation on social medias",
	"and within the scientific",
	"community already attribute",
	"the object's behavior",
	"to intelligent life", /* 180 frames */
	"we will soon be joined",
	"by experts to discuss",
	"the matter",
	"But first, ",
	"news from the front",
	"with our correspondants"
};



static void text_animation_pf(Engine_Object *object, void* data, uint64_t frame_id)
{
	TV_Scene_Data *scene_data = (TV_Scene_Data*)data;

	if (frame_id == 0) {
		scene_data->text_index = 0;
	} else if (frame_id % 10 == 0) {
		scene_data->text_index++;
		if (scene_data->text_index < ARRAY_SIZE(text_frames))
		{
			lv_label_set_text(scene_data->text, text_frames[scene_data->text_index]);
			lv_obj_align(scene_data->text, LV_ALIGN_BOTTOM_MID, 0, -2);
		} else {
			lv_label_set_text(scene_data->text, text_frames[0]);
			lv_obj_align(scene_data->text, LV_ALIGN_BOTTOM_MID, 0, -2);
		}
	}
	// lv_obj_align_to(scene_data->text, scene_data->rect, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
	// lv_label_set_text_fmt(scene_data->text2, "%x%x", sys_rand32_get(), sys_rand32_get());
	// lv_obj_align_to(scene_data->text2, scene_data->rect, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
}

static const L3_Texture *texture_frames[11] =
{
	&texture_animations_teaser_tv_frame0013,
	&texture_animations_teaser_tv_frame0014,
	&texture_animations_teaser_tv_frame0015,
	&texture_animations_teaser_tv_frame0016,
	&texture_animations_teaser_tv_frame0017,
	&texture_animations_teaser_tv_frame0018,
	&texture_animations_teaser_tv_frame0019,
	&texture_animations_teaser_tv_frame0020,
	&texture_animations_teaser_tv_frame0021,
	&texture_animations_teaser_tv_frame0022,
	&texture_animations_teaser_tv_frame0023
};

static void tv_animation_pf(Engine_Object *object, void* data, uint64_t frame_id)
{
	TV_Scene_Data *scene_data = (TV_Scene_Data*)data;

	if (frame_id == 0) {
		scene_data->tv_frame_index = 8;
	}

	if (frame_id < 180) {
		scene_data->tv_frame_index += sys_rand8_get() % 3;
		scene_data->tv_frame_index = scene_data->tv_frame_index > 10 ? 8 : scene_data->tv_frame_index;
		scene_data->tv_model.triangleTextures[1] = texture_frames[scene_data->tv_frame_index];
		//scene_data->tv_light->visual.transform.scale.x = L3_F * (scene_data->tv_frame_index - 8);
		if (scene_data->tv_frame_index == 9) {
			scene_data->tv_light->visual.transform.scale.x = L3_F * 4;
		} else if (scene_data->tv_frame_index == 10) {
			scene_data->tv_light->visual.transform.scale.x = L3_F * 3;
		} else {
		scene_data->tv_light->visual.transform.scale.x = L3_F * 2;
		}
	} else if (frame_id < 190) {
		scene_data->tv_model.triangleTextures[1] = texture_frames[6];
	} else if (frame_id < 252) {
		if (frame_id % 2 == 0) {
			scene_data->tv_frame_index += sys_rand8_get() % 3;
			scene_data->tv_frame_index = scene_data->tv_frame_index > 5 ? 3 : scene_data->tv_frame_index;
			scene_data->tv_model.triangleTextures[1] = texture_frames[scene_data->tv_frame_index];
			if (scene_data->tv_frame_index == 3) {
				scene_data->tv_light->visual.transform.scale.x = L3_F * 4;
			} else if (scene_data->tv_frame_index == 4) {
				scene_data->tv_light->visual.transform.scale.x = L3_F * 3.5;
			} else {
				scene_data->tv_light->visual.transform.scale.x = L3_F * 3;
			}
		}
	} else {
		scene_data->tv_frame_index++;
		scene_data->tv_frame_index = scene_data->tv_frame_index > 2 ? 0 : scene_data->tv_frame_index;
		scene_data->tv_model.triangleTextures[1] = texture_frames[scene_data->tv_frame_index];
	}

	// if (frame_id < 41 && frame_id % 2 == 0) {
	// 	scene_data->tv_frame_index++;
	// 	scene_data->tv_frame_index = scene_data->tv_frame_index > 2 ? 0 : scene_data->tv_frame_index;
	// 	scene_data->tv_model.triangleTextures[1] = texture_frames[scene_data->tv_frame_index];
	// } else if (frame_id % 2 == 0) {
	// 	scene_data->tv_frame_index += sys_rand8_get() % 3;
	// 	scene_data->tv_frame_index = scene_data->tv_frame_index > 5 ? 3 : scene_data->tv_frame_index;
	// 	scene_data->tv_model.triangleTextures[1] = texture_frames[scene_data->tv_frame_index];
	// }
	// lv_obj_align_to(scene_data->text, scene_data->rect, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
	// lv_label_set_text_fmt(scene_data->text2, "%x%x", sys_rand32_get(), sys_rand32_get());
	// lv_obj_align_to(scene_data->text2, scene_data->rect, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
}

 static void focus_animation_pf(Engine_Object *object, void* data, uint64_t frame_id)
{
	TV_Scene_Data *scene_data = (TV_Scene_Data*)data;

	L3_Camera *camera = engine_getcamera();

	if (frame_id == 721) {
		camera->focalLength = 700;
	}
}

static Animation camera_animation_slow = ANIMATION_INIT(
	camera_animation_objects,
	text_animation_data,
	((AnimationObjectProcess[]) {text_animation_pf, tv_animation_pf, NULL}),
	2, 5,
	ARRAY_SIZE(camera_animation_impl_frames) / 6,
	false
);

static Animation camera_animation_fast = ANIMATION_INIT(
	camera_animation_objects,
	camera_animation_data,
	((AnimationObjectProcess[]) {utility_animation_objectprocess_framearray, focus_animation_pf}),
	2, 30,
	ARRAY_SIZE(camera_animation_impl_frames),
	false
);

static TV_Scene_Data tv_scene_data = {0};

static void tv_scene_init(Engine_Scene *scene)
{
	TV_Scene_Data *scene_data = (TV_Scene_Data*)scene->data;

	camera_animation_data[1] = (void*)scene_data;
	camera_animation_data[2] = (void*)scene_data;

	text_animation_data[0] = (void*)scene_data;
	text_animation_data[1] = (void*)scene_data;
	text_animation_data[2] = (void*)scene_data;

	L3_normallight_vector((L3_Vec4){0,3*L3_F/4,1*L3_F/4,L3_F});

	scene_data->ui = lv_obj_create(lv_screen_active());
	lv_obj_set_pos(scene_data->ui, 0, 0);
	lv_obj_set_size(scene_data->ui, CONFIG_RESOLUTION_X, CONFIG_RESOLUTION_Y);
	lv_obj_add_style(scene_data->ui, &ui_style, LV_PART_MAIN);

	scene_data->text = lv_label_create(scene_data->ui);
	lv_obj_add_style(scene_data->text, &ui_style, LV_PART_MAIN);
	lv_label_set_text_fmt(scene_data->text, "");
	lv_obj_align(scene_data->text, LV_ALIGN_BOTTOM_MID, 0, 0);

	memcpy(&scene_data->tv_model, &interior_tv, sizeof(L3_Model3D));
	scene_data->tv_model_textures[0] = 0;
	scene_data->tv_model_textures[1] = &texture_animations_teaser_tv_frame0013;
	scene_data->tv_model.triangleTextures = scene_data->tv_model_textures;
	Engine_Object tmp = {
		.visual.transform.scale.x = 512.0,
		.visual.transform.scale.y = 512.0,
		.visual.transform.scale.z = 512.0,
		.visual.transform.scale.w = 0,
		.visual.transform.translation.x = 1249.30908203125,
		.visual.transform.translation.y = 306.7450256347656,
		.visual.transform.translation.z = 0.0,
		.visual.transform.translation.w = 512,
		.visual.transform.rotation.x = 0.0,
		.visual.transform.rotation.y = -256.00002655194265,
		.visual.transform.rotation.z = -0.0,
		.visual.transform.rotation.w = 512,
		.visual.config.backfaceCulling = 2,
		.visual.solid_color = 0x80,
		.visual.config.visible = L3_VISIBLE_MODEL_TEXTURED | L3_VISIBLE_THRESLIGHT,
		.visual.model = &scene_data->tv_model,
		.visual_type = ENGINE_VISUAL_MODEL,
		.view_range = 131072,
		.collisions = 0,
		.process = 0,
		.data = 0,
	};

	scene_data->tv = engine_add_object(tmp);
	camera_animation_objects[2] = scene_data->tv;

	L3_transform3DSet(1.8 * L3_F, 1.3*L3_F, 0*L3_F,0,0,0,4*L3_F,16*L3_F,0.5*L3_F,&(tmp.visual.transform));
	tmp.view_range = 32 * L3_F;
	tmp.visual_type = ENGINE_VISUAL_LIGHT;
	tmp.visual.solid_color = 0x80;
	//tv_light_objects[0] = engine_add_object(tmp);
	scene_data->tv_light = engine_add_object(tmp);

/*
	L3_transform3DSet(-1 * L3_F, 2*L3_F, -1.54*L3_F,0,0,0,4*L3_F,16*L3_F,0.5*L3_F,&(tmp.visual.transform));
	tmp.view_range = 32 * L3_F;
	tmp.visual_type = ENGINE_VISUAL_LIGHT;
	tmp.visual.solid_color = 0x80;
	engine_add_object(tmp);*/

	//L3_transform3DSet(-1.0 * L3_F,1.3*L3_F,-0.4*L3_F,-3,-0.24*L3_F,0,L3_F,L3_F,L3_F,&(tmp.visual.transform));
	L3_transform3DSet(1.3 * L3_F,1.97*L3_F,-2.79*L3_F,0,-0.25*L3_F,0,L3_F,L3_F,L3_F,&(tmp.visual.transform));
	tmp.view_range = 1 * L3_F;
	tmp.visual_type = ENGINE_VISUAL_NOTHING;
	camera_animation_objects[0] = engine_add_object(tmp);
	camera_animation_objects[1] = camera_animation_objects[0];

	L3_Camera *camera = engine_getcamera();
	camera->focalLength = 450;
}

static void tv_scene_pf(Engine_Scene *scene)
{
	TV_Scene_Data *scene_data = (TV_Scene_Data*)scene->data;

	//utility_animation_process(&tv_light_animation);
	utility_animation_process(&camera_animation_slow);
	utility_animation_process(&camera_animation_fast);

	L3_Camera *camera = engine_getcamera();
	camera->transform = camera_animation_objects[0]->visual.transform;
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
	.inf = tv_scene_init,
	.dif = tv_scene_deinit,
	.data = &tv_scene_data,
	.filters = tv_scene_filters,
	.filters_count = ARRAY_SIZE(tv_scene_filters),
	.statics = map,
	.statics_count = sizeof(map) / sizeof(*map),
};

#include <zephyr/kernel.h>
#include <zephyr/random/random.h>

#include <lvgl.h>

#include "engine.h"
#include "generated_objects.h"
#include "utility.h"
#include "filters.h"

#include "space_bk.h"
#include "space_dn.h"
#include "space_ft.h"
#include "space_lf.h"
#include "space_rt.h"
#include "space_up.h"

#include "ui_style.h"

typedef struct Alien_Ship_Scene_Data_s {
	bool					yes;
	Engine_Object			*ship;
	Engine_Object			*ship_rotate;
	Engine_Object			*player;
	lv_obj_t				*ui;
	lv_obj_t				*rect;
	lv_obj_t				*text;
	lv_obj_t				*text2;
} Alien_Ship_Scene_Data;

static ObjectProcess_FrameArray_Frame ship_rotate_animation_frames[] = {
{.type = 2, .transform = {.translation = {0}, .rotation = {.x = 0, .y = 0, .z = 2 }, .scale = {0}}},
{.type = 2, .transform = {.translation = {0}, .rotation = {.x = 0, .y = 0, .z = 2 }, .scale = {0}}},
};

static const ObjectProcess_FrameArray ship_rotate_animation_framearray = {
	.len = ARRAY_SIZE(ship_rotate_animation_frames),
	.loop = false,
	.frames = ship_rotate_animation_frames,
};

static Engine_Object *ship_rotate_animation_objects[1];

static Animation ship_rotate_animation = ANIMATION_INIT(
	ship_rotate_animation_objects,
	(void*[]) {(void*)&ship_rotate_animation_framearray},
	(AnimationObjectProcess[]) {utility_animation_objectprocess_framearray},
	1, 30,
	ARRAY_SIZE(ship_rotate_animation_frames),
	true
);

static void ship_thrust_animation_pf(Engine_Object *object, void* data, uint64_t frame_id)
{
	L3_Unit s = L3_F + ((((L3_Unit)sys_rand16_get()) - (L3_Unit)0x7FFF) * L3_F) / 0x7FFFF;
	L3_Unit r = (L3_Unit)sys_rand8_get() / 8;

	object->visual.transform.scale.z = s;
	object->visual.transform.rotation.z = r;
}

static Engine_Object *ship_thrust_animation_objects[4];

static Animation ship_thrust_animation = ANIMATION_INIT(
	ship_thrust_animation_objects,
	((void*[]) {NULL, NULL, NULL, NULL}),
	((AnimationObjectProcess[]) {ship_thrust_animation_pf, ship_thrust_animation_pf, ship_thrust_animation_pf, ship_thrust_animation_pf}),
	4, 30,
	2,
	true
);

#define CAM_ANIM_0(i, _) {.type = 2, .transform = {.translation = {.x = 0, .y = 0, .z = 0}, .rotation = {0}, .scale = {0}}}

static const ObjectProcess_FrameArray_Frame camera_animation_impl_frames[] = {
	LISTIFY(100, CAM_ANIM_0, (,)),
};

static const ObjectProcess_FrameArray camera_animation_impl = {
	.len = ARRAY_SIZE(camera_animation_impl_frames),
	.loop = false,
	.frames = camera_animation_impl_frames,
};

static void camera_animation_focus_pf(Engine_Object *object, void* data, uint64_t frame_id)
{
	Alien_Ship_Scene_Data *scene_data = (Alien_Ship_Scene_Data*)data;

	L3_Camera *camera = engine_getcamera();
	//if (frame_id % 2 == 0) {
		engine_pop_filter();
	//}
	if (frame_id == 0) {
		camera->focalLength = 130;
		engine_append_filter(filter_pixelize_2);
		engine_append_filter(filter_pixelize_4);
		engine_append_filter(filter_pixelize_6);
		engine_append_filter(filter_pixelize_8);
	}
	if (frame_id == 4) {
		camera->focalLength = 150;
		engine_append_filter(filter_pixelize_2);
		engine_append_filter(filter_pixelize_4);
		engine_append_filter(filter_pixelize_6);
		engine_append_filter(filter_pixelize_8);
		lv_obj_set_size(scene_data->rect, CONFIG_RESOLUTION_X / 3.5, CONFIG_RESOLUTION_Y / 3.5);
			lv_obj_center(scene_data->rect);
	}
	if (frame_id == 8) {
		camera->focalLength = 180;
		engine_append_filter(filter_pixelize_2);
		engine_append_filter(filter_pixelize_4);
		engine_append_filter(filter_pixelize_6);
		engine_append_filter(filter_pixelize_8);
		lv_obj_set_size(scene_data->rect, CONFIG_RESOLUTION_X / 3, CONFIG_RESOLUTION_Y / 3);
			lv_obj_center(scene_data->rect);
	}
	if (frame_id == 16) {
		camera->focalLength = 400;
		L3_transform3DSet(22.5 * L3_F,-7.5*L3_F,-75*L3_F,0,0,0,L3_F,L3_F,L3_F,&(object->visual.transform));
		engine_append_filter(filter_pixelize_2);
		engine_append_filter(filter_pixelize_4);
		engine_append_filter(filter_pixelize_4);
		engine_append_filter(filter_pixelize_6);
		engine_append_filter(filter_pixelize_6);
		engine_append_filter(filter_pixelize_8);
		engine_append_filter(filter_pixelize_8);
		lv_obj_set_size(scene_data->rect, CONFIG_RESOLUTION_X / 1.2, CONFIG_RESOLUTION_Y / 1.2);
		lv_obj_center(scene_data->rect);
	}
	lv_obj_align_to(scene_data->text, scene_data->rect, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
	lv_label_set_text_fmt(scene_data->text2, "%x%x", sys_rand32_get(), sys_rand32_get());
	lv_obj_align_to(scene_data->text2, scene_data->rect, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
}

static Engine_Object *camera_animation_objects[2];
static void *camera_animation_data[2] = {
	(void*)&camera_animation_impl,
	NULL,
};

static Animation camera_animation = ANIMATION_INIT(
	camera_animation_objects,
	camera_animation_data,
	((AnimationObjectProcess[]) {utility_animation_objectprocess_framearray, camera_animation_focus_pf}),
	2, 8,
	ARRAY_SIZE(camera_animation_impl_frames),
	false
);

static L3_Unit camera_animation_jiggle_r_impulse = 16;
static L3_Unit camera_animation_jiggle_v_impulse = 8;
static L3_Unit camera_animation_jiggle_r;
static L3_Unit camera_animation_jiggle_v;

static void camera_animation_jiggle_pf(Engine_Object *object, void* data, uint64_t frame_id)
{
	if (frame_id == 0) {
		camera_animation_jiggle_r_impulse = -camera_animation_jiggle_r_impulse;
		camera_animation_jiggle_v_impulse = -camera_animation_jiggle_v_impulse;
	} else {
		camera_animation_jiggle_r += camera_animation_jiggle_r_impulse;
		camera_animation_jiggle_v += camera_animation_jiggle_v_impulse;
	}

	L3_Camera *camera = engine_getcamera();
	camera->transform = camera_animation_objects[0]->visual.transform;
	camera->transform.rotation.x += camera_animation_jiggle_r / 64;
	camera->transform.rotation.y += camera_animation_jiggle_v / 64;
}

static Engine_Object *camera_animation_jiggle_objects[1];

Animation camera_animation_jiggle = ANIMATION_INIT(
	camera_animation_jiggle_objects,
	((void*[]) {NULL}),
	((AnimationObjectProcess[]) {camera_animation_jiggle_pf}),
	1, 30,
	50,
	true
);

void particle_cosmic_dust(E_Particle *self)
{
	self->transform.translation.z -= 24*L3_F;
	if (self->transform.translation.z < -150*L3_F) self->life = 1;
}

L3_Skybox skybox = {
	.faces.front = &space_ft,
	.faces.back = &space_bk,
	.faces.left = &space_lf,
	.faces.right = &space_rt,
	.faces.top = &space_up,
	.faces.bottom = &space_dn,
};

static Alien_Ship_Scene_Data alien_ship_scene_data = {0};

static void alien_ship_scene_init(Engine_Scene *scene)
{
	Alien_Ship_Scene_Data *scene_data = (Alien_Ship_Scene_Data*)scene->data;

	camera_animation_data[1] = (void*)scene_data;

	scene_data->ui = lv_obj_create(lv_screen_active());
	lv_obj_set_pos(scene_data->ui, 0, 0);
	lv_obj_set_size(scene_data->ui, CONFIG_RESOLUTION_X, CONFIG_RESOLUTION_Y);
	lv_obj_add_style(scene_data->ui, &ui_style, LV_PART_MAIN);

	scene_data->rect = lv_obj_create(scene_data->ui);
	lv_obj_set_size(scene_data->rect, CONFIG_RESOLUTION_X / 4, CONFIG_RESOLUTION_Y / 4);
	lv_obj_center(scene_data->rect);
	lv_obj_add_style(scene_data->rect, &ui_style, LV_PART_MAIN);
	lv_obj_set_style_border_width(scene_data->rect, 1, LV_PART_MAIN);

	scene_data->text = lv_label_create(scene_data->ui);
	lv_obj_add_style(scene_data->text, &ui_style, LV_PART_MAIN);
	lv_label_set_text_fmt(scene_data->text, "UNKWN-01");
	lv_obj_align_to(scene_data->text, scene_data->rect, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);

	scene_data->text2 = lv_label_create(scene_data->ui);
	lv_obj_add_style(scene_data->text2, &ui_style, LV_PART_MAIN);
	lv_label_set_text_fmt(scene_data->text2, "%x%x", sys_rand32_get(), sys_rand32_get());
	lv_obj_align_to(scene_data->text2, scene_data->rect, LV_ALIGN_OUT_TOP_LEFT, 0, 0);


	L3_normallight_vector((L3_Vec4){0,3*L3_F/8,-5*L3_F/8,L3_F});

	Engine_Object tmp = {0};
	L3_transform3DSet(30 * L3_F,-10*L3_F,-100*L3_F,0,0,0,L3_F,L3_F,L3_F,&(tmp.visual.transform));
	tmp.view_range = 1 * L3_F;
	tmp.visual_type = ENGINE_VISUAL_NOTHING;
	camera_animation_objects[0] = engine_add_object(tmp);
	camera_animation_objects[1] = camera_animation_objects[0];

	L3_Camera *camera = engine_getcamera();
	camera->focalLength = 100;

	scene_data->ship = engine_add_object(ships_alien_ship_object);
	scene_data->ship->view_range = 4096 * L3_F;
	L3_transform3DSet(0 * L3_F,0,0*L3_F,0,256,0,1*L3_F,1*L3_F,1*L3_F,&(scene_data->ship->visual.transform));
	scene_data->ship->visual.config.visible = L3_VISIBLE_MODEL_TEXTURED | L3_VISIBLE_NORMALLIGHT;

	scene_data->ship_rotate = engine_add_object(ships_alien_ship_rotate_object);
	L3_transform3DSet(0 * L3_F,0,0*L3_F,0,256,0,1*L3_F,1*L3_F,1*L3_F,&(scene_data->ship_rotate->visual.transform));
	scene_data->ship_rotate->visual.solid_color = 0x70;
	scene_data->ship_rotate->view_range = 4096 * L3_F;
	scene_data->ship_rotate->visual.config.visible = L3_VISIBLE_MODEL_TEXTURED | L3_VISIBLE_NORMALLIGHT;
	ship_rotate_animation_objects[0] = scene_data->ship_rotate;

	ship_thrust_animation_objects[0] = engine_add_object(ships_engine_alien_1_trail_object);
	L3_transform3DSet(1.5*L3_F,0*L3_F,-47.5*L3_F,0,256,0,0.5*L3_F,0.5*L3_F,1*L3_F,&(ship_thrust_animation_objects[0]->visual.transform));
	ship_thrust_animation_objects[0]->visual.solid_color = 0xB0;
	ship_thrust_animation_objects[0]->view_range = 4096 * L3_F;
	ship_thrust_animation_objects[0]->visual.config.visible = L3_VISIBLE_MODEL_TEXTURED;

	ship_thrust_animation_objects[1] = engine_add_object(ships_engine_alien_1_trail_object);
	L3_transform3DSet(-1.5*L3_F,0*L3_F,-47.5*L3_F,0,256,0,0.5*L3_F,0.5*L3_F,1*L3_F,&(ship_thrust_animation_objects[1]->visual.transform));
	ship_thrust_animation_objects[1]->visual.solid_color = 0xB0;
	ship_thrust_animation_objects[1]->view_range = 4096 * L3_F;
	ship_thrust_animation_objects[1]->visual.config.visible = L3_VISIBLE_MODEL_TEXTURED;

	ship_thrust_animation_objects[2] = engine_add_object(ships_engine_alien_1_trail_object);
	L3_transform3DSet(0*L3_F,2.5*L3_F,-47.5*L3_F,0,256,0,0.5*L3_F,0.5*L3_F,1*L3_F,&(ship_thrust_animation_objects[2]->visual.transform));
	ship_thrust_animation_objects[2]->visual.solid_color = 0xB0;
	ship_thrust_animation_objects[2]->view_range = 4096 * L3_F;
	ship_thrust_animation_objects[2]->visual.config.visible = L3_VISIBLE_MODEL_TEXTURED;

	ship_thrust_animation_objects[3] = engine_add_object(ships_engine_alien_1_trail_object);
	L3_transform3DSet(0*L3_F,-2.5*L3_F,-47.5*L3_F,0,256,0,0.5*L3_F,0.5*L3_F,1*L3_F,&(ship_thrust_animation_objects[3]->visual.transform));
	ship_thrust_animation_objects[3]->visual.solid_color = 0xB0;
	ship_thrust_animation_objects[3]->view_range = 4096 * L3_F;
	ship_thrust_animation_objects[3]->visual.config.visible = L3_VISIBLE_MODEL_TEXTURED;

	engine_append_filter(filter_pixelize_8);

	scene_data->yes = true;
}

static void alien_ship_scene_pf(Engine_Scene *scene)
{
	Alien_Ship_Scene_Data *scene_data = (Alien_Ship_Scene_Data*)scene->data;

	utility_animation_process(&ship_rotate_animation);
	utility_animation_process(&ship_thrust_animation);
	utility_animation_process(&camera_animation);

	L3_lookAt(scene_data->ship->visual.transform.translation, &camera_animation_objects[0]->visual.transform);

	/* Add jiggle after pos copy */
	//utility_animation_process(&camera_animation_jiggle);

	L3_Camera *camera = engine_getcamera();
	camera->transform = camera_animation_objects[0]->visual.transform;
	camera->transform.rotation.x += 10;
	camera->transform.rotation.z += 32;
	camera->transform.rotation.y += 48;

	// L3_Transform3D transform = {0};
	// transform.scale.x = 64*L3_F;
	// transform.scale.y = 64*L3_F;
	// transform.scale.z = 64*L3_F;
	// transform.translation.x = (sys_rand16_get() - 0x7FFF) * 2;
	// transform.translation.y = (sys_rand16_get() - 0x7FFF) * 2;
	// transform.translation.z = 150*L3_F;
	// engine_create_particle(transform, particle_cosmic_dust, billboards_pixel.visual.billboard, E_LIFESPAN(10.0));
}

static void alien_ship_scene_deinit(Engine_Scene *scene)
{
	Alien_Ship_Scene_Data *scene_data = (Alien_Ship_Scene_Data*)scene->data;

	lv_obj_clean(scene_data->ui);
	lv_obj_del_async(scene_data->ui);
}

static Filter_f alien_ship_scene_filters[] = {
	filter_star_bloom_3,
};

Engine_Scene alien_ship_scene = {
	.pf = alien_ship_scene_pf,
	.statics = NULL,
	.statics_count = 0,
	.inf = alien_ship_scene_init,
	.dif = alien_ship_scene_deinit,
	.data = &alien_ship_scene_data,
	.filters = alien_ship_scene_filters,
	.filters_count = ARRAY_SIZE(alien_ship_scene_filters),
	.skybox = &skybox,
};

#include <zephyr/kernel.h>
#include <zephyr/random/random.h>

#include <stdlib.h>

#include <lvgl.h>

#include "engine.h"
#include "generated_objects.h"
#include "utility.h"
#include "filters.h"

#include "point_at_computer_scene/map.h"

#include "ui_style.h"

#include "models/cube.h"

typedef struct Point_At_Computer_Scene_Data_s {
	Engine_Object			*player;
	lv_obj_t				*ui;
	Engine_Object			*billboard;
	L3_Billboard			billboard_data;
	L3_Billboard			smoke;
} Point_At_Computer_Scene_Data;


static int cig_smoke_err = 0;
static void particle_smoke(E_Particle *self)
{
	self->transform.translation.y += 1;
	// if (cig_smoke_err > 28) {
	// } else if (cig_smoke_err > 24) {
	// 	self->transform.translation.x -= 1;
	// } else if (cig_smoke_err > 20) {
 //
	// } else if (cig_smoke_err > 16) {
	// 	self->transform.translation.x += 1;
	// }
	if (self->life > E_LIFESPAN(7.0)) {
		if (cig_smoke_err > 4) {
			self->transform.translation.x += 1;
		} else if (cig_smoke_err < -4) {
			self->transform.translation.x -= 1;
		}
	}
	// self->transform.scale.x *= 0.995;
	// self->transform.scale.y *= 0.995;
	// self->transform.translation.x += (L3_Unit)(sys_rand8_get() > 0x80 ? (sys_rand8_get() > 0xC0 ? 1 : -1) : 0);
	// self->transform.translation.y += (L3_Unit)(sys_rand8_get() > 0x80 ? (sys_rand8_get() > 0xC0 ? 1 : -1) : 1);
}

#define CAM_ANIM_0(i, _) {.type = FRAMEARRAY_FRAME_RELATIVE, .transform = {.translation = {.x = 0, .y = 0, .z = 0}, .rotation = {0}, .scale = {0}}}
#define CAM_ANIM_1(i, _) {.type = FRAMEARRAY_FRAME_RELATIVE, .transform = {.translation = {.x = 0, .y = -1, .z = 0}, .rotation = {0}, .scale = {0}}}
#define CAM_ANIM_2(i, _) {.type = FRAMEARRAY_FRAME_RELATIVE, .transform = {.translation = {.x = 0, .y = 0, .z = 2}, .rotation = {0}, .scale = {0}}}
#define CAM_ANIM_3(i, _) {.type = FRAMEARRAY_FRAME_RELATIVE, .transform = {.translation = {.x = -1, .y = 0, .z = 0}, .rotation = {0}, .scale = {0}}}
#define CAM_ANIM_4(i, _) {.type = FRAMEARRAY_FRAME_RELATIVE, .transform = {.translation = {0}, .rotation = {.x = 0, .y = -1, .z = 0}, .scale = {0}}}

static const ObjectProcess_FrameArray_Frame camera_animation_impl_frames[] = {
	LISTIFY(100, CAM_ANIM_3, (,)),
};

static const ObjectProcess_FrameArray camera_animation_impl = {
	.len = ARRAY_SIZE(camera_animation_impl_frames),
	.loop = false,
	.frames = camera_animation_impl_frames,
};

static Engine_Object *camera_animation_objects[1];
static void *camera_animation_data[1] = {
	(void*)&camera_animation_impl,
};

static Animation camera_animation = ANIMATION_INIT(
	camera_animation_objects,
	camera_animation_data,
	((AnimationObjectProcess[]) {utility_animation_objectprocess_framearray}),
	1, 8,
	ARRAY_SIZE(camera_animation_impl_frames),
	false
);

static void add_smonk(Point_At_Computer_Scene_Data *scene_data)
{
	L3_Transform3D transform;
	L3_transform3DSet(0.52 * L3_F, 1.114 *L3_F, 0*L3_F,0,0,0,0.01*L3_F,0.01*L3_F,0.01*L3_F,&(transform));
	engine_create_particle(transform, &particle_smoke, &scene_data->smoke, E_LIFESPAN(8.0));
	cig_smoke_err += sys_rand8_get() > 0x80 ? 1 : - 1;
	if (cig_smoke_err > 5) {
		cig_smoke_err = 0;
	}
	if (cig_smoke_err < -5) {
		cig_smoke_err = 0;
	}
}

static Point_At_Computer_Scene_Data point_at_computer_scene_data = {0};

static void point_at_computer_scene_init(Engine_Scene *scene)
{
	Point_At_Computer_Scene_Data *scene_data = (Point_At_Computer_Scene_Data*)scene->data;

	L3_normallight_vector((L3_Vec4){0,3*L3_F/4,1*L3_F/4,L3_F});

	scene_data->ui = lv_obj_create(lv_screen_active());
	lv_obj_set_pos(scene_data->ui, 0, 0);
	lv_obj_set_size(scene_data->ui, CONFIG_RESOLUTION_X, CONFIG_RESOLUTION_Y);
	lv_obj_add_style(scene_data->ui, &ui_style, LV_PART_MAIN);

	// scene_data->text = lv_label_create(scene_data->ui);
	// lv_obj_add_style(scene_data->text, &ui_style, LV_PART_MAIN);
	// lv_label_set_text_fmt(scene_data->text, "");
	// lv_obj_align(scene_data->text, LV_ALIGN_BOTTOM_MID, 0, 0);

	Engine_Object tmp = {0};
	//L3_transform3DSet(-1.0 * L3_F,1.3*L3_F,-0.4*L3_F,-3,-0.24*L3_F,0,L3_F,L3_F,L3_F,&(tmp.visual.transform));
	L3_transform3DSet(0.80884 * L3_F, 0.86071*L3_F, 2.2105*L3_F,8.5,-256,0,L3_F,L3_F,L3_F,&(tmp.visual.transform));
	tmp.view_range = 1 * L3_F;
	tmp.visual_type = ENGINE_VISUAL_NOTHING;
	scene_data->player = engine_add_object(tmp);
	camera_animation_objects[0] = scene_data->player;

	scene_data->billboard = engine_add_object(scenes_point_at_computer_scene_billboard);
	memcpy(&scene_data->billboard_data, scene_data->billboard->visual.billboard, sizeof(L3_Billboard));
	scene_data->billboard->visual.billboard = &scene_data->billboard_data;
	scene_data->billboard->visual.config.visible = L3_VISIBLE_BILLBOARD;
	L3_transform3DSet(0.75 * L3_F, 1.114 *L3_F, 0*L3_F,0,0,0,0.16*L3_F,0.16*L3_F,0.16*L3_F,&(scene_data->billboard->visual.transform));
	scene_data->billboard_data.transparency.transparency_threshold = 0x20;

	memcpy(&scene_data->smoke, billboards_smoke.visual.billboard, sizeof(L3_Billboard));
	scene_data->smoke.transparency.transparency_threshold = 0x50;
	scene_data->smoke.transparency.transparency = 0x40;

	for (int j = 0; j < 256; j++) {
		add_smonk(scene_data);
		engine_preload_particles();
	}

	L3_Camera *camera = engine_getcamera();
	camera->focalLength = 1387;
}

static void point_at_computer_scene_pf(Engine_Scene *scene)
{
	Point_At_Computer_Scene_Data *scene_data = (Point_At_Computer_Scene_Data*)scene->data;

	utility_animation_process(&camera_animation);

	L3_Camera *camera = engine_getcamera();
	camera->transform = camera_animation_objects[0]->visual.transform;
	add_smonk(scene_data);
}

static void point_at_computer_scene_deinit(Engine_Scene *scene)
{
	Point_At_Computer_Scene_Data *scene_data = (Point_At_Computer_Scene_Data*)scene->data;

	lv_obj_clean(scene_data->ui);
	lv_obj_del_async(scene_data->ui);
}

static Filter_f point_at_computer_scene_filters[] = {
	//filter_bloom,
};

Engine_Scene point_at_computer_scene = {
	.pf = point_at_computer_scene_pf,
	.inf = point_at_computer_scene_init,
	.dif = point_at_computer_scene_deinit,
	.data = &point_at_computer_scene_data,
	.filters = point_at_computer_scene_filters,
	.filters_count = ARRAY_SIZE(point_at_computer_scene_filters),
	.statics = map,
	.statics_count = sizeof(map) / sizeof(*map),
};

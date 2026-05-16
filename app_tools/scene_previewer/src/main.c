#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/input/input.h>
#include <zephyr/timing/timing.h>
#include <stdio.h>
#include <stdlib.h>
#include <zephyr/random/random.h>
#include <math.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#include "engine.h"

#include "utility.h"

#include "logo_scene.h"

#include "scenes.h"

#include "ui_style.h"

static const struct device *display_device = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

int blit_display(L3_COLORTYPE *buffer, uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y)
{
	return blit_select_for_me(buffer, x, y, size_x, size_y);
}

typedef struct {
	L3_Unit vx;
	L3_Unit vy;
	L3_Unit z;
	L3_Unit x;
	L3_Unit speedmul;
	L3_Unit jump;
	L3_Unit xrot;
} Controls;

static Engine_Object* player = 0;
static Controls controls = {0};

static void process() {
	if (player != 0) {
		//LOG_ERR("\r\b%d %d %d", player->visual.transform.translation.x, player->visual.transform.translation.y, player->visual.transform.translation.z);
		player->visual.transform.rotation.y += controls.vy;
		player->visual.transform.rotation.x += controls.vx;
		if (player->visual.transform.rotation.y > L3_F/2) player->visual.transform.rotation.y = -L3_F/2;
		if (player->visual.transform.rotation.y < -L3_F/2) player->visual.transform.rotation.y = L3_F/2;
		if (player->visual.transform.rotation.x > L3_F/2) player->visual.transform.rotation.x = -L3_F/2;
		if (player->visual.transform.rotation.x < -L3_F/2) player->visual.transform.rotation.x = L3_F/2;

		L3_Vec4 forward = {0, 0, L3_F, L3_F};
		L3_Vec4 left = {-L3_F, 0, 0, L3_F};
		L3_Mat4 transMat;

		L3_makeRotationMatrixZXY(player->visual.transform.rotation.x,
							player->visual.transform.rotation.y,
							0,
							transMat);

		L3_vec3Xmat4(&forward, transMat);
		L3_vec3Xmat4(&left, transMat);

		player->visual.transform.translation.x += controls.z * forward.x / L3_F;
		player->visual.transform.translation.y += controls.z * forward.y / L3_F;
		player->visual.transform.translation.z += controls.z * forward.z / L3_F;
		player->visual.transform.translation.x += controls.x * left.x / L3_F;
		player->visual.transform.translation.y += controls.x * left.y / L3_F;
		player->visual.transform.translation.z += controls.x * left.z / L3_F;

		L3_Camera *camera = engine_getcamera();
		camera->transform = player->visual.transform;
	}
}

static bool slowed = false;

static void update_controls(struct input_event *evt, void *user_data)
{
	Controls *cont = user_data;

	if (evt->code == INPUT_KEY_UP) {
		if (evt->value)
			cont->vx = 5;
		else
			cont->vx = 0;
	} else if (evt->code == INPUT_KEY_DOWN) {
		if (evt->value)
			cont->vx = -5;
		else
			cont->vx = 0;
	}
	if (evt->code == INPUT_KEY_LEFT) {
		if (evt->value)
			cont->vy = 15;
		else
			cont->vy = 0;
	} else if (evt->code == INPUT_KEY_RIGHT) {
		if (evt->value)
			cont->vy = -15;
		else
			cont->vy = 0;
	}
	if (evt->code == INPUT_KEY_W) {
		if (evt->value)
			cont->z = E_SPEED(5*L3_F*cont->speedmul);
		else
			cont->z = 0;
	} else if (evt->code == INPUT_KEY_S) {
		if (evt->value)
			cont->z = -E_SPEED(5*L3_F*cont->speedmul);
		else
			cont->z = 0;
	}

	if (evt->code == INPUT_KEY_A) {
		if (evt->value)
			cont->x = E_SPEED(5*L3_F*cont->speedmul);
		else
			cont->x = 0;
	} else if (evt->code == INPUT_KEY_D) {
		if (evt->value)
			cont->x = -E_SPEED(5*L3_F*cont->speedmul);
		else
			cont->x = 0;
	}

	if (evt->code == INPUT_KEY_LEFTSHIFT) {
		if (evt->value)
			cont->speedmul = 8;
		else
			cont->speedmul = 1;
	}

	if (evt->code == INPUT_KEY_SPACE) {
		if (!evt->value) {
			if (slowed) {
				engine_set_process_fps(CONFIG_TARGET_PROCESS_FPS);
				slowed = false;
			} else {
				engine_set_process_fps(1);
				slowed = true;
			}
		}
	}
}

INPUT_CALLBACK_DEFINE(0, update_controls, &controls);

int main()
{
	for (int i = 0; i < DT_ZEPHYR_DISPLAYS_COUNT; i++) {
		if (!device_is_ready(engine_display_devices[i])) {
			LOG_ERR("Display device %d is not ready", i);
			return -ENODEV;
		}
	}

	timing_init();
	timing_start();

	k_msleep(100);
	display_blanking_on(display_device);
	display_blanking_off(display_device);

	engine_UI_set_area(0,0, CONFIG_RESOLUTION_X, CONFIG_RESOLUTION_Y);

	//display_set_contrast(display_device, 210);

	timing_init();
	timing_start();

	init_ui_style();
	init_engine(&process);

	//lv_obj_add_style(lv_screen_active(), &ui_style, LV_PART_MAIN);

	engine_switchscene(&logo_scene);
	k_msleep(100);

	//engine_getcamera()->focalLength = 12;

	engine_switchscene(&SCENE_TO_PREVIEW);

#ifndef CONFIG_SCENE_HAS_PLAYER
	/* 'player' object */
	Engine_Object tmp = {0};
	L3_transform3DSet(0 * L3_F,0,-3*L3_F,0,0,0,L3_F,L3_F,L3_F,&(tmp.visual.transform));
	tmp.view_range = 16 * L3_F;
	tmp.visual_type = ENGINE_VISUAL_NOTHING;
	player = engine_add_object(tmp);
	controls.speedmul = 1;
#endif

	return 0;
}

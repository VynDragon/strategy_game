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

#include "game/state.h"
#include "ui/ui.h"

static const struct device *display_device = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

int blit_display(L3_COLORTYPE *buffer, uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y)
{
	return blit_select_for_me(buffer, x, y, size_x, size_y);
}

static const char *speaks[] = {
	"WOG WOG",
	"SHAPRFIUGFIUEWIUFBHYIKEGAGKBGEKBJJBAGJBKGJBKGDJBKDGBJKDGBJKGDBJKDGBJKDGBDJGK",
	"Lorem Ipsum is simply dummy text of the printing and typesetting industry.",
	"Lorem Ipsum has been the industry's standard dummy text ever since 1966, when designers at Letraset and James Mosley, the librarian at St Bride Printing Library in L.",
	"\0\0"
};

static void speak_cb(void *userdata)
{
	const char **speakies = userdata;

	if (speakies[1][0] != 0) {
		ui_set_speak_cb(speak_cb, &speakies[1], speakies[1]);
	}
}

static void ui_test_scene_init(Engine_Scene *scene)
{
	ui_set_speak_cb(speak_cb, speaks, speaks[0]);
	return;
}

static uint32_t cnt = 0;

static void ui_test_scene_pf(Engine_Scene *scene)
{
	if (cnt % 60 == 59) {
		input_report(0, INPUT_EV_KEY, INPUT_KEY_SPACE, 1, false, K_FOREVER);
		input_report(0, INPUT_EV_KEY, INPUT_KEY_SPACE, 0, false, K_FOREVER);
	}
	cnt++;
}

static void ui_test_scene_deinit(Engine_Scene *scene)
{
	return;
}

static Filter_f ui_test_scene_filters[] = {
	//filter_bloom,
};

Engine_Scene ui_test_scene = {
	.pf = ui_test_scene_pf,
	.inf = ui_test_scene_init,
	.dif = ui_test_scene_deinit,
	.data = NULL,
	.filters = ui_test_scene_filters,
	.filters_count = ARRAY_SIZE(ui_test_scene_filters),
	.statics = NULL,
	.statics_count = 0,
};

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
	//display_set_contrast(display_device, 210);

	init_engine(&engine_pf);

	//engine_getcamera()->focalLength = 12;

	k_msleep(1000);

	engine_switchscene(&ui_test_scene);

	return 0;
}

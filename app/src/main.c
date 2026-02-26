#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <zephyr/timing/timing.h>
#include <stdio.h>
#include <stdlib.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#include "engine.h"
#include "logo_scene.h"
#include "context.h"

static const struct device *display_device = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

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

	init_engine(&context_engine_pf);

	engine_switchscene(&logo_scene);

	return 0;
}

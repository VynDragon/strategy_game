#include <zephyr/kernel.h>
#include "engine.h"

typedef struct Menu_Scene_Data_s {
	bool yes;
} Menu_Scene_Data;

static Menu_Scene_Data menu_scene_data = {0};

static void menu_scene_init(void *data)
{
	Menu_Scene_Data *scene_data = (Menu_Scene_Data*)data;
	scene_data->yes = true;
}

Engine_Scene strategy_menu_scene = {
	.pf = NULL,
	.statics = NULL,
	.statics_count = 0,
	.inf = menu_scene_init,
	.data = &menu_scene_data,
	.filters_count = 0,
	.skybox = NULL,
};

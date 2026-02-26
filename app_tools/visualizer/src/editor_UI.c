#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/input/input.h>
#include <zephyr/timing/timing.h>
#include <stdio.h>
#include <stdlib.h>
#include <zephyr/random/random.h>
#include <math.h>
#include <lvgl.h>
#include <lvgl_mem.h>
#include <lvgl_zephyr.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(editor_ui);

#include "editor.h"

#include "engine.h"

#include "generated_objects.h"

static lv_display_t *lv_displays[DT_ZEPHYR_DISPLAYS_COUNT];

static lv_display_t *lvgl_display;

Engine_Object *object;

static lv_obj_t *select_object_list;

lv_style_t style_transp;

lv_obj_t * background_render;
static lv_img_dsc_t background_render_img;
extern L3_COLORTYPE blit_lvgl_buffer[L3_RESOLUTION_X * L3_RESOLUTION_Y];

static void add_obj(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_target_obj(e);
	size_t id = (size_t)lv_event_get_user_data(e);
	if(code == LV_EVENT_CLICKED) {

		if (object != NULL)
		{
			engine_remove_object(object);
			engine_optimize_object_table();
			object = NULL;
		}

		object = engine_add_object(generated_object_list[id]);

		L3_transform3DSet(0, 0, 0,0,0,0,L3_F,L3_F,L3_F,&(object->visual.transform));
		if (object->collisions != NULL) {
		}
		printf("Adding: %s, id: %d\n", lv_list_get_button_text(select_object_list, obj), id);
		printf("objcount: %d\n", engine_object_getcnt() + engine_statics_getcnt());
	}
}

int init_editor_UI(void)
{
	lv_display_t *d = NULL;

	/* inverted order for some reason */
	for (int i = 0; i < DT_ZEPHYR_DISPLAYS_COUNT; i++) {
		d = lv_display_get_next(d);
		if (d == NULL) {
			printf("Invalid LV display %d object", i);
			return 0;
		}
		lv_displays[i] = d;
	}
	lvgl_display = lv_displays[1];

	lv_display_set_default(lvgl_display);
	engine_UI_set_area(0, 0, L3_RESOLUTION_X, L3_RESOLUTION_Y);
	extern int set_lvgl_rendering_cb(lv_display_t *display);
	set_lvgl_rendering_cb(lvgl_display);

	background_render = lv_img_create(lv_screen_active());
	background_render_img.header.magic = LV_IMAGE_HEADER_MAGIC;
	background_render_img.header.w = L3_RESOLUTION_X;
	background_render_img.header.h = L3_RESOLUTION_Y;
	background_render_img.data_size = L3_RESOLUTION_X * L3_RESOLUTION_Y;
	background_render_img.header.cf = LV_COLOR_FORMAT_L8;
	background_render_img.data = (uint8_t *)blit_lvgl_buffer;
	lv_img_set_src(background_render, &background_render_img);
	lv_obj_set_size(background_render, 1024, 512);
	lv_obj_center(background_render);

	select_object_list = lv_list_create(lv_screen_active());
	lv_obj_set_size(select_object_list, 256, 512);
	lv_obj_align(select_object_list, LV_ALIGN_TOP_RIGHT, 0,0);


	lv_style_init(&style_transp);
	lv_style_set_bg_opa(&style_transp, LV_OPA_TRANSP);
	//lv_style_set_border_width(&style_transp, 0);
	lv_style_set_text_color(&style_transp, lv_color_hex3(0xFFF));
	lv_style_set_radius(&style_transp, 0);
	lv_style_set_border_width(&style_transp, 1);
	lv_style_set_pad_all(&style_transp, 1);
	lv_obj_add_style(select_object_list, &style_transp, 0);

	for (int i = 0; i < ARRAY_SIZE(generated_object_list); i++) {
		lv_obj_t * btn;
		btn = lv_list_add_button(select_object_list, NULL, generated_object_list_names[i]);
		lv_obj_add_style(btn, &style_transp, 0);
		lv_obj_add_event_cb(btn, add_obj, LV_EVENT_CLICKED, (void*)i);
	}

	return 0;
}

int do_editor_UI(void)
{
	lv_display_set_default(lvgl_display);

	lv_img_set_src(background_render, &background_render_img);
	lv_timer_handler();

	return 0;
}

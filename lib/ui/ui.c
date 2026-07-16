
#include <stdio.h>
#include <lvgl.h>

#include "game/state.h"

static state_ui ui_state = {
	.speak_obj = NULL,
	.speak_cb = NULL,
};

static void switchout(void)
{
	ui_state.speak_obj = NULL;
	ui_state.speak_cb = NULL;
	lv_obj_clean(ui_state.root);
}

static void switchin(void)
{
	if (get_state()->focus != FOCUS_UI) {
		get_focus(FOCUS_UI);
	}
}

int init_ui(void)
{
	get_state()->ui = &ui_state;

	ui_state.root = lv_obj_create(lv_screen_active());
	if (ui_state.root == NULL)
		return -ENOMEM;
	lv_obj_set_pos(ui_state.root, 0, 0);
	lv_obj_set_size(ui_state.root, CONFIG_RESOLUTION_X, CONFIG_RESOLUTION_Y);
	lv_obj_add_style(ui_state.root, &ui_style, LV_PART_MAIN);
	return 0;
}

void ui_unfocus(void)
{
	switchout();
	release_focus(FOCUS_UI);
}

static int ui_set_speak_va(const char *fmt, va_list args)
{
	switchin();
	vsnprintf(ui_state.speak_buf, UI_SPEAK_BOX_CHRCNT, fmt, args);

	if (ui_state.speak_obj == NULL) {
		ui_state.speak_obj = lv_obj_create(ui_state.root);
		lv_obj_set_size(ui_state.speak_obj, UI_SPEAK_BOX_SIZE_X, UI_SPEAK_BOX_SIZE_Y);
		lv_obj_add_style(ui_state.speak_obj, &ui_style, LV_PART_MAIN);
		lv_obj_set_style_border_width(ui_state.speak_obj, 1, LV_PART_MAIN);
		ui_state.speak_obj_text = lv_label_create(ui_state.speak_obj);
		lv_obj_add_style(ui_state.speak_obj_text, &ui_style, LV_PART_MAIN);
		lv_label_set_long_mode(ui_state.speak_obj_text, LV_LABEL_LONG_MODE_WRAP);
	}

	lv_label_set_text(ui_state.speak_obj_text, ui_state.speak_buf);
	lv_obj_set_size(ui_state.speak_obj_text, UI_SPEAK_BOX_SIZE_X, UI_SPEAK_BOX_SIZE_Y);
	lv_obj_align_to(ui_state.speak_obj, ui_state.root, LV_ALIGN_BOTTOM_MID, 0, 1);

	return 0;
}

int ui_set_speak(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	int ret = ui_set_speak_va(fmt, args);
	va_end(args);

	return ret;
}

int ui_set_speak_cb(void (*cb)(void *userdata), void *userdata, const char *fmt, ...)
{
	va_list args;

	ui_state.speak_cb = cb;
	ui_state.speak_cb_userdata = userdata;

	va_start(args, fmt);
	int ret = ui_set_speak_va(fmt, args);
	va_end(args);

	return ret;
}

void ui_input_handler(uint32_t evt, void *user_data)
{
	if ((evt == UI_INPUT_BACK || evt == UI_INPUT_ENTER) && ui_state.speak_cb != NULL)
	{
		ui_cb cb = ui_state.speak_cb;
		ui_state.speak_cb = NULL;
		cb(ui_state.speak_cb_userdata);
	}
}

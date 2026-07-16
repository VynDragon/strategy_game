#pragma once

#include "ui/ui_style.h"

#include <zephyr/input/input.h>

/* Movement controls = Menu controls */
#define UI_INPUT_UP		INPUT_MOV_UP
#define UI_INPUT_DOWN		INPUT_MOV_DOWN
#define UI_INPUT_LEFT		INPUT_MOV_LEFT
#define UI_INPUT_RIGHT		INPUT_MOV_RIGHT
#define UI_INPUT_ENTER		INPUT_BUTTON_0
#define UI_INPUT_BACK		INPUT_BUTTON_1

#define UI_INPUT_F1		INPUT_BUTTON_F1
#define UI_INPUT_F2		INPUT_BUTTON_F2
#define UI_INPUT_F3		INPUT_BUTTON_F3
#define UI_INPUT_F4		INPUT_BUTTON_F4

#define UI_SPEAK_BOX_SIZE_X	128
#define UI_SPEAK_BOX_SIZE_Y	32
/* Half the screen at 128x64, so 256 characters using 4 pixels font */
#define UI_SPEAK_BOX_CHRCNT	256

typedef void (*ui_cb)(void *userdata);

typedef struct state_ui_s {
	lv_obj_t	*root;
	lv_obj_t	*speak_obj;
	lv_obj_t	*speak_obj_text;
	char		speak_buf[UI_SPEAK_BOX_CHRCNT];
	ui_cb		speak_cb;
	void 		*speak_cb_userdata;
} state_ui;

int init_ui(void);

void ui_unfocus(void);

void ui_input_handler(uint32_t evt, void *user_data);

int ui_set_speak(const char *fmt, ...);

int ui_set_speak_cb(void (*cb)(void *userdata), void *userdata, const char *fmt, ...);

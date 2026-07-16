#pragma once

#include <zephyr/input/input.h>

#include "configuration.h"

#define MAX_INPUT_HANDLERS	2

/* Button events (Press) -------------------------------- */
#define INPUT_MOV_UP		0x10
#define INPUT_MOV_DOWN		0x11
#define INPUT_MOV_LEFT		0x12
#define INPUT_MOV_RIGHT		0x13

#define INPUT_CAM_UP		0x20
#define INPUT_CAM_DOWN		0x21
#define INPUT_CAM_LEFT		0x22
#define INPUT_CAM_RIGHT		0x23

/* Enter / Jump */
#define INPUT_BUTTON_0		0x30
/* Back / Shift / Run? */
#define INPUT_BUTTON_1		0x31
/* Buttons under display from left to right */
#define INPUT_BUTTON_F1		0x32
#define INPUT_BUTTON_F2		0x33
#define INPUT_BUTTON_F3		0x34
#define INPUT_BUTTON_F4		0x35

/* Button events (Hold) -------------------------------- */
#define INPUT_MOV_UP_H		0x110
#define INPUT_MOV_DOWN_H	0x111
#define INPUT_MOV_LEFT_H	0x112
#define INPUT_MOV_RIGHT_H	0x113

#define INPUT_CAM_UP_H		0x120
#define INPUT_CAM_DOWN_H	0x121
#define INPUT_CAM_LEFT_H	0x122
#define INPUT_CAM_RIGHT_H	0x123

#ifdef CONFIG_STRATEGY_UI
#include "ui/ui.h"
#endif

typedef void (*input_handler)(uint32_t evt, void *user_data);

typedef enum state_focus_s {
	FOCUS_GAME = 0,
	FOCUS_UI = 1,
	FOCUS_PRIV
} state_focus;

typedef struct state_input_s {
	bool mov_up;
	bool mov_down;
	bool mov_left;
	bool mov_right;
	bool cam_up;
	bool cam_down;
	bool cam_left;
	bool cam_right;
	bool b0;
	bool b1;
	bool f1;
	bool f2;
	bool f3;
	bool f4;
} state_input;

typedef struct state_s {
	state_focus	focus;
	input_handler	input_handlers[MAX_INPUT_HANDLERS];
	void		*input_handlers_data[MAX_INPUT_HANDLERS];
	int64_t		uptime;
	configuration	configuration;
	state_input	input;
#ifdef CONFIG_STRATEGY_UI
	state_ui	*ui;
#endif
} state;

/* Get game state singleton */
state *get_state(void);

/* Engine processfunc */
void engine_pf(void);

/* Route input to focus from zephyr */
void game_input(struct input_event *evt, void *user_data);

void get_focus(state_focus focus);

void release_focus(state_focus focus);

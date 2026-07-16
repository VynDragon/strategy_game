#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(state);

#include "engine.h"
#include "state.h"
#include "logo_scene.h"

static k_timepoint_t logo_timeout;

static state game_state = {
	.focus = FOCUS_GAME,
	.input_handlers = {
		[FOCUS_UI] = ui_input_handler,
	},
	.uptime = 0,
	.configuration = {
		.mov_up = INPUT_KEY_W,
		.mov_down = INPUT_KEY_S,
		.mov_left = INPUT_KEY_A,
		.mov_right = INPUT_KEY_D,
		.cam_up = INPUT_KEY_UP,
		.cam_down = INPUT_KEY_DOWN,
		.cam_left = INPUT_KEY_LEFT,
		.cam_right = INPUT_KEY_RIGHT,
		.b0 = INPUT_KEY_SPACE,
		.b1 = INPUT_KEY_LEFTSHIFT,
		.f1 = INPUT_KEY_1,
		.f2 = INPUT_KEY_2,
		.f3 = INPUT_KEY_3,
		.f4 = INPUT_KEY_4,
	},
};

state *get_state(void)
{
	return &game_state;
}

void get_focus(state_focus focus)
{
	game_state.focus = focus;
}

void release_focus(state_focus focus)
{
	if (focus == game_state.focus) {
		game_state.focus = game_state.focus > FOCUS_GAME ? focus - 1 : FOCUS_GAME;
	}
}

static int engine_init(void)
{
	int ret;

	engine_UI_set_area(0,0, CONFIG_RESOLUTION_X, CONFIG_RESOLUTION_Y);
	init_ui_style();
#ifdef CONFIG_STRATEGY_UI
	ret = init_ui();
#endif
	logo_timeout = sys_timepoint_calc(K_SECONDS(1));
	engine_switchscene(&logo_scene);

	return ret;
}

static void game_input_dispatch(uint32_t evt)
{
	if (game_state.input_handlers[game_state.focus] != NULL) {
		game_state.input_handlers[game_state.focus](evt, game_state.input_handlers_data);
	}
}

void game_input(struct input_event *evt, void *user_data)
{
	if (evt->code == game_state.configuration.mov_up) {
		if (evt->value == 0) {
			game_state.input.mov_up = false;
		} else {
			if (!game_state.input.mov_up) {
				game_state.input.mov_up = true;
				game_input_dispatch(INPUT_MOV_UP);
			}
		}
	}

	if (evt->code == game_state.configuration.mov_down) {
		if (evt->value == 0) {
			game_state.input.mov_down = false;
		} else {
			if (!game_state.input.mov_down) {
				game_state.input.mov_down = true;
				game_input_dispatch(INPUT_MOV_DOWN);
			}
		}
	}

	if (evt->code == game_state.configuration.mov_left) {
		if (evt->value == 0) {
			game_state.input.mov_left = false;
		} else {
			if (!game_state.input.mov_left) {
				game_state.input.mov_left = true;
				game_input_dispatch(INPUT_MOV_LEFT);
			}
		}
	}

	if (evt->code == game_state.configuration.mov_right) {
		if (evt->value == 0) {
			game_state.input.mov_right = false;
		} else {
			if (!game_state.input.mov_right) {
				game_state.input.mov_right = true;
				game_input_dispatch(INPUT_MOV_RIGHT);
			}
		}
	}

	if (evt->code == game_state.configuration.cam_up) {
		if (evt->value == 0) {
			game_state.input.cam_up = false;
		} else {
			if (!game_state.input.cam_up) {
				game_state.input.cam_up = true;
				game_input_dispatch(INPUT_CAM_UP);
			}
		}
	}

	if (evt->code == game_state.configuration.cam_down) {
		if (evt->value == 0) {
			game_state.input.cam_down = false;
		} else {
			if (!game_state.input.cam_down) {
				game_state.input.cam_down = true;
				game_input_dispatch(INPUT_CAM_DOWN);
			}
		}
	}

	if (evt->code == game_state.configuration.cam_left) {
		if (evt->value == 0) {
			game_state.input.cam_left = false;
		} else {
			if (!game_state.input.cam_left) {
				game_state.input.cam_left = true;
				game_input_dispatch(INPUT_CAM_LEFT);
			}
		}
	}

	if (evt->code == game_state.configuration.cam_right) {
		if (evt->value == 0) {
			game_state.input.cam_right = false;
		} else {
			if (!game_state.input.cam_right) {
				game_state.input.cam_right = true;
				game_input_dispatch(INPUT_CAM_RIGHT);
			}
		}
	}

	if (evt->code == game_state.configuration.b0) {
		if (evt->value == 0) {
			game_state.input.b0 = false;
		} else {
			if (!game_state.input.b0) {
				game_state.input.b0 = true;
				game_input_dispatch(INPUT_BUTTON_0);
			}
		}
	}

	if (evt->code == game_state.configuration.b1) {
		if (evt->value == 0) {
			game_state.input.b1 = false;
		} else {
			if (!game_state.input.b1) {
				game_state.input.b1 = true;
				game_input_dispatch(INPUT_BUTTON_1);
			}
		}
	}

	if (evt->code == game_state.configuration.f1) {
		if (evt->value == 0) {
			game_state.input.f1 = false;
		} else {
			if (!game_state.input.f1) {
				game_state.input.f1 = true;
				game_input_dispatch(INPUT_BUTTON_F1);
			}
		}
	}

	if (evt->code == game_state.configuration.f2) {
		if (evt->value == 0) {
			game_state.input.f2 = false;
		} else {
			if (!game_state.input.f2) {
				game_state.input.f2 = true;
				game_input_dispatch(INPUT_BUTTON_F2);
			}
		}
	}

	if (evt->code == game_state.configuration.f3) {
		if (evt->value == 0) {
			game_state.input.f3 = false;
		} else {
			if (!game_state.input.f3) {
				game_state.input.f3 = true;
				game_input_dispatch(INPUT_BUTTON_F3);
			}
		}
	}

	if (evt->code == game_state.configuration.f4) {
		if (evt->value == 0) {
			game_state.input.f4 = false;
		} else {
			if (!game_state.input.f4) {
				game_state.input.f4 = true;
				game_input_dispatch(INPUT_BUTTON_F4);
			}
		}
	}
}

INPUT_CALLBACK_DEFINE(0, game_input, &game_state);

static void engine_pf_input_hold(void)
{
	if (game_state.input.mov_up) {
		game_input_dispatch(INPUT_MOV_UP_H);
	}
	if (game_state.input.mov_down) {
		game_input_dispatch(INPUT_MOV_DOWN_H);
	}
	if (game_state.input.mov_left) {
		game_input_dispatch(INPUT_MOV_LEFT_H);
	}
	if (game_state.input.mov_right) {
		game_input_dispatch(INPUT_MOV_RIGHT_H);
	}
	if (game_state.input.cam_up) {
		game_input_dispatch(INPUT_CAM_UP_H);
	}
	if (game_state.input.cam_down) {
		game_input_dispatch(INPUT_CAM_DOWN_H);
	}
	if (game_state.input.cam_left) {
		game_input_dispatch(INPUT_CAM_LEFT_H);
	}
	if (game_state.input.cam_right) {
		game_input_dispatch(INPUT_CAM_RIGHT_H);
	}
}

void engine_pf(void)
{
	if (game_state.uptime == 0) {
		engine_init();
	}
	if (sys_timepoint_expired(logo_timeout) && engine_getscene() == &logo_scene)
	{
		engine_cleanscene();
	}

	engine_pf_input_hold();

	game_state.uptime = k_uptime_get();
}

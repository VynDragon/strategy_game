#pragma once

typedef enum {
	LOGO = 0,
	MAIN_MENU = 1,

} Strategy_Context;


void context_engine_pf(void);

Strategy_Context get_strategy_context(void);

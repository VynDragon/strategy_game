#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app_context);

#include "engine.h"
#include "context.h"
#include "scenes.h"

static Strategy_Context current_context = LOGO;
static uint32_t ticks = 0;
static k_timepoint_t logo_timeout;

static void context_engine_pf_init(void)
{
	logo_timeout = sys_timepoint_calc(K_SECONDS(1));
}

void context_engine_pf(void)
{
	if (ticks == 0) {
		context_engine_pf_init();
	}
	if (sys_timepoint_expired(logo_timeout) && current_context == LOGO)
	{
		current_context = MAIN_MENU;
		engine_switchscene(&strategy_menu_scene);
	}
	ticks++;
}

Strategy_Context get_strategy_context(void)
{
	return current_context;
}


#include <lvgl.h>

lv_style_t ui_style;

void init_ui_style(void) {

	lv_style_init(&ui_style);
	//lv_style_set_bg_opa(&ui_style, LV_OPA_TRANSP);
	lv_style_set_text_color(&ui_style, lv_color_hex(0xffffff));
	lv_style_set_radius(&ui_style, 0);
	lv_style_set_border_width(&ui_style, 0);
	lv_style_set_border_color(&ui_style, lv_color_hex(0xffffff));
	lv_style_set_pad_all(&ui_style, 1);
	lv_style_set_text_line_space(&ui_style, 1);
}

#pragma once

#include <zephyr/input/input.h>

typedef struct configuration_s {
	uint16_t mov_up;
	uint16_t mov_down;
	uint16_t mov_left;
	uint16_t mov_right;
	uint16_t cam_up;
	uint16_t cam_down;
	uint16_t cam_left;
	uint16_t cam_right;
	uint16_t b0;
	uint16_t b1;
	uint16_t f1;
	uint16_t f2;
	uint16_t f3;
	uint16_t f4;
} configuration;

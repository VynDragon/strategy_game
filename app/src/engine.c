#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app_engine);

#include "engine.h"

static const struct device *display_device = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

int blit_display_L8(L3_COLORTYPE *buffer, uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y)
{
	struct display_buffer_descriptor buf_desc;
	buf_desc.buf_size = size_x * size_y;
	buf_desc.width = size_x;
	buf_desc.height = size_y;
	buf_desc.pitch = size_x;

	display_write(display_device, x, y, &buf_desc, buffer);

	return 0;
}

int blit_display_MONO(L3_COLORTYPE *buffer, uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y)
{
	struct display_buffer_descriptor buf_desc;
	uint8_t buf[128] = {0};

	if (size_y < 8 || (y & 0x7) != 0 || (size_y & 0x7) != 0) {
		LOG_ERR("Bad position or size");
	}

	buf_desc.buf_size = size_x;
	buf_desc.width = size_x;
	buf_desc.height = 8;
	buf_desc.pitch = size_x;

	for (int j = 0; j < size_y; j+= 8) {
		for (int i = 0; i < size_x; i++) {
			buf[i] = buffer[i + (j + 0) * size_x] > 128 ? buf[i] | 1<<0 : buf[i] & ~(1<<0);
			buf[i] = buffer[i + (j + 1) * size_x] > 128 ? buf[i] | 1<<1 : buf[i] & ~(1<<1);
			buf[i] = buffer[i + (j + 2) * size_x] > 128 ? buf[i] | 1<<2 : buf[i] & ~(1<<2);
			buf[i] = buffer[i + (j + 3) * size_x] > 128 ? buf[i] | 1<<3 : buf[i] & ~(1<<3);
			buf[i] = buffer[i + (j + 4) * size_x] > 128 ? buf[i] | 1<<4 : buf[i] & ~(1<<4);
			buf[i] = buffer[i + (j + 5) * size_x] > 128 ? buf[i] | 1<<5 : buf[i] & ~(1<<5);
			buf[i] = buffer[i + (j + 6) * size_x] > 128 ? buf[i] | 1<<6 : buf[i] & ~(1<<6);
			buf[i] = buffer[i + (j + 7) * size_x] > 128 ? buf[i] | 1<<7 : buf[i] & ~(1<<7);
		}
		display_write(display_device, x, j, &buf_desc, buf);
	}

	return 0;
}

int blit_display_MONO_dither(L3_COLORTYPE *buffer, uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y)
{
	struct display_buffer_descriptor buf_desc;
	uint8_t buf[128] = {0};
	int error = 0;

	if (size_y < 8 || (y & 0x7) != 0 || (size_y & 0x7) != 0) {
		LOG_ERR("Bad position or size");
	}

	buf_desc.buf_size = size_x;
	buf_desc.width = size_x;
	buf_desc.height = 8;
	buf_desc.pitch = size_x;

#define ERR_THRE 255
#define ERR_MUL 2

	for (int j = 0; j < size_y; j+= 8) {
		for (int i = 0; i < size_x; i++) {
			error += buffer[i + (j + 0) * size_x];
			if (error > ERR_THRE) {
				buf[i] |= 1 << 0;
				error -= ERR_THRE * ERR_MUL;
			} else {
				buf[i] &= ~(1<<0);
			}
			error += buffer[i + (j + 1) * size_x];
			if (error > ERR_THRE) {
				buf[i] |= 1 << 1;
				error -= ERR_THRE * ERR_MUL;
			} else {
				buf[i] &= ~(1<<1);
			}
			error += buffer[i + (j + 2) * size_x];
			if (error > ERR_THRE) {
				buf[i] |= 1 << 2;
				error -= ERR_THRE * ERR_MUL;
			} else {
				buf[i] &= ~(1<<2);
			}
			error += buffer[i + (j + 3) * size_x];
			if (error > ERR_THRE) {
				buf[i] |= 1 << 3;
				error -= ERR_THRE * ERR_MUL;
			} else {
				buf[i] &= ~(1<<3);
			}
			error += buffer[i + (j + 4) * size_x];
			if (error > ERR_THRE) {
				buf[i] |= 1 << 4;
				error -= ERR_THRE * ERR_MUL;
			} else {
				buf[i] &= ~(1<<4);
			}
			error += buffer[i + (j + 5) * size_x];
			if (error > ERR_THRE) {
				buf[i] |= 1 << 5;
				error -= ERR_THRE * ERR_MUL;
			} else {
				buf[i] &= ~(1<<5);
			}
			error += buffer[i + (j + 6) * size_x];
			if (error > ERR_THRE) {
				buf[i] |= 1 << 6;
				error -= ERR_THRE * ERR_MUL;
			} else {
				buf[i] &= ~(1<<6);
			}
			error += buffer[i + (j + 7) * size_x];
			if (error > ERR_THRE) {
				buf[i] |= 1 << 7;
				error -= ERR_THRE * ERR_MUL;
			} else {
				buf[i] &= ~(1<<7);
			}
			/* buf[i] = buffer[i + (j + 0) * size_x] > 85 ? buf[i] | 1<<0 : buf[i] & ~(1<<0);
			buf[i] = buffer[i + (j + 0) * size_x] > 170 ? buf[i] | 1<<1 : buf[i] & ~(1<<1);
			buf[i] = buffer[i + (j + 1) * size_x] > 85 ? buf[i] | 1<<2 : buf[i] & ~(1<<2);
			buf[i] = buffer[i + (j + 1) * size_x] > 170 ? buf[i] | 1<<3 : buf[i] & ~(1<<3);
			buf[i] = buffer[i + (j + 2) * size_x] > 85 ? buf[i] | 1<<4 : buf[i] & ~(1<<4);
			buf[i] = buffer[i + (j + 2) * size_x] > 170 ? buf[i] | 1<<5 : buf[i] & ~(1<<5);
			buf[i] = buffer[i + (j + 3) * size_x] > 85 ? buf[i] | 1<<6 : buf[i] & ~(1<<6);
			buf[i] = buffer[i + (j + 3) * size_x] > 170 ? buf[i] | 1<<7 : buf[i] & ~(1<<7); */
		}
		display_write(display_device, x, j, &buf_desc, buf);
	}

	return 0;
}

int blit_display_RGB565(L3_COLORTYPE *buffer, uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y)
{
	struct display_buffer_descriptor buf_desc;
	uint16_t buf[160] = {0};
	buf_desc.buf_size = size_x * 2;
	buf_desc.width = size_x;
	buf_desc.height = 1;
	buf_desc.pitch = size_x;

	for (int j = y; j < size_y; j+= 1) {
		for (int i = 0; i < size_x; i++) {
			buf[i] = (uint16_t)(buffer[i + j * size_x] & 0xF8);
		}
		display_write(display_device, x, j, &buf_desc, buf);
	}
	return 0;
}

int blit_display(L3_COLORTYPE *buffer, uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y)
{
	return blit_display_L8(buffer, x, y, size_x, size_y);
}

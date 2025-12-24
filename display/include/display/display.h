#ifndef SSD1306_H
#define SSD1306_H

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Размеры дисплея
#define SSD1306_WIDTH  128
#define SSD1306_HEIGHT 64
#define SSD1306_BUFFER_SIZE (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

// Адреса I2C
#define SSD1306_I2C_ADDR    0x3C
#define SSD1306_I2C_ADDR_ALT 0x3D

// Команды дисплея
#define SSD1306_SETCONTRAST          0x81
#define SSD1306_DISPLAYALLON_RESUME  0xA4
#define SSD1306_DISPLAYALLON         0xA5
#define SSD1306_NORMALDISPLAY        0xA6
#define SSD1306_INVERTDISPLAY        0xA7
#define SSD1306_DISPLAYOFF           0xAE
#define SSD1306_DISPLAYON            0xAF
#define SSD1306_SETDISPLAYOFFSET     0xD3
#define SSD1306_SETCOMPINS           0xDA
#define SSD1306_SETVCOMDETECT        0xDB
#define SSD1306_SETDISPLAYCLOCKDIV   0xD5
#define SSD1306_SETPRECHARGE         0xD9
#define SSD1306_SETMULTIPLEX         0xA8
#define SSD1306_SETLOWCOLUMN         0x00
#define SSD1306_SETHIGHCOLUMN        0x10
#define SSD1306_SETSTARTLINE         0x40
#define SSD1306_MEMORYMODE           0x20
#define SSD1306_COLUMNADDR           0x21
#define SSD1306_PAGEADDR             0x22
#define SSD1306_COMSCANINC           0xC0
#define SSD1306_COMSCANDEC           0xC8
#define SSD1306_SEGREMAP             0xA0
#define SSD1306_CHARGEPUMP           0x8D

// Структура дисплея
typedef struct {
    uint8_t width;
    uint8_t height;
    uint8_t pages;
    uint8_t address;
    i2c_inst_t *i2c_port;
    bool external_vcc;
    uint8_t buffer[SSD1306_BUFFER_SIZE];
} ssd1306_t;

// Инициализация
void ssd1306_init(ssd1306_t *ssd, uint8_t width, uint8_t height, uint8_t address, i2c_inst_t *i2c, bool external_vcc);

// Управление дисплеем
void ssd1306_display_on(ssd1306_t *ssd);
void ssd1306_display_off(ssd1306_t *ssd);
void ssd1306_invert(ssd1306_t *ssd, bool invert);
void ssd1306_contrast(ssd1306_t *ssd, uint8_t contrast);

// Буфер
void ssd1306_clear(ssd1306_t *ssd);
void ssd1306_clear_buffer(ssd1306_t *ssd);
void ssd1306_show(ssd1306_t *ssd);

// Рисование
void ssd1306_draw_pixel(ssd1306_t *ssd, uint8_t x, uint8_t y, bool color);
void ssd1306_draw_line(ssd1306_t *ssd, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool color);
void ssd1306_draw_hline(ssd1306_t *ssd, uint8_t x, uint8_t y, uint8_t w, bool color);
void ssd1306_draw_vline(ssd1306_t *ssd, uint8_t x, uint8_t y, uint8_t h, bool color);
void ssd1306_draw_rect(ssd1306_t *ssd, uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool color, bool fill);
void ssd1306_draw_circle(ssd1306_t *ssd, uint8_t x0, uint8_t y0, uint8_t r, bool color, bool fill);
void ssd1306_draw_triangle(ssd1306_t *ssd, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool color, bool fill);

// Текст
void ssd1306_draw_char(ssd1306_t *ssd, uint8_t x, uint8_t y, char c, uint8_t scale, bool color);
void ssd1306_draw_string(ssd1306_t *ssd, uint8_t x, uint8_t y, const char *str, uint8_t scale, bool color);

// Изображения
void ssd1306_draw_bitmap(ssd1306_t *ssd, uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, bool color);

// Прокрутка
void ssd1306_start_scroll_h(ssd1306_t *ssd, bool direction, uint8_t start, uint8_t end);
void ssd1306_start_scroll_vh(ssd1306_t *ssd, bool direction, uint8_t start, uint8_t end);
void ssd1306_stop_scroll(ssd1306_t *ssd);

// Утилиты
bool ssd1306_get_pixel(ssd1306_t *ssd, uint8_t x, uint8_t y);
void ssd1306_fill(ssd1306_t *ssd, bool color);
void ssd1306_update_rect(ssd1306_t *ssd, uint8_t x, uint8_t y, uint8_t w, uint8_t h);

#endif // SSD1306_H
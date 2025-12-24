#include "display/display.h"

// Шрифт 5x8 (базовый ASCII)
static const uint8_t font5x8[95][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // space
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // (
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // )
    {0x14, 0x08, 0x3E, 0x08, 0x14}, // *
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // /
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ;
    {0x08, 0x14, 0x22, 0x41, 0x00}, // <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // =
    {0x00, 0x41, 0x22, 0x14, 0x08}, // >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ?
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // @
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
    {0x00, 0x7F, 0x41, 0x41, 0x00}, // [
    {0x02, 0x04, 0x08, 0x10, 0x20}, // backslash
    {0x00, 0x41, 0x41, 0x7F, 0x00}, // ]
    {0x04, 0x02, 0x01, 0x02, 0x04}, // ^
    {0x40, 0x40, 0x40, 0x40, 0x40}, // _
    {0x00, 0x01, 0x02, 0x04, 0x00}, // `
    {0x20, 0x54, 0x54, 0x54, 0x78}, // a
    {0x7F, 0x48, 0x44, 0x44, 0x38}, // b
    {0x38, 0x44, 0x44, 0x44, 0x20}, // c
    {0x38, 0x44, 0x44, 0x48, 0x7F}, // d
    {0x38, 0x54, 0x54, 0x54, 0x18}, // e
    {0x08, 0x7E, 0x09, 0x01, 0x02}, // f
    {0x0C, 0x52, 0x52, 0x52, 0x3E}, // g
    {0x7F, 0x08, 0x04, 0x04, 0x78}, // h
    {0x00, 0x44, 0x7D, 0x40, 0x00}, // i
    {0x20, 0x40, 0x44, 0x3D, 0x00}, // j
    {0x7F, 0x10, 0x28, 0x44, 0x00}, // k
    {0x00, 0x41, 0x7F, 0x40, 0x00}, // l
    {0x7C, 0x04, 0x18, 0x04, 0x78}, // m
    {0x7C, 0x08, 0x04, 0x04, 0x78}, // n
    {0x38, 0x44, 0x44, 0x44, 0x38}, // o
    {0x7C, 0x14, 0x14, 0x14, 0x08}, // p
    {0x08, 0x14, 0x14, 0x18, 0x7C}, // q
    {0x7C, 0x08, 0x04, 0x04, 0x08}, // r
    {0x48, 0x54, 0x54, 0x54, 0x20}, // s
    {0x04, 0x3F, 0x44, 0x40, 0x20}, // t
    {0x3C, 0x40, 0x40, 0x20, 0x7C}, // u
    {0x1C, 0x20, 0x40, 0x20, 0x1C}, // v
    {0x3C, 0x40, 0x30, 0x40, 0x3C}, // w
    {0x44, 0x28, 0x10, 0x28, 0x44}, // x
    {0x0C, 0x50, 0x50, 0x50, 0x3C}, // y
    {0x44, 0x64, 0x54, 0x4C, 0x44}, // z
    {0x00, 0x08, 0x36, 0x41, 0x00}, // {
    {0x00, 0x00, 0x7F, 0x00, 0x00}, // |
    {0x00, 0x41, 0x36, 0x08, 0x00}, // }
    {0x10, 0x08, 0x08, 0x10, 0x08}, // ~
};

// Приватные функции
static void ssd1306_command(ssd1306_t *ssd, uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd}; // Co = 0, D/C = 0
    i2c_write_blocking(ssd->i2c_port, ssd->address, buf, 2, false);
}

static void ssd1306_command_list(ssd1306_t *ssd, const uint8_t *commands, uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        ssd1306_command(ssd, commands[i]);
    }
}

// Публичные функции

void ssd1306_init(ssd1306_t *ssd, uint8_t width, uint8_t height, uint8_t address, i2c_inst_t *i2c, bool external_vcc) {
    ssd->width = width;
    ssd->height = height;
    ssd->pages = height / 8;
    ssd->address = address;
    ssd->i2c_port = i2c;
    ssd->external_vcc = external_vcc;
    
    // Очищаем буфер
    memset(ssd->buffer, 0, sizeof(ssd->buffer));
    
    // Последовательность команд инициализации
    const uint8_t init_commands[] = {
        SSD1306_DISPLAYOFF,
        SSD1306_SETDISPLAYCLOCKDIV, 0x80,
        SSD1306_SETMULTIPLEX, height - 1,
        SSD1306_SETDISPLAYOFFSET, 0x00,
        SSD1306_SETSTARTLINE | 0x00,
        SSD1306_CHARGEPUMP, external_vcc ? 0x10 : 0x14,
        SSD1306_MEMORYMODE, 0x00,
        SSD1306_SEGREMAP | 0x01,
        SSD1306_COMSCANDEC,
        SSD1306_SETCOMPINS, (height == 64) ? 0x12 : 0x02,
        SSD1306_SETCONTRAST, external_vcc ? 0x9F : 0xCF,
        SSD1306_SETPRECHARGE, external_vcc ? 0x22 : 0xF1,
        SSD1306_SETVCOMDETECT, 0x40,
        SSD1306_DISPLAYALLON_RESUME,
        SSD1306_NORMALDISPLAY,
        SSD1306_DISPLAYON
    };
    
    ssd1306_command_list(ssd, init_commands, sizeof(init_commands));
    sleep_ms(100);
}

void ssd1306_display_on(ssd1306_t *ssd) {
    ssd1306_command(ssd, SSD1306_DISPLAYON);
}

void ssd1306_display_off(ssd1306_t *ssd) {
    ssd1306_command(ssd, SSD1306_DISPLAYOFF);
}

void ssd1306_invert(ssd1306_t *ssd, bool invert) {
    ssd1306_command(ssd, invert ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY);
}

void ssd1306_contrast(ssd1306_t *ssd, uint8_t contrast) {
    ssd1306_command(ssd, SSD1306_SETCONTRAST);
    ssd1306_command(ssd, contrast);
}

void ssd1306_clear(ssd1306_t *ssd) {
    ssd1306_clear_buffer(ssd);
    ssd1306_show(ssd);
}

void ssd1306_clear_buffer(ssd1306_t *ssd) {
    memset(ssd->buffer, 0, sizeof(ssd->buffer));
}

void ssd1306_show(ssd1306_t *ssd) {
    const uint8_t commands[] = {
        SSD1306_COLUMNADDR, 0, ssd->width - 1,
        SSD1306_PAGEADDR, 0, ssd->pages - 1
    };
    
    ssd1306_command_list(ssd, commands, sizeof(commands));
    
    // Отправляем данные
    uint8_t *ptr = ssd->buffer;
    for (uint8_t i = 0; i < ssd->pages; i++) {
        uint8_t data[ssd->width + 1];
        data[0] = 0x40; // Co = 0, D/C = 1
        
        for (uint8_t x = 0; x < ssd->width; x++) {
            data[x + 1] = *ptr++;
        }
        
        i2c_write_blocking(ssd->i2c_port, ssd->address, data, ssd->width + 1, false);
    }
}

void ssd1306_draw_pixel(ssd1306_t *ssd, uint8_t x, uint8_t y, bool color) {
    if (x >= ssd->width || y >= ssd->height) return;
    
    uint16_t index = x + (y / 8) * ssd->width;
    if (color) {
        ssd->buffer[index] |= (1 << (y & 7));
    } else {
        ssd->buffer[index] &= ~(1 << (y & 7));
    }
}

bool ssd1306_get_pixel(ssd1306_t *ssd, uint8_t x, uint8_t y) {
    if (x >= ssd->width || y >= ssd->height) return false;
    
    uint16_t index = x + (y / 8) * ssd->width;
    return (ssd->buffer[index] >> (y & 7)) & 1;
}

void ssd1306_draw_line(ssd1306_t *ssd, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool color) {
    int16_t dx = abs(x1 - x0);
    int16_t dy = abs(y1 - y0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;
    
    while (1) {
        ssd1306_draw_pixel(ssd, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int16_t e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void ssd1306_draw_hline(ssd1306_t *ssd, uint8_t x, uint8_t y, uint8_t w, bool color) {
    for (uint8_t i = 0; i < w; i++) {
        ssd1306_draw_pixel(ssd, x + i, y, color);
    }
}

void ssd1306_draw_vline(ssd1306_t *ssd, uint8_t x, uint8_t y, uint8_t h, bool color) {
    for (uint8_t i = 0; i < h; i++) {
        ssd1306_draw_pixel(ssd, x, y + i, color);
    }
}

void ssd1306_draw_rect(ssd1306_t *ssd, uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool color, bool fill) {
    if (fill) {
        for (uint8_t i = 0; i < h; i++) {
            ssd1306_draw_hline(ssd, x, y + i, w, color);
        }
    } else {
        ssd1306_draw_hline(ssd, x, y, w, color);
        ssd1306_draw_hline(ssd, x, y + h - 1, w, color);
        ssd1306_draw_vline(ssd, x, y, h, color);
        ssd1306_draw_vline(ssd, x + w - 1, y, h, color);
    }
}

void ssd1306_draw_circle(ssd1306_t *ssd, uint8_t x0, uint8_t y0, uint8_t r, bool color, bool fill) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    
    if (fill) {
        ssd1306_draw_vline(ssd, x0, y0 - r, 2 * r + 1, color);
    } else {
        ssd1306_draw_pixel(ssd, x0, y0 + r, color);
        ssd1306_draw_pixel(ssd, x0, y0 - r, color);
        ssd1306_draw_pixel(ssd, x0 + r, y0, color);
        ssd1306_draw_pixel(ssd, x0 - r, y0, color);
    }
    
    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        
        if (fill) {
            ssd1306_draw_vline(ssd, x0 + x, y0 - y, 2 * y + 1, color);
            ssd1306_draw_vline(ssd, x0 - x, y0 - y, 2 * y + 1, color);
            ssd1306_draw_vline(ssd, x0 + y, y0 - x, 2 * x + 1, color);
            ssd1306_draw_vline(ssd, x0 - y, y0 - x, 2 * x + 1, color);
        } else {
            ssd1306_draw_pixel(ssd, x0 + x, y0 + y, color);
            ssd1306_draw_pixel(ssd, x0 - x, y0 + y, color);
            ssd1306_draw_pixel(ssd, x0 + x, y0 - y, color);
            ssd1306_draw_pixel(ssd, x0 - x, y0 - y, color);
            ssd1306_draw_pixel(ssd, x0 + y, y0 + x, color);
            ssd1306_draw_pixel(ssd, x0 - y, y0 + x, color);
            ssd1306_draw_pixel(ssd, x0 + y, y0 - x, color);
            ssd1306_draw_pixel(ssd, x0 - y, y0 - x, color);
        }
    }
}

void ssd1306_draw_triangle(ssd1306_t *ssd, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool color, bool fill) {
    if (fill) {
        // Простая заливка (для небольших треугольников)
        ssd1306_draw_line(ssd, x0, y0, x1, y1, color);
        ssd1306_draw_line(ssd, x1, y1, x2, y2, color);
        ssd1306_draw_line(ssd, x2, y2, x0, y0, color);
        
        // Заполнение горизонтальными линиями
        int16_t min_y = y0, max_y = y0;
        if (y1 < min_y) min_y = y1;
        if (y2 < min_y) min_y = y2;
        if (y1 > max_y) max_y = y1;
        if (y2 > max_y) max_y = y2;
        
        for (int16_t y = min_y; y <= max_y; y++) {
            int16_t x_start = ssd->width, x_end = 0;
            
            // Проверяем каждую сторону
            for (uint8_t i = 0; i < 3; i++) {
                uint8_t *px1, *py1, *px2, *py2;
                if (i == 0) { px1 = &x0; py1 = &y0; px2 = &x1; py2 = &y1; }
                else if (i == 1) { px1 = &x1; py1 = &y1; px2 = &x2; py2 = &y2; }
                else { px1 = &x2; py1 = &y2; px2 = &x0; py2 = &y0; }
                
                if ((*py1 <= y && *py2 >= y) || (*py1 >= y && *py2 <= y)) {
                    if (*py1 != *py2) {
                        int16_t x = *px1 + (y - *py1) * (*px2 - *px1) / (*py2 - *py1);
                        if (x < x_start) x_start = x;
                        if (x > x_end) x_end = x;
                    }
                }
            }
            
            if (x_start <= x_end) {
                ssd1306_draw_hline(ssd, x_start, y, x_end - x_start + 1, color);
            }
        }
    } else {
        ssd1306_draw_line(ssd, x0, y0, x1, y1, color);
        ssd1306_draw_line(ssd, x1, y1, x2, y2, color);
        ssd1306_draw_line(ssd, x2, y2, x0, y0, color);
    }
}

void ssd1306_draw_char(ssd1306_t *ssd, uint8_t x, uint8_t y, char c, uint8_t scale, bool color) {
    if (c < 32 || c > 126) return; // Только печатные символы
    
    const uint8_t *bitmap = font5x8[c - 32];
    
    for (uint8_t col = 0; col < 5; col++) {
        uint8_t line = bitmap[col];
        for (uint8_t row = 0; row < 8; row++) {
            if (line & 0x1) {
                if (scale == 1) {
                    ssd1306_draw_pixel(ssd, x + col, y + row, color);
                } else {
                    ssd1306_draw_rect(ssd, x + col * scale, y + row * scale, scale, scale, color, true);
                }
            }
            line >>= 1;
        }
    }
}

void ssd1306_draw_string(ssd1306_t *ssd, uint8_t x, uint8_t y, const char *str, uint8_t scale, bool color) {
    uint8_t cursor_x = x;
    uint8_t cursor_y = y;
    
    while (*str) {
        if (*str == '\n') {
            cursor_x = x;
            cursor_y += 8 * scale;
        } else {
            ssd1306_draw_char(ssd, cursor_x, cursor_y, *str, scale, color);
            cursor_x += 6 * scale;
        }
        str++;
    }
}

void ssd1306_draw_bitmap(ssd1306_t *ssd, uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, bool color) {
    for (uint8_t j = 0; j < h; j++) {
        for (uint8_t i = 0; i < w; i++) {
            if (bitmap[i + (j / 8) * w] & (1 << (j & 7))) {
                ssd1306_draw_pixel(ssd, x + i, y + j, color);
            }
        }
    }
}

void ssd1306_start_scroll_h(ssd1306_t *ssd, bool direction, uint8_t start, uint8_t end) {
    ssd1306_command(ssd, direction ? 0x27 : 0x26);
    ssd1306_command(ssd, 0x00);
    ssd1306_command(ssd, start);
    ssd1306_command(ssd, 0x00);
    ssd1306_command(ssd, end);
    ssd1306_command(ssd, 0x00);
    ssd1306_command(ssd, 0xFF);
}

void ssd1306_start_scroll_vh(ssd1306_t *ssd, bool direction, uint8_t start, uint8_t end) {
    ssd1306_command(ssd, direction ? 0x2A : 0x29);
    ssd1306_command(ssd, 0x00);
    ssd1306_command(ssd, start);
    ssd1306_command(ssd, 0x00);
    ssd1306_command(ssd, end);
    ssd1306_command(ssd, 0x01);
}

void ssd1306_stop_scroll(ssd1306_t *ssd) {
    ssd1306_command(ssd, 0x2E);
}

void ssd1306_fill(ssd1306_t *ssd, bool color) {
    memset(ssd->buffer, color ? 0xFF : 0x00, sizeof(ssd->buffer));
}

void ssd1306_update_rect(ssd1306_t *ssd, uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    // Упрощенная версия - обновляем весь дисплей
    ssd1306_show(ssd);
}
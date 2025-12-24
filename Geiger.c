#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "pico/multicore.h"
#include <stdio.h>

#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5
#define OLED_ADDR 0x3C
#define BUTTON_PIN 2
#define SIGNAL_PIN 19
#define COUNT 0

// Глобальные переменные (используем volatile для shared memory)
volatile bool timer_running = false;
volatile uint32_t elapsed_seconds = 0;
volatile uint32_t timer_start = 0;
volatile uint32_t signal_counter = 0;
volatile bool display_update_needed = false;
volatile bool counter_update_needed = false;
volatile uint32_t last_display_update = 0;

// Флаги для синхронизации между ядрами
volatile bool timer_update_flag = false;
volatile bool signal_detected = false;

// Функции для OLED (оставляем без изменений)
void oled_cmd(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, 2, false);
}

void oled_init() {
    sleep_ms(100);
    oled_cmd(0xAE);
    oled_cmd(0x8D); oled_cmd(0x14);
    oled_cmd(0xA8); oled_cmd(0x3F);
    oled_cmd(0xAF);
    sleep_ms(100);
}

void clear_area(uint8_t start_col, uint8_t end_col, uint8_t start_page, uint8_t end_page) {
    for (uint8_t page = start_page; page <= end_page; page++) {
        oled_cmd(0x21); oled_cmd(start_col); oled_cmd(end_col);
        oled_cmd(0x22); oled_cmd(page); oled_cmd(page);
        
        for (uint8_t col = start_col; col <= end_col; col++) {
            uint8_t buf[2] = {0x40, 0x00};
            i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, 2, false);
        }
    }
}

void draw_digit(uint8_t digit, uint8_t x, uint8_t page_y) {
    uint8_t patterns[11][24] = {
        {0xFF,0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,0xFF,
         0xFF,0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFF,0xFF},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF},
        {0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0xFF,0xFF,
         0xFF,0xFF,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81},
        {0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0xFF,0xFF,
         0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0xFF,0xFF},
        {0xFF,0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFF,0xFF,
         0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,0xFF},
        {0xFF,0xFF,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,
         0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0xFF,0xFF},
        {0xFF,0xFF,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,
         0xFF,0xFF,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0xFF,0xFF},
        {0xFF,0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,0xFF,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF},
        {0xFF,0xFF,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0xFF,0xFF,
         0xFF,0xFF,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0xFF,0xFF},
        {0xFF,0xFF,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0xFF,0xFF,
         0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0xFF,0xFF},
        {0x00,0x00,0x24,0x24,0x00,0x00,0x00,0x00,0x24,0x24,0x00,0x00,
         0x00,0x00,0x24,0x24,0x00,0x00,0x00,0x00,0x24,0x24,0x00,0x00}
    };
    
    oled_cmd(0x21); oled_cmd(x); oled_cmd(x + 11);
    oled_cmd(0x22); oled_cmd(page_y); oled_cmd(page_y);
    for (int col = 0; col < 12; col++) {
        uint8_t buf[2] = {0x40, patterns[digit][col]};
        i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, 2, false);
    }
    
    oled_cmd(0x22); oled_cmd(page_y + 1); oled_cmd(page_y + 1);
    for (int col = 0; col < 12; col++) {
        uint8_t buf[2] = {0x40, patterns[digit][col + 12]};
        i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, 2, false);
    }
}

// ОТДЕЛЬНАЯ ФУНКЦИЯ ДЛЯ БЫСТРОГО ОБНОВЛЕНИЯ СЧЕТЧИКА
void fast_update_counter() {
    // Только минимально необходимые операции
    uint32_t counter = signal_counter;
    if (counter > 9999) counter = 9999;
    
    // Позиции для 4 цифр
    uint8_t x_positions[4] = {20, 36, 52, 68};
    
    // Вычисляем цифры
    uint8_t digits[4];
    digits[0] = counter / 1000;
    digits[1] = (counter % 1000) / 100;
    digits[2] = (counter % 100) / 10;
    digits[3] = counter % 10;
    
    // Определяем с какой цифры начинать (пропускаем ведущие нули)
    uint8_t start_pos = 0;
    if (counter < 1000) start_pos = 1;
    if (counter < 100) start_pos = 2;
    if (counter < 10) start_pos = 3;
    
    // Очищаем только область цифр (быстро)
    clear_area(20, 79, 4, 5);
    
    // Рисуем только видимые цифры
    for (int i = start_pos; i < 4; i++) {
        draw_digit(digits[i], x_positions[i], 4);
    }
    
    // Если счетчик = 0, показываем одну цифру
    if (counter == 0) {
        draw_digit(0, 48, 4);
    }
}

void update_timer_display() {
    clear_area(32, 79, 0, 1);
    
    uint32_t seconds = elapsed_seconds;
    if (timer_running) {
        seconds += (time_us_32() / 1000000) - timer_start;
    }
    if (seconds == 10) {
        printf("%4d\n", signal_counter);
        seconds = 0;
        signal_counter = 0;
        timer_start += 10;
    }
    
    if (seconds > 999) seconds = 999;
    
    uint8_t digit1 = seconds / 100;
    uint8_t digit2 = (seconds % 100) / 10;
    uint8_t digit3 = seconds % 10;
    
    draw_digit(digit1, 32, 0);
    draw_digit(digit2, 48, 0);
    draw_digit(digit3, 64, 0);
    
    // Подпись "SEC" (упрощенная версия)
    oled_cmd(0x21); oled_cmd(90); oled_cmd(110);
    oled_cmd(0x22); oled_cmd(1); oled_cmd(1);
    uint8_t sec_text[21] = {0x66,0xCF,0x89,0x89,0xDB,0x72,0x00,
                            0x78,0xDC,0x94,0x94,0xDC,0x58,0x00,
                            0x38,0xCC,0x84,0x84,0xCC,0x48,0x00};
    for (int i = 0; i < 21; i++) {
        uint8_t buf[2] = {0x40, sec_text[i]};
        i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, 2, false);
    }
}

void update_full_display() {
    // Полное обновление всего дисплея
    update_timer_display();
    fast_update_counter();
    
    // Обновляем подписи полностью
    // Подпись "CNT"
    oled_cmd(0x21); oled_cmd(90); oled_cmd(110);
    oled_cmd(0x22); oled_cmd(5); oled_cmd(5);
    uint8_t cnt_text[21] = {0xFF,0x11,0x11,0x11,0x1B,0x0E,0x00,
                            0x00,0x00,0x08,0xFC,0x88,0x00,0x00,
                            0x78,0xCC,0x84,0x84,0xCC,0x48,0x00};
    for (int i = 0; i < 21; i++) {
        uint8_t buf[2] = {0x40, cnt_text[i]};
        i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, 2, false);
    }
}

// Отображение состояния таймера
void update_status_display() {
    // Очищаем маленькую область для статуса
    clear_area(0, 15, 0, 1);
    
    if (timer_running) {
        // Зеленый треугольник "▶" (запущено)
        uint8_t run_symbol[2][8] = {
            {0x00,0x80,0xC0,0xE0,0xF0,0xE0,0xC0,0x80},  // Верх
            {0x00,0x00,0x01,0x03,0x07,0x03,0x01,0x00}   // Низ
        };
        
        for (int page = 0; page < 2; page++) {
            oled_cmd(0x21); oled_cmd(4); oled_cmd(11);
            oled_cmd(0x22); oled_cmd(page); oled_cmd(page);
            
            for (int i = 0; i < 8; i++) {
                uint8_t buf[2] = {0x40, run_symbol[page][i]};
                i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, 2, false);
            }
        }
    } else {
        // Красный квадрат "■" (остановлено)
        uint8_t stop_symbol[2][8] = {
            {0x00,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x00},  // Верх
            {0x00,0x07,0x07,0x07,0x07,0x07,0x07,0x00}   // Низ
        };
        
        for (int page = 0; page < 2; page++) {
            oled_cmd(0x21); oled_cmd(4); oled_cmd(11);
            oled_cmd(0x22); oled_cmd(page); oled_cmd(page);
            
            for (int i = 0; i < 8; i++) {
                uint8_t buf[2] = {0x40, stop_symbol[page][i]};
                i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, 2, false);
            }
        }
    }
}

// ПРЕРЫВАНИЕ ПО ФРОНТУ СИГНАЛА
void signal_irq_handler(uint gpio, uint32_t events) {
    // Фильтр дребезга - проверяем, что сигнал стабилен
    static uint32_t last_edge_time = 0;
    uint32_t now = time_us_32();
    
    if (now - last_edge_time > 1000) { // Дебаунс 1 мс
        last_edge_time = now;
        if (timer_running) {
            signal_counter++;
            if (signal_counter > 9999) signal_counter = 9999;
            signal_detected = true; // Флаг для основного цикла
        }
    }
}

// ТАЙМЕР ДЛЯ ОБНОВЛЕНИЯ ДИСПЛЕЯ (прерывание по таймеру)
bool timer_callback(struct repeating_timer *t) {
    if (timer_running) {
        timer_update_flag = true;
    }
    return true;
}

// ФУНКЦИЯ ДЛЯ ВТОРОГО ЯДРА - ОПРОС КНОПКИ
void core1_entry() {
    static uint32_t press_start = 0;
    static bool was_pressed = false;
    static bool long_press_detected = false;
    
    while (true) {
        bool is_pressed = gpio_get(BUTTON_PIN);
        uint32_t now = time_us_32();
        
        if (is_pressed && !was_pressed) {
            press_start = now;
            long_press_detected = false;
            was_pressed = true;
            gpio_put(25, 1);
        }
        else if (!is_pressed && was_pressed) {
            was_pressed = false;
            gpio_put(25, 0);
            
            if (!long_press_detected) {
                // Короткое нажатие
                if (timer_running) {
                    elapsed_seconds += (now / 1000000) - timer_start;
                    timer_running = false;
                } else {
                    timer_running = true;
                    timer_start = now / 1000000;
                }
                display_update_needed = true;
            }
        }
        else if (is_pressed && was_pressed) {
            if (!long_press_detected && (now - press_start > 2000000)) {
                long_press_detected = true;
                
                timer_running = false;
                elapsed_seconds = 0;
                signal_counter = 0;
                display_update_needed = true;
                
                for (int i = 0; i < 3; i++) {
                    gpio_put(25, 1);
                    sleep_ms(100);
                    gpio_put(25, 0);
                    sleep_ms(100);
                }
            }
        }
        
        tight_loop_contents(); // Энергоэффективный цикл
    }
}

int main() {
    stdio_init_all();
    // Инициализация I2C
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    // Кнопка управления
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_PIN);
    
    // Сигнальный вход с ПРЕРЫВАНИЕМ
    gpio_init(SIGNAL_PIN);
    gpio_set_dir(SIGNAL_PIN, GPIO_IN);
    gpio_pull_down(SIGNAL_PIN);
    
    // Настройка прерывания по фронту сигнала
    gpio_set_irq_enabled_with_callback(SIGNAL_PIN, GPIO_IRQ_EDGE_RISE, 
                                       true, signal_irq_handler);
    
    // Светодиод
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    
    // OLED инициализация
    oled_init();
    clear_area(0, 127, 0, 7);
    
    // Разделительная линия
    oled_cmd(0x21); oled_cmd(0); oled_cmd(127);
    oled_cmd(0x22); oled_cmd(3); oled_cmd(3);
    for (int col = 0; col < 128; col++) {
        uint8_t buf[2] = {0x40, 0x18};
        i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, 2, false);
    }
    
    // Инициализируем дисплей
    update_full_display();
    
    // Запускаем второе ядро для опроса кнопки
    multicore_launch_core1(core1_entry);
    
    // Настройка таймера для обновления дисплея (250 мс)
    struct repeating_timer timer;
    add_repeating_timer_ms(250, timer_callback, NULL, &timer);
    
    uint32_t last_counter = 0;
    
    // ОСНОВНОЙ ЦИКЛ (ядро 0)
    while (1) {
        // Проверяем флаги и обновляем дисплей по необходимости
        if (timer_update_flag) {
            timer_update_flag = false;
            update_timer_display();
        }
        
        if (signal_detected) {
            signal_detected = false;
            fast_update_counter(); // Быстрое обновление только счетчика
            gpio_put(25, 1);
            sleep_ms(10);
            gpio_put(25, 0);
        }
        
        if (display_update_needed) {
            display_update_needed = false;
            update_full_display();
        }
        
        // Если счетчик изменился извне (например, сброс)
        if (last_counter != signal_counter) {
            last_counter = signal_counter;
            fast_update_counter();
        }
        
        // Энергоэффективное ожидание
        __wfi(); // Wait For Interrupt - процессор спит до прерывания
    }
    
    return 0;
}
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <font_ubuntu_mono_24.h>
#include <st7789_stm32_spi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

extern void snake_game();
extern void delay_ms(uint32_t ms);
extern bool is_right_pressed();
extern bool is_left_pressed();
extern bool is_up_pressed();
extern bool is_down_pressed();

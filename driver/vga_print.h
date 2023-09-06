#ifndef KERNEL_KPRINT_H
#define KERNEL_KPRINT_H

#include "../driver/port.h"

#define SCREEN_WIDE  80
#define SCREEN_HIGHT 25

#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

#define TEXT_BLACK         0
#define TEXT_BLUE          1
#define TEXT_GREEN         2
#define TEXT_CYAN          3
#define TEXT_RED           4
#define TEXT_MAGENTA       5
#define TEXT_BROWN         6
#define TEXT_LIGHT_GRAY    7
#define TEXT_DARK_GRAY     8
#define TEXT_LIGHT_BLUE    9
#define TEXT_LIGHT_GREEN   10
#define TEXT_LIGHT_CYAN    11
#define TEXT_LIGHT_RED     12
#define TEXT_LIGHT_PURPLE  13
#define TEXT_YELLOW        14
#define TEXT_WHITE         15

#define vga_video_memory (char*) 0xb8000

void vga_print_init();

void vga_screen_clear();

int vga_print(const char* str, int color);

void vga_print_at(const char c, int fore_color, int back_color, int x, int y);

#endif  // KERNEL_KPRINT_H

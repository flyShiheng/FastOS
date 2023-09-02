
#include "../driver/kprint.h"

int get_cursor_offset();
void set_cursor_offset(int offset);

void kprint_init() {
    port_byte_out(REG_SCREEN_CTRL, 10);
    port_byte_out(REG_SCREEN_DATA, 0);
}

int kprint(const char* str, int color) {
    int i = 0;
    int offset = get_cursor_offset() / 2;
    int global_x = offset % SCREEN_WIDE;
    int global_y = offset / SCREEN_WIDE;
    while(str[i] != 0) {
        kprint_at(str[i++], color, TEXT_BLUE, global_x++, global_y);
    }
    global_y++;
    if (global_y == SCREEN_HIGHT) {
        screen_clear();
    }
    return 0;
}

void screen_clear() {
    int screen_size = SCREEN_WIDE * SCREEN_HIGHT;
    int i;
    char *screen = vga_video_memory;

    for (i = 0; i < screen_size; i++) {
        screen[i*2] = ' ';
        screen[i*2+1] = (TEXT_BLUE & 0x0F) | (TEXT_BLUE << 4);
    }
    set_cursor_offset(0);
}

void kprint_at(const char c, int fore_color, int back_color, int x, int y) {
    char *video_memory = vga_video_memory;
    char color = (fore_color & 0x0F) | (back_color << 4);
    int offset = 2 * (x + y * SCREEN_WIDE);
    if (c == '\n') {
        offset += 2 * (SCREEN_WIDE - x);
    } else {
        video_memory[offset] = c;
        video_memory[offset + 1] = color;
        offset += 2;
    }
    set_cursor_offset(offset);
}

int get_cursor_offset() {
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8; /* High byte: << 8 */
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    return offset * 2;
}

void set_cursor_offset(int offset) {
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (uint8_t)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (uint8_t)(offset & 0xff));
}

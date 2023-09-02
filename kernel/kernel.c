
#define SCREEN_WIDE  80
#define SCREEN_HIGHT 25

#define vga_video_memory (char*) 0xb8000

int global_x = 16;
int global_y = 0;

int kprint(const char* str, int color) {
    int i = 0;
    while(str[i] != 0) {
        kprint_at(str[i++], color, 0, global_x++, global_y);
        if (global_x == SCREEN_WIDE) {
            global_x = 0;
            global_y++;
        }
    }
    return 0;
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
}

void kernel_main() {
    kprint("Hello FastOS", 14);
}

#include "../driver/kprint.h"

void kernel_main() {
    // kprint_init();
    screen_clear();
    kprint("Hello FastOS", TEXT_YELLOW);
}

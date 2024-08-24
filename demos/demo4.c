#define HYDROGEN_TERM_WIN32_SRC
#include "../hydrogen_win32.h"

int main() {
    hydrogen_config_title = "Hello World!";
    hydrogen_config_blocking_input = true;

    hydrogen_init();

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            hydrogen_set_color(x, Normal, y, Normal);
            hydrogen_put_char(x, y, 'X');
        }
    }

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            hydrogen_set_color(x, Bright, y, Bright);
            hydrogen_put_char(x + 9, y, 'X');
        }
    }

    hydrogen_refresh();

    hydrogen_get_key();

    hydrogen_terminate();
}
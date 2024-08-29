#define HYDROGEN_TERM_WIN32_SRC
#include "../hydrogen_win32.h"

int main() {
    hydrogen_config_title = "Hello World!";
    hydrogen_config_blocking_input = true;

    hydrogen_init();

    hydrogen_clear();

    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 10; x++) {
            hydrogen_put_char(x, y, 'X');
        }
    }

    hydrogen_clear_area(1, 1, 8, 1);


    hydrogen_refresh();

    hydrogen_get_key();

    hydrogen_terminate();
}
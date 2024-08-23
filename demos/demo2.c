#include "../hydrogen_win32.h"

int main() {
    hydrogen_config_title = "Hello World!";
    hydrogen_config_blocking_input = true;

    hydrogen_init();

    hydrogen_set_color(Blue, Normal, Black, Normal);
    hydrogen_put_box(0, 0, 10, 10, true);

    hydrogen_set_color(Red, Normal, Black, Normal);
    hydrogen_put_box(11, 0, 10, 10, false);

    hydrogen_set_color(Green, Normal, Black, Normal);
    hydrogen_put_box(3, 5, 15, 3, true);

    hydrogen_refresh();

    hydrogen_get_key();

    hydrogen_terminate();
}
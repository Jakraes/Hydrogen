#include "../hydrogen_win32.h"

int main() {
    hydrogen_config_title = "Hello World!";
    hydrogen_config_blocking_input = true;

    hydrogen_init();

    hydrogen_clear();

    hydrogen_set_color(White, Normal, Black, Normal);
    hydrogen_put_box(0, 0, hydrogen_config_viewport_width, hydrogen_config_viewport_height, false);

    hydrogen_refresh();

    hydrogen_get_key();

    hydrogen_terminate();
}
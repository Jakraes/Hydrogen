#include "../hydrogen_win32.h"

int main() {
    hydrogen_config_title = "Hello World!";
    // hydrogen_config_blocking_input = True;
    hydrogen_config_blocking_input = False;

    hydrogen_init();

    int counter = 0;

    do {
        hydrogen_clear();

        Hydrogen_Byte key = hydrogen_get_key();

        hydrogen_put_str_fmt(0, 0, "Current key: %c", key);

        hydrogen_put_str_fmt(0, 1, "Counter: %d", counter++);

        if (key == 'Q') break;       

        hydrogen_refresh();
    } while (1);
    
    hydrogen_terminate();

    return 0;
}
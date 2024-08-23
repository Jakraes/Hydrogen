/*
MIT License

Copyright (c) 2024 João Carvalho

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef HYDROGEN_TERM_WIN32_H
#define HYDROGEN_TERM_WIN32_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>
#include <conio.h>

// -------------------------------------------------------- //
//                       Declarations                       //
// -------------------------------------------------------- //

// -- Types -- //

typedef enum Hydrogen_Color {
    Black,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White
} Hydrogen_Color;

typedef enum Hydrogen_Color_Mode {
    Normal = 0,
    Bright = 8
} Hydrogen_Color_Mode;


// -- Config -- //

uint8_t* hydrogen_config_title = "Hydrogen Terminal";
uint8_t hydrogen_config_viewport_width = 80;
uint8_t hydrogen_config_viewport_height = 25;
bool hydrogen_config_cursor_hidden = true; 
bool hydrogen_config_blocking_input = true;


// -- Internal -- //

HANDLE _hydrogen_console;
CHAR_INFO* _hydrogen_stdout_buffer;
Hydrogen_Color _hydrogen_current_fg_color;
Hydrogen_Color _hydrogen_current_bg_color;
Hydrogen_Color_Mode _hydrogen_current_fg_mode;
Hydrogen_Color_Mode _hydrogen_current_bg_mode;
uint8_t _hydrogen_current_key;
HANDLE _hydrogen_input_thread;


// -- Functions -- //

void hydrogen_init();
void hydrogen_terminate();

void hydrogen_refresh();
void hydrogen_clear();

void hydrogen_set_color(Hydrogen_Color fg, Hydrogen_Color_Mode fg_mode, Hydrogen_Color bg, Hydrogen_Color_Mode bg_mode);

void hydrogen_put_char(uint8_t x, uint8_t y, uint8_t ch);
void hydrogen_put_str(uint8_t x, uint8_t y, const uint8_t* str);
void hydrogen_put_str_fmt(uint8_t x, uint8_t y, const uint8_t* str, ...);
void hydrogen_put_box(uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool double_line);

DWORD WINAPI _hydrogen_input_thread_func(LPVOID param);
uint8_t hydrogen_get_key();


// -------------------------------------------------------- //
//                      Implementations                     //
// -------------------------------------------------------- //

/**
 * Initializes the Hydrogen terminal by setting up the console handle, allocating the stdout buffer, 
 * setting the initial foreground and background colors, setting the console title, and hiding the cursor.
 * 
 * @return None
 */
void hydrogen_init() {
    _hydrogen_console = GetStdHandle(STD_OUTPUT_HANDLE);
    
    _hydrogen_stdout_buffer = malloc(hydrogen_config_viewport_width * hydrogen_config_viewport_height * sizeof(CHAR_INFO));

    _hydrogen_current_fg_color = White;
    _hydrogen_current_bg_color = Black;
    _hydrogen_current_fg_mode = Normal;
    _hydrogen_current_bg_mode = Normal;
    _hydrogen_current_key = 0;

    SetConsoleTitle(hydrogen_config_title);

    if (hydrogen_config_cursor_hidden) {
        CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
        SetConsoleCursorInfo(_hydrogen_console, &cursor_info);
    }

    if (!hydrogen_config_blocking_input) {
        _hydrogen_input_thread = CreateThread(NULL, 0, _hydrogen_input_thread_func, NULL, 0, NULL);
    }

    hydrogen_clear();
}

/**
 * Terminates the Hydrogen terminal by freeing the stdout buffer.
 * 
 * @return None
 */
void hydrogen_terminate() {
    free(_hydrogen_stdout_buffer);

    if (!hydrogen_config_blocking_input) {
        TerminateThread(_hydrogen_input_thread, 0);
        CloseHandle(_hydrogen_input_thread);
    }
}

/**
 * Refreshes the console by writing the contents of the stdout buffer to the console.
 * 
 * @return None
 */
void hydrogen_refresh() {
    COORD coord = { 0, 0 };
    SMALL_RECT rect = { 0, 0, hydrogen_config_viewport_height - 1, hydrogen_config_viewport_width - 1 };
    WriteConsoleOutput(_hydrogen_console, _hydrogen_stdout_buffer, (COORD) { hydrogen_config_viewport_height, hydrogen_config_viewport_width }, coord, &rect);
}

/**
 * Clears the console by setting all characters to null and resetting the 
 * foreground and background colors to their default values.
 *
 * @return None
 */
void hydrogen_clear() {
    for (int y = 0; y < hydrogen_config_viewport_height; y++) {
        for (int x = 0; x < hydrogen_config_viewport_width; x++) {
            _hydrogen_stdout_buffer[y * hydrogen_config_viewport_height + x].Char.AsciiChar = '\0';
            _hydrogen_stdout_buffer[y * hydrogen_config_viewport_height + x].Attributes = White | Normal | (Black << 4) | (Normal << 4);
        }
    }
}

/**
 * Sets the current foreground and background colors of the console.
 *
 * @param fg The foreground color to be set.
 * @param fg_mode The mode of the foreground color.
 * @param bg The background color to be set.
 * @param bg_mode The mode of the background color.
 *
 * @return None
 */
void hydrogen_set_color(Hydrogen_Color fg, Hydrogen_Color_Mode fg_mode, Hydrogen_Color bg, Hydrogen_Color_Mode bg_mode) {
    _hydrogen_current_fg_color = fg;
    _hydrogen_current_fg_mode = fg_mode;
    _hydrogen_current_bg_color = bg;
    _hydrogen_current_bg_mode = bg_mode;
}

/**
 * Writes a character to the console at the specified coordinates.
 *
 * @param ch The character to be written.
 * @param x The x-coordinate of the starting position.
 * @param y The y-coordinate of the starting position.
 *
 * @return None
 */
void hydrogen_put_char(uint8_t x, uint8_t y, uint8_t ch) {
    if (x >= hydrogen_config_viewport_width || y >= hydrogen_config_viewport_height) return;

    _hydrogen_stdout_buffer[y * hydrogen_config_viewport_height + x].Char.AsciiChar = ch;
    _hydrogen_stdout_buffer[y * hydrogen_config_viewport_height + x].Attributes = _hydrogen_current_fg_color | _hydrogen_current_fg_mode | (_hydrogen_current_bg_color << 4) | (_hydrogen_current_bg_mode << 4);
}

/**
 * Prints a string to the console at the specified coordinates.
 * 
 * @param str The string to be printed.
 * @param x The x-coordinate of the starting position.
 * @param y The y-coordinate of the starting position.
 * 
 * @return None
 */
void hydrogen_put_str(uint8_t x, uint8_t y, const uint8_t* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        hydrogen_put_char(x + i, y, str[i]);
    }
}

/**
 * Prints a formatted string to the console at the specified coordinates.
 * 
 * @param x The x-coordinate of the starting position.
 * @param y The y-coordinate of the starting position.
 * @param str The format string to be printed.
 * @param ... Variable number of arguments to be used in the format string.
 * 
 * @return None
 */
void hydrogen_put_str_fmt(uint8_t x, uint8_t y, const uint8_t* str, ...) {
    va_list args;
    va_start(args, str);
    char formatted_str[256];
    vsnprintf(formatted_str, 256, str, args);
    va_end(args);

    hydrogen_put_str(x, y, (const uint8_t*)formatted_str);
}

/**
 * Draws a box on the console at the specified coordinates.
 * 
 * @param x The x-coordinate of the top-left corner of the box.
 * @param y The y-coordinate of the top-left corner of the box.
 * @param width The width of the box.
 * @param height The height of the box.
 * @param double_line Whether to use double lines for the box border.
 * 
 * @return None
 */
void hydrogen_put_box(uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool double_line) {
    if (double_line) {
        for (int i = y; i < y + height; i++) {
            hydrogen_put_char(x, i, 186);
            hydrogen_put_char(x + width - 1, i, 186);
        }

        for (int i = x; i < x + width; i++) {
            hydrogen_put_char(i, y, 205);
            hydrogen_put_char(i, y + height - 1, 205);
        }

        hydrogen_put_char(x, y, 201);
        hydrogen_put_char(x + width - 1, y, 187);
        hydrogen_put_char(x, y + height - 1, 200);
        hydrogen_put_char(x + width - 1, y + height - 1, 188);
    } else {
        for (int i = y; i < y + height; i++) {
            hydrogen_put_char(x, i, 179);
            hydrogen_put_char(x + width - 1, i, 179);
        }

        for (int i = x; i < x + width; i++) {
            hydrogen_put_char(i, y, 196);
            hydrogen_put_char(i, y + height - 1, 196);
        }

        hydrogen_put_char(x, y, 218);
        hydrogen_put_char(x + width - 1, y, 191);
        hydrogen_put_char(x, y + height - 1, 192);
        hydrogen_put_char(x + width - 1, y + height - 1, 217);
    }
}


/**
 * The input thread function for Hydrogen.
 * 
 * This function is responsible for continuously reading input from the console and storing it in the _hydrogen_current_key variable.
 * 
 * @param param A pointer to a void object, currently unused.
 * 
 * @return The thread exit status, currently always 0.
 */
DWORD WINAPI _hydrogen_input_thread_func(LPVOID param) {
    while (1) {
        _hydrogen_current_key = _getch();
        Sleep(1);
    }

    return 0;
}

/**
 * Retrieves the current key from the console.
 *
 * @return The current key pressed by the user.
 */
uint8_t hydrogen_get_key() {
    if (hydrogen_config_blocking_input) {
        _hydrogen_current_key = _getch();
    }

    return _hydrogen_current_key;
}

#endif
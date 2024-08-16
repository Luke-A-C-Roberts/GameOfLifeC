#include "ldata.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <raylib.h>

#define SCREEN_WIDTH  1800
#define SCREEN_HEIGHT 1000
#define LAYOUT_TOP    100
#define SQUARE_SIZE   20
#define CELLS_WIDTH   SCREEN_WIDTH / SQUARE_SIZE
#define CELLS_HEIGHT  (SCREEN_HEIGHT - LAYOUT_TOP) / SQUARE_SIZE

void log_msg(cstr string) {
    printf("\033[32m%s\033[0m\n", string);
}

void log_err(cstr string) {
    printf("\033[31m%s\033[0m\n", string);
}

typedef struct {
    i32   x;
    i32   y;
    i32   w;
    i32   h;
    str   unpressed_message;
    str   pressed_message;
    bool  pressed;
    bool  show_pressed;
    Color unpressed_color;
    Color pressed_color;
    i32   pressed_timer;
} Button;

Button* init_button(
    ci32  x,
    ci32  y,
    ci32  w,
    ci32  h,
    cstr  upmessage,
    cstr  pmessage,
    const bool  pressed,
    const Color upcolor,
    const Color pcolor) {
    
    Button* button = malloc(sizeof(Button));

    str unpressed_message = malloc(strlen(upmessage) + 1);
    strcpy(unpressed_message, upmessage);

    str pressed_message = malloc(strlen(pmessage) + 1);
    strcpy(pressed_message, pmessage);

    *button = (Button){
        .x = x,
        .y = y,
        .w = w,
        .h = h,
        .unpressed_message = unpressed_message,
        .pressed_message   = pressed_message,
        .pressed           = pressed,
        .show_pressed      = pressed,
        .unpressed_color   = upcolor, 
        .pressed_color     = pcolor,
        .pressed_timer     = 0
    };
    
    return button;
}

Button* make_play_button(ci32 x, ci32 y) {
    return init_button(
        x, y, 100, 60, "paused", "playing", false, RED, GREEN);
}

Button* make_clear_button(ci32 x, ci32 y) {
    return init_button(
        x, y, 100, 60, "clear", "cleared!", false, GREEN, RED);
}

bool mouse_pressed_button(Button* button, i32 mouse_button) {
    i32 mouse_x = GetMouseX();
    i32 mouse_y = GetMouseY();
    
    return mouse_x >= button->x
        && mouse_y >= button->y
        && mouse_x <  button->x + button->w
        && mouse_y <  button->y + button->h
        && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void toggle_button_event(Button* button) {
    if (mouse_pressed_button(button, MOUSE_BUTTON_LEFT)) {
        button->pressed = !button->pressed;
        button->show_pressed = button->pressed;
    }
}

void press_button_event(Button* button) {
    if (!button->show_pressed
    &&  mouse_pressed_button(button, MOUSE_BUTTON_LEFT)) {
        button->pressed = true;
        button->show_pressed = true;
        return;
    }
    
    button->pressed = false;
    if (button->pressed_timer > 30) {
        button->show_pressed = false;
        button->pressed_timer = 0;
        return;
    }
    button->pressed_timer++;
    
}

void display_button(const Button* button) {
    if (button->show_pressed) {
        DrawRectangle(button->x, button->y, button->w, button->h, button->pressed_color);
        DrawText(button->pressed_message, button->x + 10, button->y + 10, 20, BLACK);
        return;
    }

    DrawRectangle(button->x, button->y, button->w, button->h, button->unpressed_color);
    DrawText(button->unpressed_message, button->x + 10, button->y + 10, 20, BLACK);
}

void free_button(Button* button) {
    free(button->unpressed_message);
    free(button->pressed_message);
    free(button);
}

void display_top(Color color) {
    DrawRectangle(0, 0, SCREEN_WIDTH, LAYOUT_TOP, color);
}

void display_background() {
    ClearBackground(BLACK);
    display_top(GRAY);
}

typedef struct {
    i32 x;
    i32 y;
    u8 state;
} Cell;

bool cell_state(Cell cell) {
    return (cell.state & 0x01) == 0x01;
}

Cell** init_cells() {
    Cell** cells = malloc(CELLS_HEIGHT * sizeof(Cell*));
    
    for (u64 y = 0; y < CELLS_HEIGHT; ++y) {
        cells[y] = malloc(CELLS_WIDTH * sizeof(Cell));

        for (u64 x = 0; x < CELLS_WIDTH; ++x)
            cells[y][x] = (Cell) {
                .x = (i32)x * SQUARE_SIZE,
                .y = (i32)y * SQUARE_SIZE + LAYOUT_TOP,
                .state = 0
            };
    }
    return cells;
}

void free_cells(Cell** cells) {
    for (u64 y = 0; y < CELLS_HEIGHT; ++y)
        free(cells[y]);
    free(cells);
}

void display_cell(Cell cell) {
    DrawRectangle(
        cell.x, cell.y,
        SQUARE_SIZE, SQUARE_SIZE,
        cell_state(cell)? WHITE : BLACK
    );
}

void display_cells(Cell** cells) {
    for (u64 y = 0; y < CELLS_HEIGHT; ++y) {
        for (u64 x = 0; x < CELLS_WIDTH; ++x)
            display_cell(cells[y][x]);
    }
}

u8 count_neighbors(const Cell** cells, cu64 x, cu64 y) {
    const bool check_left = x > 0;
    const bool check_right = x < CELLS_WIDTH - 1;
    const bool check_up = y > 0;
    const bool check_down = y < CELLS_HEIGHT - 1;
    
    u8 count = 0;
    
    if (check_left  && check_up   && cell_state(cells[y - 1][x - 1]))
        count++;
    if (check_right && check_up   && cell_state(cells[y - 1][x + 1]))
        count++;
    if (check_left  && check_down && cell_state(cells[y + 1][x - 1]))
        count++;
    if (check_right && check_down && cell_state(cells[y + 1][x + 1]))
        count++;
    if (check_left  && cell_state(cells[y][x - 1]))
        count++;
    if (check_right && cell_state(cells[y][x + 1]))
        count++;
    if (check_up    && cell_state(cells[y - 1][x]))
        count++;
    if (check_down  && cell_state(cells[y + 1][x]))
        count++;

    return count;
}

bool apply_rule(const bool cell_state, cu8 neighbor_count) {
    if (cell_state) {
        switch (neighbor_count) {
            case 2: return true;
            case 3: return true;
            default: return false;
        }
    }
    
    if (neighbor_count == 3)
        return true;

    return false;
}

void next_game_move(Cell** cells) {
    for (u64 y = 0; y < CELLS_HEIGHT; ++y) {
        for (u64 x = 0; x < CELLS_WIDTH; ++x) {
            cu8 neighbor_count = count_neighbors((const Cell**)cells, x, y);
            const bool state = cell_state(cells[y][x]);
            cells[y][x].state += apply_rule(state, neighbor_count)? 2 : 0;
        }
    }
    for (u64 y = 0; y < CELLS_HEIGHT; ++y) {
        for (u64 x = 0; x < CELLS_WIDTH; ++x)
            cells[y][x].state >>= 1;
    }
}

void mouse_coords(i64 *coords) {
    f64 mouse_x = (f64)GetMouseX();
    f64 mouse_y = (f64)GetMouseY() - LAYOUT_TOP;

    coords[0] = (i64)floor((mouse_x / SCREEN_WIDTH)  * CELLS_WIDTH);
    coords[1] = (i64)floor((mouse_y / (SCREEN_HEIGHT - LAYOUT_TOP)) * CELLS_HEIGHT);
}

bool valid_cell_coords(ci64* coords) {
    return coords[0] >= 0
        && coords[0] <  CELLS_WIDTH
        && coords[1] >= 0
        && coords[1] <  CELLS_HEIGHT;
}

bool toggle_cell_event(ci64* coords) {
    return valid_cell_coords(coords)
        && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void toggle_cell(Cell** cells, u64 x, u64 y) {
    cells[y][x].state = cell_state(cells[y][x])? 0 : 1;
}

i32 main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game of Life!");
    SetTargetFPS(60);

    Button* play_button = make_play_button(20, 20);
    Button* clear_button = make_clear_button(1680, 20);
    Cell** cells = init_cells();
    i64 coords[2];

    while(!WindowShouldClose()) {
        mouse_coords(coords);

        if (toggle_cell_event(coords))
            toggle_cell(cells, coords[0], coords[1]);

        BeginDrawing();
        
        display_background();
        toggle_button_event(play_button);
        press_button_event(clear_button); 
        display_button(play_button);
        display_button(clear_button);
        display_cells(cells);

        EndDrawing();
        
        if (play_button->pressed)
            next_game_move(cells);
        
        if (clear_button->pressed)
            cells = init_cells();
    }

    free_button(play_button);
    free_button(clear_button);
    free_cells(cells);

    return EXIT_SUCCESS; 
}

#include <stdio.h>
#include <raylib.h>

#define QUEUE_IMPLEMENTATION
#include "utils/queue.h"

#define FPS 60
#define SNAKE_BODY_WIDTH 24
#define BG_PRIMARY CLITERAL(Color){ 6, 13, 17, 255 }
#define BG_SECONDARY CLITERAL(Color){ 8, 18, 23, 255 }

#define SNAKE_GREEN CLITERAL(Color){ 106, 200, 89, 255 }
#define SNAKE_BORDER_COLOR BLACK

int get_food_snake_collision_body_part_index(Vector2 food, Vector2 *snake_body, int snake_body_count) {
    for(int i = 0; i < snake_body_count; i++) {
        if(snake_body[i].x == food.x && snake_body[i].y == food.y) {
            return i;
        }
    }
    return -1;
}

Vector2 generate_food(Vector2 *snake_body, int snake_body_count) {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    int rows = screen_height / SNAKE_BODY_WIDTH;
    int cols = screen_width / SNAKE_BODY_WIDTH;
    int row = GetRandomValue(0, rows-1);
    int col = GetRandomValue(0, cols-1);
    bool generated = false;
    do {
        if(get_food_snake_collision_body_part_index(
            (Vector2) {col, row},
            snake_body,
            snake_body_count
        ) != -1) {
            row = GetRandomValue(0, rows-1);
            col = GetRandomValue(0, cols-1);
        } else {
            generated = true;
        }
    } while(!generated);
    return (Vector2) {col, row};
}

void draw_snake(Vector2 *snake_body, int snake_body_count) {

    for(int i = 0; i < snake_body_count; i++) {
        Vector2 snake_body_part = snake_body[i];
        Color snake_color = SNAKE_GREEN;
        if(i == 0) {
            snake_color = WHITE;
        }
        DrawRectangle(
            SNAKE_BODY_WIDTH * (snake_body_part.x),
            SNAKE_BODY_WIDTH * (snake_body_part.y),
            SNAKE_BODY_WIDTH,
            SNAKE_BODY_WIDTH,
            snake_color
        );
        DrawRectangleLines(
            SNAKE_BODY_WIDTH * (snake_body_part.x),
            SNAKE_BODY_WIDTH * (snake_body_part.y),
            SNAKE_BODY_WIDTH,
            SNAKE_BODY_WIDTH,
            SNAKE_BORDER_COLOR
        );
    }
}

void draw_food(Vector2 food) {
    DrawRectangle(
        SNAKE_BODY_WIDTH * (food.x),
        SNAKE_BODY_WIDTH * (food.y),
        SNAKE_BODY_WIDTH,
        SNAKE_BODY_WIDTH,
        ORANGE
    );

    DrawRectangleLines(
        SNAKE_BODY_WIDTH * (food.x),
        SNAKE_BODY_WIDTH * (food.y),
        SNAKE_BODY_WIDTH,
        SNAKE_BODY_WIDTH,
        SNAKE_BORDER_COLOR
    );
}

void draw_striped_bg() {
    ClearBackground(BG_PRIMARY);
    int line_thickness = 2;
    int rows = GetScreenHeight() / SNAKE_BODY_WIDTH;
    for(int i = 0; i < rows; i++) {
        DrawLineEx(
            (Vector2) {0, i * SNAKE_BODY_WIDTH},
            (Vector2) {GetScreenWidth(), i * SNAKE_BODY_WIDTH},
            line_thickness,
            BG_SECONDARY
        );
    }
}
void draw_dense_striped_bg() {
    ClearBackground(BG_PRIMARY);
    int line_thickness = 3;
    int rows = GetScreenHeight() / SNAKE_BODY_WIDTH;
    for(int i = 0; i < 2 * rows; i++) {
        DrawLineEx(
            (Vector2) {0, i * SNAKE_BODY_WIDTH/2},
            (Vector2) {GetScreenWidth(), i * SNAKE_BODY_WIDTH/2},
            line_thickness,
            BG_SECONDARY
        );
    }
}

void draw_thick_striped_bg() {
    ClearBackground(BG_PRIMARY);
    int rows = GetScreenHeight() / SNAKE_BODY_WIDTH;
    for(int i = 0; i < rows; i++) {
        if(i % 2) {
            DrawRectangle(0, i * SNAKE_BODY_WIDTH, GetScreenWidth(), SNAKE_BODY_WIDTH, BG_SECONDARY);
        }
    }
}

void draw_semi_thick_striped_bg() {
    ClearBackground(BG_PRIMARY);
    int rows = GetScreenHeight() / SNAKE_BODY_WIDTH;
    for(int i = 0; i < 2 * rows; i++) {
        if(i % 2) {
            DrawRectangle(0, i * SNAKE_BODY_WIDTH/2, GetScreenWidth(), SNAKE_BODY_WIDTH/2, BG_SECONDARY);
        }
    }
}

void draw_dot_matrix_bg() {
    ClearBackground(BG_PRIMARY);
    int rows = GetScreenHeight() / SNAKE_BODY_WIDTH;
    int cols = GetScreenWidth() / SNAKE_BODY_WIDTH;
    int dot_thickness = 6;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            DrawRectangle(
                j * SNAKE_BODY_WIDTH - dot_thickness/2,
                i * SNAKE_BODY_WIDTH - dot_thickness/2,
                dot_thickness,
                dot_thickness,
                BG_SECONDARY
            );
        }
    }
}

void draw_plus_matrix_bg() {
    ClearBackground(BG_PRIMARY);
    int rows = GetScreenHeight() / SNAKE_BODY_WIDTH;
    int cols = GetScreenWidth() / SNAKE_BODY_WIDTH;
    int line_thickness = 2;
    int plus_thickness = 12;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            DrawLineEx(
                (Vector2) {j * SNAKE_BODY_WIDTH - plus_thickness/2, i * SNAKE_BODY_WIDTH},
                (Vector2) {j * SNAKE_BODY_WIDTH + plus_thickness/2, i * SNAKE_BODY_WIDTH},
                line_thickness,
                BG_SECONDARY
            );
            DrawLineEx(
                (Vector2) {j * SNAKE_BODY_WIDTH, i * SNAKE_BODY_WIDTH - plus_thickness/2},
                (Vector2) {j * SNAKE_BODY_WIDTH, i * SNAKE_BODY_WIDTH + plus_thickness/2},
                line_thickness,
                BG_SECONDARY
            );
        }
    }
}

void draw_grid_bg() {
    ClearBackground(BG_PRIMARY);
    int line_thickness = 2;
    int rows = GetScreenHeight() / SNAKE_BODY_WIDTH;
    int cols = GetScreenWidth() / SNAKE_BODY_WIDTH;
    for(int i = 0; i < rows; i++) {
        DrawLineEx(
            (Vector2) {0, i * SNAKE_BODY_WIDTH},
            (Vector2) {GetScreenWidth(), i * SNAKE_BODY_WIDTH},
            line_thickness,
            BG_SECONDARY
        );
    }
    for(int i = 0; i < cols; i++) {
        DrawLineEx(
            (Vector2) {i * SNAKE_BODY_WIDTH, 0},
            (Vector2) {i * SNAKE_BODY_WIDTH, GetScreenHeight(),},
            line_thickness,
            BG_SECONDARY
        );
    }
}


int main() {
    const int screen_width = 32 * SNAKE_BODY_WIDTH;
    const int screen_height = 32 * SNAKE_BODY_WIDTH;
    int rows = screen_height / SNAKE_BODY_WIDTH;
    int cols = screen_width / SNAKE_BODY_WIDTH;
    InitWindow(screen_width, screen_height, "Snake");
    SetTargetFPS(FPS);
    Vector2 snake_body[1000] = {
        (Vector2) {cols/2, rows/2},
        (Vector2) {cols/2 - 1, rows/2},
        (Vector2) {cols/2 - 2 , rows/2},
    };
    int snake_body_count = 3;
    Vector2 snake_direction = {1, 0};
    int frames_elapsed = 0;
    int snake_movement_frame_delay = 0.25 * FPS; // one second delay

    Vector2 food = generate_food(snake_body, snake_body_count);

    Queue *input_queue = queue_create(sizeof(Vector2), 5);
    while(!WindowShouldClose()) {

        if(IsKeyPressed(KEY_W)) {
            if(snake_direction.y != 1) {
                snake_direction = (Vector2){0, -1};
            }
        }

        if(IsKeyPressed(KEY_S)) {
            if(snake_direction.y != -1) {
                snake_direction = (Vector2){0, 1};
            }
        }

        if(IsKeyPressed(KEY_A)) {
            if(snake_direction.x != 1) {
                snake_direction = (Vector2){-1, 0};
            }
        }

        if(IsKeyPressed(KEY_D)) {
            if(snake_direction.x != -1) {
                snake_direction = (Vector2){1, 0};
            }
        }

        // update snake head
        frames_elapsed += 1;
        if(frames_elapsed > snake_movement_frame_delay) {
            frames_elapsed = 0;
            for(int i = snake_body_count - 1; i > 0; i--) {
                snake_body[i] = snake_body[i-1];
            }
            snake_body[0].x += snake_direction.x;
            snake_body[0].y += snake_direction.y;
            int collided_body_part_index = get_food_snake_collision_body_part_index(
                food,
                snake_body,
                snake_body_count
            );
            if(collided_body_part_index == 0) {
                food = generate_food(snake_body, snake_body_count);
                Vector2 snake_tail = snake_body[snake_body_count - 1];
                Vector2 part_before_snake_tail = snake_body[snake_body_count - 2];
                Vector2 diff = {
                    .x = snake_tail.x - part_before_snake_tail.x,
                    .y = snake_tail.y - part_before_snake_tail.y,
                };
                printf("diff: { %.3f, %.3f }\n", diff.x, diff.y);
                snake_body_count++;
                snake_body[snake_body_count-1] = (Vector2) {
                    .x = snake_tail.x + diff.x,
                    .y = snake_tail.y + diff.y,
                };
            }
        }
        BeginDrawing();
        {
            // draw_striped_bg();
            // draw_dense_striped_bg();
            // draw_thick_striped_bg();
            // draw_semi_thick_striped_bg();
            // draw_dot_matrix_bg();
            draw_plus_matrix_bg();
            // draw_grid_bg();
            draw_food(food);
            draw_snake(snake_body, snake_body_count);
        }
        EndDrawing();
    }
    CloseWindow();
}
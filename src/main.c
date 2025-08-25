#include <stdio.h>
#include <raylib.h>

#define QUEUE_IMPLEMENTATION
#include "utils/queue.h"

#define FPS 60
#define SNAKE_BODY_WIDTH 24
#define BG_PRIMARY CLITERAL(Color){ 6, 13, 17, 255 }
#define BG_SECONDARY CLITERAL(Color){ 28, 39, 45, 255 }

#define SNAKE_GREEN CLITERAL(Color){ 106, 200, 89, 255 }
#define SNAKE_BORDER_COLOR BLACK

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
} SnakeDirection;

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
    int dot_thickness = 4;
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

void draw_checkered_bg() {
    ClearBackground(BG_PRIMARY);
    int rows = GetScreenHeight() / SNAKE_BODY_WIDTH;
    int cols = GetScreenWidth() / SNAKE_BODY_WIDTH;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if((i + j) % 2 == 0) {
                DrawRectangle(
                    j * SNAKE_BODY_WIDTH,
                    i * SNAKE_BODY_WIDTH,
                    SNAKE_BODY_WIDTH,
                    SNAKE_BODY_WIDTH,
                    BG_SECONDARY
                );
            }
        }
    }
}

SnakeDirection get_opposite_snake_direction(SnakeDirection snake_direction) {
    switch (snake_direction)
    {
    case DIR_UP:
        return DIR_DOWN;
        break;
    case DIR_DOWN:
        return DIR_UP;
        break;
    case DIR_LEFT:
        return DIR_RIGHT;
        break;
    case DIR_RIGHT:
        return DIR_LEFT;
        break;
    default:
        fprintf(
            stderr,
            "Error: default case shouldn't be reached in function: "
            "get_opposite_snake_direction() function\n"
        );
        // TODO(huzaif): What would be a good return value here?
        return -1;
    }
}

Vector2 vector2_from_snake_direction(SnakeDirection snake_direction) {
    switch (snake_direction)
    {
    case DIR_UP:
        return (Vector2) {0, -1};
        break;
    case DIR_DOWN:
        return (Vector2) {0, 1};
        break;
    case DIR_LEFT:
        return (Vector2) {-1, 0};
        break;
    case DIR_RIGHT:
        return (Vector2) {1, 0};
        break;
    default:
        fprintf(
            stderr,
            "Error: default case shouldn't be reached in function: "
            "get_opposite_snake_direction() function\n"
        );
        // TODO(huzaif): What would be a good return value here?
        return (Vector2) {0, 0};
    }
}

char *snake_direction_to_string(SnakeDirection snake_direction) {
        switch(snake_direction) {
            case DIR_UP:
                return "UP";
            case DIR_DOWN:
                return "DOWN";
            case DIR_LEFT:
                return "LEFT";
            case DIR_RIGHT:
                return "RIGHT";
            default:
                return "";
        }
}

void input_queue_full_data_printer(Queue *q) {
    SnakeDirection *data = (SnakeDirection *) q->data;
    printf("Input Queue: [\n");
    for(size_t i = 0; i < q->capacity; i++) {
        char *start = "-";
        char *end = "-";
        if(q->start == i) {
            start = "s";
        }
        if(q->end == i) {
            end = "e";
        }
        char *dir = snake_direction_to_string(data[i]);
        printf("%s %s \t%zu => %s,\n", start, end, i, dir);
    }
    printf("]\n");
}

void input_queue_filled_data_printer(Queue *q) {
    SnakeDirection *data = (SnakeDirection *) q->data;
    printf("Input Queue: [\n");
    int i = q->start;
    while(1) {
        if(q->start == -1) {
            break;
        }
        int idx = i % q->capacity;
        char *start = "-";
        char *end = "-";
        if(q->start == idx) {
            start = "s";
        }
        if(q->end == idx) {
            end = "e";
        }
        char *dir = snake_direction_to_string(data[idx]);
        printf("%s %s %s,\n", start, end, dir);

        i += 1;

        if(i % q->capacity == q->end) {
            break;
        }
    }
    printf("]\n");
}

void input_queue_data_printer(Queue *q) {
    // input_queue_full_data_printer(q);
    input_queue_filled_data_printer(q);
}

int main() {
    const int screen_width = 32 * SNAKE_BODY_WIDTH;
    const int screen_height = 32 * SNAKE_BODY_WIDTH;
    int rows = screen_height / SNAKE_BODY_WIDTH;
    int cols = screen_width / SNAKE_BODY_WIDTH;
    InitWindow(screen_width, screen_height, "Snake");
    SetTargetFPS(FPS);
    // TODO(huzaif): Implement a dynamic array and use it for the snake body
    Vector2 snake_body[1000] = {
        (Vector2) {cols/2, rows/2},
        (Vector2) {cols/2 - 1, rows/2},
        (Vector2) {cols/2 - 2 , rows/2},
    };
    int snake_body_count = 3;
    SnakeDirection snake_direction = DIR_RIGHT;
    int frames_elapsed = 0;
    int snake_movement_frame_delay = 0.25 * FPS; // one second delay

    Vector2 food = generate_food(snake_body, snake_body_count);

    int score = 0;

    Queue *input_queue = queue_create(sizeof(SnakeDirection), 5);
    while(!WindowShouldClose()) {

        // TODO(huzaif): function peek_end() has not been tested
        // TODO(huzaif): test if the input queue implementation actually works properly;

        SnakeDirection *input_queue_latest = peek_end(input_queue);
        SnakeDirection direction_to_queue;
        if(IsKeyPressed(KEY_W)) {
            if(
                !input_queue_latest ||
                !(*input_queue_latest == DIR_UP || *input_queue_latest == DIR_DOWN)
            ) {
                direction_to_queue = DIR_UP;
                enqueue(input_queue, &direction_to_queue);
                printf("\nW pressed!\n");
                input_queue_data_printer(input_queue);
            }
        }

        if(IsKeyPressed(KEY_S)) {
            if(
                !input_queue_latest ||
                !(*input_queue_latest == DIR_UP || *input_queue_latest == DIR_DOWN)
            ) {
                direction_to_queue = DIR_DOWN;
                enqueue(input_queue, &direction_to_queue);
                printf("\nS pressed!\n");
                input_queue_data_printer(input_queue);
            }
        }

        if(IsKeyPressed(KEY_A)) {
            if(
                !input_queue_latest ||
                !(*input_queue_latest == DIR_LEFT || *input_queue_latest == DIR_RIGHT)
            ) {
                direction_to_queue = DIR_LEFT;
                enqueue(input_queue, &direction_to_queue);
                printf("\nA pressed!\n");
                input_queue_data_printer(input_queue);
            }
        }

        if(IsKeyPressed(KEY_D)) {
            if(
                !input_queue_latest ||
                !(*input_queue_latest == DIR_LEFT || *input_queue_latest == DIR_RIGHT)
            ) {
                direction_to_queue = DIR_RIGHT;
                enqueue(input_queue, &direction_to_queue);
                printf("\nD pressed!\n");
                input_queue_data_printer(input_queue);
            }
        }

        // update snake head
        frames_elapsed += 1;
        if(frames_elapsed > snake_movement_frame_delay) {
            frames_elapsed = 0;
            for(int i = snake_body_count - 1; i > 0; i--) {
                snake_body[i] = snake_body[i-1];
            }
            SnakeDirection *next_candidate_snake_direction = dequeue(input_queue);
            if(
                next_candidate_snake_direction &&
                *next_candidate_snake_direction != get_opposite_snake_direction(snake_direction)
            ) {
                snake_direction = *next_candidate_snake_direction;
            }
            Vector2 vector2_snake_direction = vector2_from_snake_direction(snake_direction);
            snake_body[0].x += vector2_snake_direction.x;
            snake_body[0].y += vector2_snake_direction.y;
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
            // draw_plus_matrix_bg();
            // draw_grid_bg();
            draw_checkered_bg();
            draw_food(food);
            draw_snake(snake_body, snake_body_count);
        }
        EndDrawing();
    }
    CloseWindow();
}
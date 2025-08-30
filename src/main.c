#include <stdio.h>
#include <raylib.h>

#define QUEUE_IMPLEMENTATION
#include "utils/queue.h"

#define FPS 60

#define BG_PRIMARY CLITERAL(Color){ 11, 24, 31, 255 }
#define BG_SECONDARY CLITERAL(Color){ 28, 39, 45, 255 }

#define SNAKE_GREEN CLITERAL(Color){ 106, 200, 89, 255 }

#define SNAKE_BODY_WIDTH 24
#define ROWS 32
#define COLS 32
#define GRID_MARGIN (2 * SNAKE_BODY_WIDTH)
#define GRID_WIDTH (COLS * SNAKE_BODY_WIDTH)
#define GRID_HEIGHT (ROWS * SNAKE_BODY_WIDTH)

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
} SnakeDirection;

typedef enum {
    PLAYING,
    PAUSED,
    GAME_OVER,
} GameState;

static GameState game_state = PLAYING;

int food_snake_body_collision_index(Vector2 food, Vector2 *snake_body, int snake_body_count) {
    for(int i = 0; i < snake_body_count; i++) {
        if(snake_body[i].x == food.x && snake_body[i].y == food.y) {
            return i;
        }
    }
    return -1;
}

int snake_head_snake_body_collision_index(Vector2 *snake_body, int snake_body_count) {
    Vector2 snake_head = snake_body[0];
    for(int i = 1; i < snake_body_count; i++) {
        if(snake_head.x == snake_body[i].x && snake_head.y == snake_body[i].y) {
            return i;
        }
    }

    return -1;
}

Vector2 generate_food(Vector2 *snake_body, int snake_body_count) {
    int row = GetRandomValue(0, ROWS-1);
    int col = GetRandomValue(0, COLS-1);
    bool generated = false;
    do {
        if(food_snake_body_collision_index(
            (Vector2) {col, row},
            snake_body,
            snake_body_count
        ) != -1) {
            row = GetRandomValue(0, ROWS-1);
            col = GetRandomValue(0, COLS-1);
        } else {
            generated = true;
        }
    } while(!generated);
    return (Vector2) {col, row};
}

typedef void draw_snake_fn(Vector2 *snake_body, int snake_body_count);

void draw_rainbow_snake(Vector2 *snake_body, int snake_body_count) {
    int body_part_padding = 0.2 * SNAKE_BODY_WIDTH;
    for(int i = snake_body_count - 1; i >= 0; i--) {
        Vector2 snake_body_part = snake_body[i];
        Color snake_color = ColorFromHSV(360.0f * (float)i/snake_body_count, 1.0f, 1.0f);
        if(i == 0) {
            snake_color = WHITE;
            if(game_state == GAME_OVER) {
                snake_color = RED;
            }
        }

        Rectangle body_part_rect = {
            SNAKE_BODY_WIDTH * (snake_body_part.x) + body_part_padding + GRID_MARGIN,
            SNAKE_BODY_WIDTH * (snake_body_part.y) + body_part_padding + GRID_MARGIN,
            SNAKE_BODY_WIDTH - 2 * body_part_padding,
            SNAKE_BODY_WIDTH - 2 * body_part_padding,
        };

        float roundness = 0.5;
        int segments = 4;
        DrawRectangleRounded(
            body_part_rect,
            roundness,
            segments,
            snake_color
        );
        // DrawRectangleRoundedLines(
        //     body_part_rect,
        //     roundness,
        //     segments,
        //     BG_PRIMARY
        // );
    }

    for(int i = 0; i < snake_body_count - 1; i++) {
        Vector2 part1, part2;
        part1 = snake_body[i];
        part2 = snake_body[i+1];
        if(snake_body[i].x > snake_body[i+1].x) {
            part1 = snake_body[i+1];
            part2 = snake_body[i];
        }
        if(snake_body[i].y > snake_body[i+1].y) {
            part1 = snake_body[i+1];
            part2 = snake_body[i];
        }
        Vector2 diff = (Vector2) {part1.x - part2.x, part1.y - part2.y};
        // should always be negative for horizontally aligned parts
        // because part1.x is less than part2.x from the nature of
        // the above conditional assignments
        Color snake_color = ColorFromHSV(360.0f * ((float)i + 0.5)/snake_body_count, 1.0f, 1.0f);
        if(diff.x < 0) { // assuming x is -1
            DrawRectangle(
                part2.x * SNAKE_BODY_WIDTH - body_part_padding + GRID_MARGIN,
                part2.y * SNAKE_BODY_WIDTH + body_part_padding + 1 + GRID_MARGIN,
                2 * body_part_padding,
                SNAKE_BODY_WIDTH - 2 * body_part_padding - 2,
                // GRAY
                snake_color
            );
            // DrawRectangleLines(
            //     part2.x * SNAKE_BODY_WIDTH - body_part_padding + GRID_MARGIN,
            //     part2.y * SNAKE_BODY_WIDTH + body_part_padding + 1 + GRID_MARGIN,
            //     2 * body_part_padding,
            //     SNAKE_BODY_WIDTH - 2 * body_part_padding - 2,
            //     BG_PRIMARY
            // );
        } else {    // assuming y is -1
            DrawRectangle(
                part2.x * SNAKE_BODY_WIDTH + body_part_padding + 1 + GRID_MARGIN,
                part2.y * SNAKE_BODY_WIDTH - body_part_padding + GRID_MARGIN,
                SNAKE_BODY_WIDTH - 2 * body_part_padding -2,
                2 * body_part_padding,
                // GRAY
                snake_color
            );
            // DrawRectangleLines(
            //     part2.x * SNAKE_BODY_WIDTH + body_part_padding + 1 + GRID_MARGIN,
            //     part2.y * SNAKE_BODY_WIDTH - body_part_padding + GRID_MARGIN,
            //     SNAKE_BODY_WIDTH - 2 * body_part_padding -2,
            //     2 * body_part_padding,
            //     BG_PRIMARY
            // );
        }
    }
}

void draw_snake_piped(Vector2 *snake_body, int snake_body_count) {
    for(int i = snake_body_count - 1; i >= 0; i--) {
        Vector2 snake_body_part = snake_body[i];
        Color snake_color = SNAKE_GREEN;
        if(i == 0 && game_state == GAME_OVER) {
            snake_color = RED;
        }
        DrawRectangle(
            SNAKE_BODY_WIDTH * (snake_body_part.x) + GRID_MARGIN,
            SNAKE_BODY_WIDTH * (snake_body_part.y) + GRID_MARGIN,
            SNAKE_BODY_WIDTH,
            SNAKE_BODY_WIDTH,
            snake_color
        );
    }
    for(int i = snake_body_count - 1; i > 0; i--) {
        Vector2 part1 = snake_body[i];
        Vector2 part2 = snake_body[i-1];
        if(snake_body[i].x > snake_body[i-1].x) {
            part1 = snake_body[i-1];
            part2 = snake_body[i];
        }
        if(snake_body[i].y > snake_body[i-1].y) {
            part1 = snake_body[i-1];
            part2 = snake_body[i];
        }

        Vector2 diff = (Vector2) {
            part1.x - part2.x,
            part1.y - part2.y
        };

        printf("Diff: {%.3f, %.3f}\n", diff.x, diff.y);

        Color pipe_color = { 106 * 0.5, 200 * 0.5, 89 * 0.5, 255 };
        int pipe_width = SNAKE_BODY_WIDTH/4;
        // if the adjacent body parts are separated by COLS - 1 cells
        // because the snake passed through the left or right screen boundary
        if(diff.x < -1){ 
            DrawRectangle(
                SNAKE_BODY_WIDTH * (part1.x) + GRID_MARGIN,
                SNAKE_BODY_WIDTH * (part1.y + 0.5) - pipe_width/2 + GRID_MARGIN,
                SNAKE_BODY_WIDTH/2 + pipe_width/2,
                pipe_width,
                pipe_color
            );
            DrawRectangle(
                SNAKE_BODY_WIDTH * (part2.x + 0.5) - pipe_width/2 + GRID_MARGIN,
                SNAKE_BODY_WIDTH * (part1.y + 0.5) - pipe_width/2 + GRID_MARGIN,
                SNAKE_BODY_WIDTH/2 + pipe_width/2,
                pipe_width,
                pipe_color
            );
        } else if(diff.x < 0){
            DrawRectangle(
                SNAKE_BODY_WIDTH * (part1.x + 0.5) - pipe_width/2 + GRID_MARGIN,
                SNAKE_BODY_WIDTH * (part1.y + 0.5) - pipe_width/2 + GRID_MARGIN,
                SNAKE_BODY_WIDTH + pipe_width,
                pipe_width,
                pipe_color
            );
        // if the adjacent body parts are separated by ROWS - 1 cells
        // because the snake passed through the top or bottom screen boundary
        } else if(diff.y < -1) {
            DrawRectangle(
                SNAKE_BODY_WIDTH * (part1.x + 0.5) - pipe_width/2 + GRID_MARGIN,
                SNAKE_BODY_WIDTH * (part1.y ) + GRID_MARGIN,
                pipe_width,
                SNAKE_BODY_WIDTH/2 + pipe_width/2,
                pipe_color
            );
            DrawRectangle(
                SNAKE_BODY_WIDTH * (part1.x + 0.5) - pipe_width/2 + GRID_MARGIN,
                SNAKE_BODY_WIDTH * (part2.y + 0.5) - pipe_width/2 + GRID_MARGIN,
                pipe_width,
                SNAKE_BODY_WIDTH/2 + pipe_width/2,
                pipe_color
            );
        }
        else {
            DrawRectangle(
                SNAKE_BODY_WIDTH * (part1.x + 0.5) - pipe_width/2 + GRID_MARGIN,
                SNAKE_BODY_WIDTH * (part1.y + 0.5) - pipe_width/2 + GRID_MARGIN,
                pipe_width,
                SNAKE_BODY_WIDTH + pipe_width,
                pipe_color
            );
        }
    }
}

void draw_snake_simple(Vector2 *snake_body, int snake_body_count) {
    for(int i = snake_body_count - 1; i >= 0; i--) {
        Vector2 snake_body_part = snake_body[i];
        Color snake_color = SNAKE_GREEN;
        if(i == 0) {
            snake_color = WHITE;
            if(game_state == GAME_OVER) {
                snake_color = RED;
            }
        }
        DrawRectangle(
            SNAKE_BODY_WIDTH * (snake_body_part.x) + GRID_MARGIN,
            SNAKE_BODY_WIDTH * (snake_body_part.y) + GRID_MARGIN,
            SNAKE_BODY_WIDTH,
            SNAKE_BODY_WIDTH,
            snake_color
        );
        DrawRectangleLines(
            SNAKE_BODY_WIDTH * (snake_body_part.x) + GRID_MARGIN,
            SNAKE_BODY_WIDTH * (snake_body_part.y) + GRID_MARGIN,
            SNAKE_BODY_WIDTH,
            SNAKE_BODY_WIDTH,
            BG_PRIMARY
        );
    }
}

void draw_food(Vector2 food) {
    DrawRectangle(
        SNAKE_BODY_WIDTH * (food.x) + GRID_MARGIN,
        SNAKE_BODY_WIDTH * (food.y) + GRID_MARGIN,
        SNAKE_BODY_WIDTH,
        SNAKE_BODY_WIDTH,
        ORANGE
    );

    // draw left eye
    DrawCircle(
        SNAKE_BODY_WIDTH * (food.x + 0.3) + GRID_MARGIN,
        SNAKE_BODY_WIDTH * (food.y + 0.3) + GRID_MARGIN,
        0.1 * SNAKE_BODY_WIDTH,
        BG_PRIMARY
    );

    // draw right eye
    DrawCircle(
        SNAKE_BODY_WIDTH * (food.x + 0.7) + GRID_MARGIN,
        SNAKE_BODY_WIDTH * (food.y + 0.3) + GRID_MARGIN,
        0.1 * SNAKE_BODY_WIDTH,
        BG_PRIMARY
    );

    // draw smiley
    if(game_state == GAME_OVER) {
        DrawCircleSector(
            (Vector2) {
                SNAKE_BODY_WIDTH * (food.x + 0.5) + GRID_MARGIN,
                SNAKE_BODY_WIDTH * (food.y + 0.5) + GRID_MARGIN,
            },
            SNAKE_BODY_WIDTH/4,
            0,
            180,
            3,
            BG_PRIMARY
        );
    } else if(game_state == PAUSED) {
        DrawLineEx(
            (Vector2) {
                SNAKE_BODY_WIDTH * (food.x + 0.3) + GRID_MARGIN,
                SNAKE_BODY_WIDTH * (food.y + 0.7) + GRID_MARGIN,
            },
            (Vector2) {
                SNAKE_BODY_WIDTH * (food.x + 0.7) + GRID_MARGIN,
                SNAKE_BODY_WIDTH * (food.y + 0.7) + GRID_MARGIN,
            },
            2,
            BG_PRIMARY
        );
    } else if(game_state == PLAYING) {
        DrawCircleSector(
            (Vector2) {
                SNAKE_BODY_WIDTH * (food.x + 0.5) + GRID_MARGIN,
                SNAKE_BODY_WIDTH * (food.y + 0.75) + GRID_MARGIN,
            },
            SNAKE_BODY_WIDTH/4,
            180,
            360,
            3,
            BG_PRIMARY
        );
    }

    Color food_highlight_color = GRAY;
    int time_ms = (int) (GetTime() * 1000.0);
    int highlight_offset = 4 + (time_ms % 300)/100;

    // draw right from top-left
    DrawLineEx(
        (Vector2) {SNAKE_BODY_WIDTH * food.x - (highlight_offset + 1) + GRID_MARGIN, SNAKE_BODY_WIDTH * food.y - highlight_offset + GRID_MARGIN},
        (Vector2) {SNAKE_BODY_WIDTH * food.x + highlight_offset + GRID_MARGIN, SNAKE_BODY_WIDTH * food.y - highlight_offset + GRID_MARGIN},
        2,
        food_highlight_color
    );

    // draw down from top-left
    DrawLineEx(
        (Vector2) {SNAKE_BODY_WIDTH * food.x - highlight_offset + GRID_MARGIN, SNAKE_BODY_WIDTH * food.y - (highlight_offset + 1) + GRID_MARGIN},
        (Vector2) {SNAKE_BODY_WIDTH * food.x - highlight_offset + GRID_MARGIN, SNAKE_BODY_WIDTH * food.y + highlight_offset + GRID_MARGIN},
        2,
        food_highlight_color
    );

    // draw right from bottom-left
    DrawLineEx(
        (Vector2) {SNAKE_BODY_WIDTH * food.x - (highlight_offset + 1) + GRID_MARGIN, SNAKE_BODY_WIDTH * (food.y + 1) + highlight_offset + GRID_MARGIN},
        (Vector2) {SNAKE_BODY_WIDTH * food.x + highlight_offset + GRID_MARGIN, SNAKE_BODY_WIDTH * (food.y + 1) + highlight_offset + GRID_MARGIN},
        2,
        food_highlight_color
    );

    // draw up from bottom-left
    DrawLineEx(
        (Vector2) {SNAKE_BODY_WIDTH * food.x - highlight_offset + GRID_MARGIN, SNAKE_BODY_WIDTH * (food.y + 1) - (highlight_offset + 1) + GRID_MARGIN},
        (Vector2) {SNAKE_BODY_WIDTH * food.x - highlight_offset + GRID_MARGIN, SNAKE_BODY_WIDTH * (food.y + 1) + highlight_offset + GRID_MARGIN},
        2,
        food_highlight_color
    );

    // draw left from top-right
    DrawLineEx(
        (Vector2) {SNAKE_BODY_WIDTH * (food.x + 1) + (highlight_offset + 1) + GRID_MARGIN, SNAKE_BODY_WIDTH * food.y - highlight_offset + GRID_MARGIN},
        (Vector2) {SNAKE_BODY_WIDTH * (food.x + 1) - highlight_offset + GRID_MARGIN, SNAKE_BODY_WIDTH * food.y - highlight_offset + GRID_MARGIN},
        2,
        food_highlight_color
    );

    // draw down from top-right
    DrawLineEx(
        (Vector2) {SNAKE_BODY_WIDTH * (food.x + 1) + highlight_offset + GRID_MARGIN, SNAKE_BODY_WIDTH * food.y - (highlight_offset + 1) + GRID_MARGIN},
        (Vector2) {SNAKE_BODY_WIDTH * (food.x + 1) + highlight_offset + GRID_MARGIN, SNAKE_BODY_WIDTH * food.y + highlight_offset + GRID_MARGIN},
        2,
        food_highlight_color
    );

    // draw left from bottom-right
    DrawLineEx(
        (Vector2) {SNAKE_BODY_WIDTH * (food.x + 1) - highlight_offset + GRID_MARGIN, SNAKE_BODY_WIDTH * (food.y + 1) + highlight_offset + GRID_MARGIN},
        (Vector2) {SNAKE_BODY_WIDTH * (food.x + 1) + (highlight_offset + 1) + GRID_MARGIN, SNAKE_BODY_WIDTH * (food.y + 1) + highlight_offset + GRID_MARGIN},
        2,
        food_highlight_color
    );

    // draw up from bottom-right
    DrawLineEx(
        (Vector2) {SNAKE_BODY_WIDTH * (food.x + 1) + highlight_offset + GRID_MARGIN, SNAKE_BODY_WIDTH * (food.y + 1) - (highlight_offset + 1) + GRID_MARGIN},
        (Vector2) {SNAKE_BODY_WIDTH * (food.x + 1) + highlight_offset + GRID_MARGIN, SNAKE_BODY_WIDTH * (food.y + 1) + highlight_offset + GRID_MARGIN},
        2,
        food_highlight_color
    );
}

typedef void draw_background(void);

void draw_striped_bg() {
    ClearBackground(BG_PRIMARY);
    int line_thickness = 2;
    for(int i = 0; i < ROWS; i++) {
        DrawLineEx(
            (Vector2) {GRID_MARGIN, i * SNAKE_BODY_WIDTH + GRID_MARGIN},
            (Vector2) {GRID_WIDTH + GRID_MARGIN, i * SNAKE_BODY_WIDTH + GRID_MARGIN},
            line_thickness,
            BG_SECONDARY
        );
    }
    
}
void draw_dense_striped_bg() {
    ClearBackground(BG_PRIMARY);
    int line_thickness = 3;
    for(int i = 0; i < 2 * ROWS; i++) {
        DrawLineEx(
            (Vector2) {GRID_MARGIN, i * SNAKE_BODY_WIDTH/2 + GRID_MARGIN},
            (Vector2) {GRID_WIDTH + GRID_MARGIN, i * SNAKE_BODY_WIDTH/2 + GRID_MARGIN},
            line_thickness,
            BG_SECONDARY
        );
    }
    
}

void draw_thick_striped_bg() {
    ClearBackground(BG_PRIMARY);
    for(int i = 0; i < ROWS; i++) {
        if(i % 2) {
            DrawRectangle(
                GRID_MARGIN,
                i * SNAKE_BODY_WIDTH + GRID_MARGIN,
                GRID_WIDTH,
                SNAKE_BODY_WIDTH,
                BG_SECONDARY
            );
        }
    }
    
}

void draw_semi_thick_striped_bg() {
    ClearBackground(BG_PRIMARY);
    for(int i = 0; i < 2 * ROWS; i++) {
        if(i % 2) {
            DrawRectangle(
                GRID_MARGIN,
                i * SNAKE_BODY_WIDTH/2 + GRID_MARGIN,
                GRID_WIDTH,
                SNAKE_BODY_WIDTH/2,
                BG_SECONDARY
            );
        }
    }
    
}

void draw_dot_matrix_bg() {
    ClearBackground(BG_PRIMARY);
    int dot_thickness = 4;
    for(int i = 1; i < ROWS; i++) {
        for(int j = 1; j < COLS; j++) {
            DrawRectangle(
                j * SNAKE_BODY_WIDTH - dot_thickness/2 + GRID_MARGIN,
                i * SNAKE_BODY_WIDTH - dot_thickness/2 + GRID_MARGIN,
                dot_thickness,
                dot_thickness,
                BG_SECONDARY
            );
        }
    }
    
}

void draw_plus_matrix_bg() {
    ClearBackground(BG_PRIMARY);
    int line_thickness = 2;
    int plus_thickness = 12;
    for(int i = 1; i < ROWS; i++) {
        for(int j = 1; j < COLS; j++) {
            DrawLineEx(
                (Vector2) {j * SNAKE_BODY_WIDTH - plus_thickness/2 + GRID_MARGIN, i * SNAKE_BODY_WIDTH + GRID_MARGIN},
                (Vector2) {j * SNAKE_BODY_WIDTH + plus_thickness/2 + GRID_MARGIN, i * SNAKE_BODY_WIDTH + GRID_MARGIN},
                line_thickness,
                BG_SECONDARY
            );
            DrawLineEx(
                (Vector2) {j * SNAKE_BODY_WIDTH + GRID_MARGIN, i * SNAKE_BODY_WIDTH - plus_thickness/2 + GRID_MARGIN},
                (Vector2) {j * SNAKE_BODY_WIDTH + GRID_MARGIN, i * SNAKE_BODY_WIDTH + plus_thickness/2 + GRID_MARGIN},
                line_thickness,
                BG_SECONDARY
            );
        }
    }
    
}

void draw_grid_bg() {
    ClearBackground(BG_PRIMARY);
    int line_thickness = 2;
    for(int i = 0; i <= ROWS; i++) {
        DrawLineEx(
            (Vector2) {GRID_MARGIN, i * SNAKE_BODY_WIDTH + GRID_MARGIN},
            (Vector2) {GRID_WIDTH + GRID_MARGIN, i * SNAKE_BODY_WIDTH + GRID_MARGIN},
            line_thickness,
            BG_SECONDARY
        );
    }
    for(int i = 0; i <= COLS; i++) {
        DrawLineEx(
            (Vector2) {i * SNAKE_BODY_WIDTH + GRID_MARGIN, GRID_MARGIN},
            (Vector2) {i * SNAKE_BODY_WIDTH + GRID_MARGIN, GRID_HEIGHT + GRID_MARGIN},
            line_thickness,
            BG_SECONDARY
        );
    }
}

void draw_checkered_bg() {
    ClearBackground(BG_PRIMARY);
    for(int i = 0; i < ROWS; i++) {
        for(int j = 0; j < COLS; j++) {
            if((i + j) % 2 == 0) {
                DrawRectangle(
                    j * SNAKE_BODY_WIDTH + GRID_MARGIN,
                    i * SNAKE_BODY_WIDTH + GRID_MARGIN,
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

void init_new_game(
    Vector2 *out_snake_body,
    int *out_snake_body_count,
    SnakeDirection *out_snake_direction,
    Vector2 *out_food,
    int *out_score,
    Queue *input_queue
) {
    
    out_snake_body[0] = (Vector2) {COLS/2, ROWS/2};
    out_snake_body[1] = (Vector2) {COLS/2 - 1, ROWS/2},
    out_snake_body[2] = (Vector2) {COLS/2 - 2 , ROWS/2},
    *out_snake_body_count = 3;
    *out_snake_direction = DIR_RIGHT;
    *out_food = generate_food(out_snake_body, *out_snake_body_count);
    *out_score = 0;
    
    input_queue->start = -1;
    input_queue->end = 0;
    game_state = PLAYING;
}

int main() {
    const int screen_width = GRID_WIDTH + 2 * GRID_MARGIN;
    const int screen_height = GRID_HEIGHT + 2 * GRID_MARGIN;
    InitWindow(screen_width, screen_height, "Snake");
    InitAudioDevice();
    SetExitKey(KEY_NULL);
    SetTargetFPS(FPS);
    
    Vector2 snake_body[ROWS * COLS];
    int snake_body_count;
    SnakeDirection snake_direction;
    Vector2 food;
    Queue *input_queue = queue_create(sizeof(SnakeDirection), 10);
    int score;
    init_new_game(
        snake_body,
        &snake_body_count,
        &snake_direction,
        &food,
        &score,
        input_queue
    );
    
    // load sounds
    Sound crunch_sound = LoadSound("assets/sounds/crunch.mp3");
    Sound game_over_sound = LoadSound("assets/sounds/game_over.mp3");
    
    draw_background *draw_bg[] = {
        draw_plus_matrix_bg,
        draw_dot_matrix_bg,
        draw_checkered_bg,
        draw_grid_bg,
        draw_dense_striped_bg,
        draw_semi_thick_striped_bg,
        draw_thick_striped_bg,
        draw_striped_bg,
    };
    int draw_bg_fn_index = 0;

    draw_snake_fn *draw_snake = draw_snake_piped;

    int frames_elapsed = 0;
    int snake_movement_frame_delay = 0.02 * FPS; // one second delay
    
    while(!WindowShouldClose()) {
        
        // TODO(huzaif): function peek_end() has not been tested
        // TODO(huzaif): test if the input queue implementation actually works properly;
        
        SnakeDirection *input_queue_latest = peek_end(input_queue);
        SnakeDirection direction_to_queue;
        if(IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            if(
                !input_queue_latest ||
                !(*input_queue_latest == DIR_UP || *input_queue_latest == DIR_DOWN)
            ) {
                direction_to_queue = DIR_UP;
                enqueue(input_queue, &direction_to_queue);
            }
        }

        if(IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            if(
                !input_queue_latest ||
                !(*input_queue_latest == DIR_UP || *input_queue_latest == DIR_DOWN)
            ) {
                direction_to_queue = DIR_DOWN;
                enqueue(input_queue, &direction_to_queue);
            }
        }

        if(IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
            if(
                !input_queue_latest ||
                !(*input_queue_latest == DIR_LEFT || *input_queue_latest == DIR_RIGHT)
            ) {
                direction_to_queue = DIR_LEFT;
                enqueue(input_queue, &direction_to_queue);
            }
        }

        if(IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
            if(
                !input_queue_latest ||
                !(*input_queue_latest == DIR_LEFT || *input_queue_latest == DIR_RIGHT)
            ) {
                direction_to_queue = DIR_RIGHT;
                enqueue(input_queue, &direction_to_queue);
            }
        }

        if(IsKeyPressed(KEY_ESCAPE)) {
            if(game_state == PLAYING) {
                game_state = PAUSED;
            } else if(game_state == PAUSED) {
                game_state = PLAYING;
            }
        }

        if(IsKeyPressed(KEY_R)) {
            if(game_state == GAME_OVER) {
                init_new_game(
                    snake_body,
                    &snake_body_count,
                    &snake_direction,
                    &food,
                    &score,
                    input_queue
                );
                StopSound(game_over_sound);
            }
        }

        int key_pressed = 0;
        while((key_pressed = GetKeyPressed()) != 0) {
            if(key_pressed >= KEY_ZERO && key_pressed <= KEY_NINE) {
                // TODO(huzaif): I don't know if this is a good way
                size_t draw_bg_array_size = sizeof(draw_bg)/sizeof(draw_bg[0]);
                int idx = key_pressed - KEY_ZERO - 1;
                if(idx >= 0 && idx < draw_bg_array_size) {
                    draw_bg_fn_index = idx;
                }
            }
        }

        // update snake head
        frames_elapsed += 1;
        if(game_state == PLAYING && frames_elapsed > snake_movement_frame_delay) {
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
            if(snake_body[0].x < 0) {
                snake_body[0].x = COLS - 1;
            }
            if(snake_body[0].x >= COLS) {
                snake_body[0].x = 0;
            }
            snake_body[0].y += vector2_snake_direction.y;
            if(snake_body[0].y < 0) {
                snake_body[0].y = ROWS - 1;
            }
            if(snake_body[0].y >= COLS) {
                snake_body[0].y = 0;
            }
            int collided_body_part_index = food_snake_body_collision_index(
                food,
                snake_body,
                snake_body_count
            );
            if(collided_body_part_index == 0) {
                score++;
                PlaySound(crunch_sound);
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

            collided_body_part_index = snake_head_snake_body_collision_index(snake_body, snake_body_count);
            if(collided_body_part_index != -1) {
                if(game_state == PLAYING) {
                    PlaySound(game_over_sound);
                }
                game_state = GAME_OVER;
            }
        }

        char score_text[256];
        sprintf(score_text, "Score: %d", score);
        int score_text_width = MeasureText(score_text, 20);

        char game_over_text[256];
        sprintf(game_over_text, "Game Over!");
        int game_over_text_width = MeasureText(game_over_text, 20);

        char game_paused_text[256];
        sprintf(game_paused_text, "Paused");
        int game_paused_text_width = MeasureText(game_paused_text, 20);

        char restart_game_text[256];
        sprintf(restart_game_text, "Press 'R' to restart");
        int restart_game_text_width = MeasureText(restart_game_text, 40);
        BeginDrawing();
        {
            draw_bg[draw_bg_fn_index]();
            // draw grid bounding box
            DrawRectangleLines(
                GRID_MARGIN,
                GRID_MARGIN,
                GRID_WIDTH,
                GRID_HEIGHT,
                BG_SECONDARY
            );
            draw_snake(snake_body, snake_body_count);
            draw_food(food);
            DrawText(score_text, screen_width - score_text_width - 10, 4, 20, ORANGE);
            if(game_state == GAME_OVER) {
                DrawRectangle(
                    0, 0,
                    screen_width, screen_height,
                    (Color) {.r = 0, .g = 0, .b = 0, .a = 100}
                );
                DrawText(game_over_text, screen_width/2 - game_over_text_width/2, 4, 20, RED);
                DrawText(restart_game_text, screen_width/2 - restart_game_text_width/2, screen_height/2 - 20, 40, GREEN);

            } else if(game_state == PAUSED) {
                DrawText(game_paused_text, screen_width/2 - game_paused_text_width/2, 4, 20, YELLOW);
            }
        }
        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();
}
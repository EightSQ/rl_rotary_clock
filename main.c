#include <stdio.h>
#include <math.h>
#include <raylib.h>

typedef struct {
    int x, y;
    float angle[2];
    float speed;
    float neighbor_speed_sum;
    int neighbors;
    bool speed_locked;
} ClockT;

const int clock_hand_length = 30;
void draw_clock(const ClockT *clock) {
    Color col;
    if (clock->speed >= 0) {
        col = GREEN;
    } else {
        col = RED;
    }
    col.a = 200 * (0.5 * 1.0f/PI) * fabsf(clock->speed);
    Vector2 ring_center;
    ring_center.x = clock->x;
    ring_center.y = clock->y;
    DrawRing(ring_center, clock_hand_length - 5, clock_hand_length, 0.0f, 360.0f, 1, col);
    //DrawCircleLines(clock->x, clock->y, clock_hand_length, col);

    // speed text
    char speed_text[16];
    snprintf(speed_text, 16, "%f", 0.5f * 1/PI * clock->speed);
    DrawText(speed_text, clock->x - clock_hand_length/2, clock->y, 8, GRAY);

    for (int i = 0; i < 2; i++) {
        int hand_length = clock_hand_length / (i + 1);
        int end_x = clock->x + hand_length * cos(clock->angle[i]);
        int end_y = clock->y + hand_length * sin(clock->angle[i]);
        DrawLine(clock->x, clock->y, end_x, end_y, i == 0 ? BLACK : DARKGREEN);
    }
}

const float adaptation_step_size = 0.3f;
void update_clock(ClockT *clock, float frame_time) {
    if (!clock->speed_locked) {
        clock->speed += frame_time * adaptation_step_size * ((clock->neighbor_speed_sum / clock->neighbors) - clock->speed);
        clock->neighbor_speed_sum = 0.0f;
    }
    clock->angle[0] += clock->speed * frame_time;
    clock->angle[0] -= 2 * PI * (clock->angle[0] > 2 * PI);
    clock->angle[1] += (1.0f/60.0f) * clock->speed * frame_time;
    clock->angle[1] -= 2 * PI * (clock->angle[1] > 2 * PI);
}


const int screenWidth = 1920;
const int screenHeight = 1080;
const int x_offset = clock_hand_length + 10;
const int y_offset = clock_hand_length + 10;
const int clocks_x = (screenWidth - x_offset) / (2 * clock_hand_length);
const int clocks_y = (screenHeight - y_offset) / (2 * clock_hand_length);

void create_clocks(ClockT clocks[clocks_x][clocks_y]) {
    for (int i = 0; i < clocks_x; i++) {
        for (int j = 0; j < clocks_y; j++) {
            clocks[i][j].x = x_offset + i * clock_hand_length * 2;
            clocks[i][j].y = y_offset + j * clock_hand_length * 2;
            clocks[i][j].angle[0] = (2.0f/2000.f) * GetRandomValue(0, 2000) * PI;
            clocks[i][j].angle[1] = 0.001f * GetRandomValue(0, 2000) * PI;
            clocks[i][j].speed = 2 * PI * (0.0005 * GetRandomValue(-2000, 2000));
            clocks[i][j].speed_locked = false;
            // left middle
            if (i == (clocks_x/2) && j == (clocks_y/2)) {
                clocks[i][j].speed = 2 * PI;
                clocks[i][j].speed_locked = true;
            }
            // right middle
            if (i == (clocks_x/2 + 1) && j == (clocks_y/2)) {
                clocks[i][j].speed = -2 * PI;
                clocks[i][j].speed_locked = true;
            }
            int borders = 0;
            borders += i == 0 || i == (clocks_x - 1);
            borders += j == 0 || j == (clocks_y - 1);
            if (borders == 2) {
                clocks[i][j].neighbors = 3;
            } else if (borders == 1) {
                clocks[i][j].neighbors = 5;
            } else {
                clocks[i][j].neighbors = 8;
            }
            clocks[i][j].neighbor_speed_sum = 0.0f;
        }
    }
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "time is running out");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // create clocks

    ClockT clocks[clocks_x][clocks_y];
    create_clocks(clocks);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(WHITE);

        for (int i = 0; i < clocks_x; i++) {
            for (int j = 0; j < clocks_y; j++) {
                draw_clock(&clocks[i][j]);
            }
        }
        EndDrawing();
        //----------------------------------------------------------------------------------

        // Post Update
        for (int i = 0; i < clocks_x; i++) {
            for (int j = 0; j < clocks_y; j++) {
                for (int ni = i - 1; ni <= i + 1; ni++) {
                    for (int nj = j - 1; nj <= j + 1; nj++) {
                        if (ni == i && nj == j) continue;
                        if (ni >= 0 && ni < clocks_x && nj >= 0 && nj < clocks_y) {
                            clocks[i][j].neighbor_speed_sum += clocks[ni][nj].speed;
                        }
                    }
                }
                update_clock(&clocks[i][j], GetFrameTime());
            }
        }

    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

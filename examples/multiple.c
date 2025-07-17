#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "raylib.h"

int main(void)
{
    SetTargetFPS(144);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 800, "graph.c");

    Graph gs[] = {
        graph(25, RED, BLACK, DARKGRAY),
        graph(50, BLUE, BLACK, DARKGRAY),
        graph(75, GREEN, BLACK, DARKGRAY),
        graph(100, YELLOW, BLACK, DARKGRAY),
    };
    size_t gs_count = sizeof gs / sizeof gs[0];

    for (size_t i = 0; i < gs_count; i++) {
        Graph* g = &gs[i];
        g->st_reset.pos = (Vector2) { .x = 106.00, .y = 107.00 };
        g->st_reset.scale = (Vector3) { .x = 1.00, .y = 1.00, .z = 1.83 };
        g->st_current = g->st_reset;

        Line* x = line_alloc(g, 101, "X", RED);
        Line* y = line_alloc(g, 101, "Y", BLUE);
        Line* l = line_alloc(g, 101, "Line", GREEN);
        Line* c = line_alloc(g, 101, "Shape", YELLOW);

        for (size_t j = 0; j < 101; j++) {
            x->points[j] = (Vector2) { j, 0.0 };
            y->points[j] = (Vector2) { 0.0, j };
            l->points[j] = (Vector2) { j, j * j / 100.0 };
            c->points[j] = (Vector2) {
                cos(j * PI * 2 / (i + 3)) * 25.0 + 50.0,
                sin(j * PI * 2 / (i + 3)) * 25.0 + 50.0,
            };
        }
    }

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_Q)) {
            break;
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);

        int const margin = 4;
        int screen_width = GetScreenWidth();
        int screen_height = GetScreenHeight();

        int half_width = screen_width / 2;
        int half_height = screen_height / 2;

        for (size_t i = 0; i < gs_count; i++) {
            Graph* g = &gs[i];

            int col = i % 2;
            int row = i / 2;
            g->bound = (Rectangle) {
                col * half_width + margin,
                row * half_height + margin,
                half_width - 2 * margin,
                half_height - 2 * margin
            };

            graph_update(g);

            Vector2 mpos = GetMousePosition();
            if (CheckCollisionPointRec(mpos, g->bound)) {
                if (IsKeyPressed(KEY_A)) {
                    int c = g->lines.count * 10 + 100;
                    Line* r = line_alloc(g, 101, "Random", (Color) { c, c, 255, 255 });

                    for (size_t i = 0; i < 101; i++) {
                        r->points[i] = (Vector2) { i, (rand() / (double)RAND_MAX) * 10 };
                    }
                }

                if (IsKeyPressed(KEY_D)) {
                    graph_line_pop(g);
                }

                if (IsKeyPressed(KEY_C)) {
                    graph_line_delete(g, "Circle");
                }

                if (IsKeyPressed(KEY_R)) {
                    graph_line_delete(g, "Random");
                }
            }

            graph_draw(g);
        }

        EndDrawing();
    }

    for (size_t i = 0; i < gs_count; i++) {
        graph_free(&gs[i]);
    }

    CloseWindow();

    return 0;
}

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "raylib.h"

int main(void)
{
    SetTargetFPS(144);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 800, "graph.c");

    Graph g = graph(25, RED, BLACK, DARKGRAY);

    g.st_reset.pos = (Vector2) { 135, 135 };
    g.st_reset.scale.z = 5.3;

    g.st_current = g.st_reset;

    Line* x = line_alloc(&g, 101, "X", RED, 6);
    Line* y = line_alloc(&g, 101, "Y", BLUE, 6);
    Line* l = line_alloc(&g, 101, "Line", GREEN, 12);
    Line* c = line_alloc(&g, 101, "Circle", YELLOW, 12);

    for (size_t i = 0; i < 101; i++) {
        x->points[i] = (Vector2) { i, 0.0 };
        y->points[i] = (Vector2) { 0.0, i };
        l->points[i] = (Vector2) { i, i * i / 100.0 };
        c->points[i] = (Vector2) {
            cos(i * PI * 2 / 100.0) * 25.0 + 50.0,
            sin(i * PI * 2 / 100.0) * 25.0 + 50.0,
        };
    }

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_Q)) {
            break;
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);

        int const margin = 0;
        g.bound = (Rectangle) {
            margin, margin,
            GetScreenWidth() - 2 * margin,
            GetScreenHeight() - 2 * margin
        };

        graph_update(&g);

        if (IsKeyPressed(KEY_A)) {
            int c = g.lines.count * 10 + 100;
            Line* r = line_alloc(&g, 101, "Random", (Color) { c, c, 255, 255 }, 6);

            for (size_t i = 0; i < 101; i++) {
                r->points[i] = (Vector2) { i, (rand() / (double)RAND_MAX) * 10 };
            }
        }

        if (IsKeyPressed(KEY_D)) {
            graph_line_pop(&g);
        }

        if (IsKeyPressed(KEY_C)) {
            graph_line_delete(&g, "Circle");
        }

        if (IsKeyPressed(KEY_R)) {
            graph_line_delete(&g, "Random");
        }

        graph_draw(&g);

        EndDrawing();
    }

    graph_free(&g);
    CloseWindow();

    return 0;
}

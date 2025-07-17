#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "raylib.h"
#include "raymath.h"

static void _graph_draw_line(Graph* g, Line* l);

Line* line_alloc(Graph* g, int count, const char* name, Color color)
{
    Line* l = malloc(sizeof(Line) + sizeof(Vector2) * count);
    assert(l != NULL);

    l->name = name;
    l->color = color;
    l->count = count;
    memset(l->points, 0, sizeof(Vector2) * count);

    if (g != NULL) {
        if (g->lines.count >= g->lines.capacity) {
            g->lines.capacity *= 2;
            g->lines.items = realloc(g->lines.items,
                sizeof(*g->lines.items) * g->lines.capacity);
        }
        g->lines.items[g->lines.count++] = l;
    }

    return l;
}

bool graph_line_delete(Graph* g, const char* name)
{
    assert(g != NULL);

    bool found = false;

    for (size_t i = 0; i < g->lines.count; i++) {
        Line* c = g->lines.items[i];
        if (found) {
            g->lines.items[i - 1] = c;
        } else if (!strcmp(c->name, name)) {
            found = true;
            free(c);
            g->lines.items[i] = NULL;
        }
    }

    if (found) {
        g->lines.count--;
    }

    return found;
}

bool graph_line_pop(Graph* g)
{
    assert(g != NULL);
    if (g->lines.count < 1)
        return false;

    g->lines.count--;

    Line* last = g->lines.items[g->lines.count];
    assert(last != NULL);

    free(last);
    g->lines.items[g->lines.count] = NULL;

    return true;
}

void graph_draw_relative_line(Graph* g, int type, int offset, Color color)
{
    float y = g->bound.height;
    switch (type) {
    case X_AXIS:
        DrawLine(
            g->bound.x, y - offset,
            g->bound.x + g->bound.width, y - offset,
            color);
        return;
    case Y_AXIS:
        if (offset > 0 && offset < g->bound.width)
            DrawLine(
                g->bound.x + offset, g->bound.y,
                g->bound.x + offset, g->bound.y + g->bound.height,
                color);
        return;
    default:
        assert(false && "Unreachable");
    }
}

void graph_draw_lines(Graph* g)
{
    for (size_t i = 0; i < g->lines.count; i++) {
        Line* l = g->lines.items[i];

        _graph_draw_line(g, l);

        if (!g->show_legend)
            continue;

        double y = g->bound.y + 7 + 25 * i;
        if (y < g->bound.y + g->bound.height - 20) {
            DrawRectangle(g->bound.x + 5, y, 20, 20, l->color);
            DrawText(l->name, g->bound.x + 28, y, 20, l->color);
        }
    }
}

static void _graph_draw_line(Graph* g, Line* line)
{
    Vector2 prev_point;

    bool continuous = true;

    for (size_t x = 0; x < line->count; x++) {
        Vector2 point = Vector2Scale(line->points[x], g->st_current.scale.z);
        point.x *= g->st_current.scale.x;
        point.y *= g->st_current.scale.y;

        point.x += g->bound.x + g->st_current.pos.x;
        point.y = g->bound.y + g->bound.height - point.y - g->st_current.pos.y;

        if (point.x < g->bound.x) {
            continuous = false;
            continue;
        } else if (point.x > g->bound.width + g->bound.x) {
            continuous = false;
            continue;
        }

        if (point.y < g->bound.y) {
            continuous = false;
            continue;
        } else if (point.y > g->bound.height + g->bound.y) {
            continuous = false;
            continue;
        }

        if (continuous && x > 0) {
            DrawLineEx(
                point,
                prev_point,
                2, line->color);
        } else {
            continuous = true;
        }
        prev_point = point;
    }
}

void graph_draw_line_value_at_mouse(Graph* g)
{
    Vector2 mpos = GetMousePosition();
    mpos.x -= g->bound.x + g->st_current.pos.x;
    mpos = Vector2Scale(mpos, 1.0 / g->st_current.scale.z);
    mpos.x *= 1.0 / g->st_current.scale.x;
    graph_draw_line_value_at_x(g, mpos.x);
}

void graph_draw_line_value_at_x(Graph* g, double x)
{
    for (size_t i = 0; i < g->lines.count; i++) {
        Line* line = g->lines.items[i];
        for (size_t j = 1; j < line->count; j++) {
            Vector2 point = line->points[j];
            Vector2 prev = line->points[j - 1];
            if (point.x - prev.x > 0) {
                if (x < point.x && x > prev.x) {
                    graph_draw_point(g, point, 6, line->color);
                }
            } else {
                if (x > point.x && x < prev.x) {
                    graph_draw_point(g, prev, 6, line->color);
                }
            }
        }
    }
}

void graph_draw_grid(Graph* g)
{
    double margin = g->grid_margin * g->st_current.scale.z;
    for (double x = fmod(g->st_current.pos.y, margin * g->st_current.scale.y); x < g->bound.height; x += margin * g->st_current.scale.y) {
        graph_draw_relative_line(g, X_AXIS, x - g->bound.y, g->c_grid);
    }
    for (double y = fmod(g->st_current.pos.x, margin * g->st_current.scale.x); y < g->bound.width; y += margin * g->st_current.scale.x) {
        graph_draw_relative_line(g, Y_AXIS, y, g->c_grid);
    }
}

void graph_draw_border(Graph* g)
{
    // border
    double border_width = 2;
    DrawRectangle(
        g->bound.x - border_width, g->bound.y - border_width,
        g->bound.width + 2 * border_width, g->bound.height + 2 * border_width,
        g->c_border);
    // graph
    DrawRectangleRec(g->bound, g->c_background);
}

void graph_draw_point(Graph* g, Vector2 point, int size, Color color)
{
    Vector2 real_coordinates = point;
    point = Vector2Scale(point, g->st_current.scale.z);
    point.x *= g->st_current.scale.x;
    point.y *= g->st_current.scale.y;

    point.x += g->bound.x + g->st_current.pos.x;
    point.y = g->bound.y + g->bound.height - point.y - g->st_current.pos.y;

    if (point.x < g->bound.x) {
        return;
    } else if (point.x > g->bound.width + g->bound.x) {
        return;
    }
    if (point.y < g->bound.y) {
        return;
    } else if (point.y > g->bound.height + g->bound.y) {
        return;
    }
    float selected_point;
    if (real_coordinates.y != 0) {
        selected_point = real_coordinates.y;
    } else {
        selected_point = real_coordinates.x;
    }
    DrawText(TextFormat("%.1lf", selected_point), point.x, point.y + 4, 20, color);
    DrawCircleV(point, size, color);
}

void graph_draw(Graph* g)
{
    graph_draw_border(g);
    graph_draw_grid(g);
    graph_draw_lines(g);

    if (IsKeyDown(KEY_TAB)) {
        graph_draw_line_value_at_mouse(g);
    }

    if (IsKeyPressed(KEY_P)) {
        graph_print_position(g);
    }
}

void graph_update(Graph* g)
{
    Vector2 mpos = GetMousePosition();
    double delta = GetFrameTime();

    if (CheckCollisionPointRec(mpos, g->bound)) {
        double zoom_speed = GetMouseWheelMove();
        if (zoom_speed != 0) {
            zoom_speed *= IsKeyDown(KEY_LEFT_SHIFT) ? 30.0 * g->st_current.scale.z : 20.0;
            graph_zoom(g, zoom_speed, delta);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 d = GetMouseDelta();

            g->st_current.pos.y -= d.y;
            g->st_current.pos.x += d.x;
        }

        if (IsKeyPressed(KEY_SPACE)) {
            g->st_current.pos = g->st_reset.pos;
            g->st_current.scale = g->st_reset.scale;
        }

        if (IsKeyPressed(KEY_L)) {
            g->show_legend = !g->show_legend;
        }
    }
}

void graph_zoom(Graph* g, double zoom, double delta)
{
    if (IsKeyDown(KEY_LEFT_CONTROL)) {
        g->st_current.scale.x += zoom * delta;
    } else if (IsKeyDown(KEY_LEFT_ALT)) {
        g->st_current.scale.y += zoom * delta;
    } else {
        g->st_current.scale.z += zoom * delta;
    }
    if (g->st_current.scale.y <= 0.01) {
        g->st_current.scale.y = 0.01;
    }
    if (g->st_current.scale.x <= 0.01) {
        g->st_current.scale.x = 0.01;
    }
    if (g->st_current.scale.z <= 0.01) {
        g->st_current.scale.z = 0.01;
    }
}
void graph_print_position(Graph* g)
{
    printf("Graph Current Stance:\n");
    printf("Pos = (x: %.2lf, y: %.2lf)\n", g->st_current.pos.x, g->st_current.pos.y);
    printf("Scale = (x: %.2lf, y: %.2lf, z: %.2lf)\n", g->st_current.scale.x, g->st_current.scale.y, g->st_current.scale.z);
}

void graph_free(Graph* g)
{
    for (size_t i = 0; i < g->lines.count; i++) {
        free(g->lines.items[i]);
    }
    free(g->lines.items);
}

Graph graph(double grid_margin, Color border, Color bg, Color grid)
{
    Graph g = { 0 };

    g.font = GetFontDefault();
    g.font_size = 24;

    g.show_legend = true;

    g.bound = (Rectangle) { 0 };

    g.st_reset.pos.x = 0;
    g.st_reset.pos.y = 0;
    g.st_reset.scale.z = 1.0;
    g.st_reset.scale.x = 1.0;
    g.st_reset.scale.y = 1.0;

    g.st_current = g.st_reset;

    g.grid_margin = grid_margin;
    assert(grid_margin > 0);

    g.c_border = border;
    g.c_grid = grid;
    g.c_background = bg;

    g.lines.count = 0;
    g.lines.capacity = 8;
    g.lines.items = malloc(sizeof(Line*) * g.lines.capacity);
    assert(g.lines.items != NULL);

    return g;
}

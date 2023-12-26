#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define TR_IMPLEMENTATION
#include "tr.h"

#define WIDTH 75
#define HEIGHT 16*3

static void vertex_shader_func(vertex_t vertex, v2f *position, void *out_data) {
    // pixel type
    *(char *)out_data = '+';
    *position = (v2f) {vertex.vertex.x, vertex.vertex.y};
}

static void fragment_shader_func(void *in_data, char *pixel_type) {
    *pixel_type = *(char *)in_data;
}

int main() {
    tr_t tr = tr_create(WIDTH, HEIGHT);
    tr_clear(&tr);

    vertex_t triangle[] = {
        {{-0.5f, -0.5f, 0.0f}, {0}},
        {{0.5f, -0.5f, 0.0f}, {0}},
        {{0.0f, 0.5f, 0.0f}, {0}}
    };

    shader_t basic_shader = {vertex_shader_func, fragment_shader_func};
    tr_set_shader(basic_shader);

    float timer = 0.0;
    while (1) {
        timer += 0.001;
        tr_draw_line(&tr, 0, 0, fabsf(cosf(timer/10)) * WIDTH, fabsf(sinf(timer/14)) * HEIGHT);
        // tr_draw_text(&tr, 10, 16*2, "hello", 5);
        tr_render(&tr);
        sleep(0.2);

        // Clear scr
        tr_clear(&tr);
    }
}
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/ioctl.h>

#define TR_IMPLEMENTATION
#include "tr.h"

static unsigned short winw = 0, winh = 0;

static void vertex_shader_func(shader_t *shader, void *in_vertex, v3f *position) {
    // pixel value data
    *(char *)shader->pipe_data = '+';
    
    v3f v = *(v3f *)in_vertex;
    *position = (v3f) {(v.x + 1) * (winw/2), (v.y + 1) * (winh/2), 0.0f};
}

static void fragment_shader_func(shader_t *shader, char *color) {
    *color = *(char *)shader->pipe_data;
}

int main() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    tr_t tr = tr_create(w.ws_col, w.ws_row);

    float triangle[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

    vertex_buffer_t triangle_vtx_buffer = {
        .size = 3,
        .verticies = triangle
    };

    uint32_t indices[] = {0, 1, 2};
    index_buffer_t index_buffer = {
        .indices = indices,
        .size = sizeof(indices) / sizeof(uint32_t)
    };
    
    vertex_attrib_buffer_t vtx_attrib_buffer = {
        .attribs = (vertex_attrib_t[]) {
            tr_make_vertex_attrib(3, TR_FLOAT)
        },
        .size = 1
    };

    tr_make_shader(vertex_shader_func, fragment_shader_func, vtx_attrib_buffer, sizeof(char));

    tr_set_clear_color(&tr, '#');

    unsigned short prevw, prevh;

    char winsize_str[5];

    float timer = 0.0;
    while (1) {
        // printf("%i %i\n", tr.width, tr.height);
        timer += 0.001; 
        tr_draw_mesh(&tr, index_buffer, triangle_vtx_buffer, false);
        sprintf(winsize_str, "%i %i", tr.width, tr.height);
        tr_draw_text(&tr, winsize_str, 2, 2, sizeof(winsize_str));
        // tr_draw_line(&tr, 0, 0, fabsf(cosf(timer/10)) * WIDTH, fabsf(sinf(timer/14)) * HEIGHT);
        // tr_draw_text(&tr, "hello", 10, 16*2, 5);
        tr_render(&tr);
        sleep(0.2);

        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        if (prevw != w.ws_col || prevh != w.ws_row) {
            tr = tr_create(w.ws_col, w.ws_row);
            winw = w.ws_col;
            winh = w.ws_row;
            tr_set_clear_color(&tr, '#');
        }
        prevw = w.ws_col;
        prevh = w.ws_row;


        // Clear scr
        tr_clear(&tr);
    }
}
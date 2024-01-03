#ifndef TERMINAL_RENDERER_H
#define TERMINAL_RENDERER_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>

///                           ///
/// Terminal Renderer Library ///
///                           ///

#define xytoi(x, y, width) (((y) * (width)) + (x))
#define ASSERT(_e, ...) if (!(_e)) { fprintf(stderr, __VA_ARGS__); exit(1); }

typedef struct {
    float x, y;
} v2f;

typedef struct {
    float x, y, z;
} v3f;

typedef struct {
    float x, y, z, w;
} v4f;

typedef struct {
    size_t width;
    size_t height;

    char clear_color;

    char *framebuffer;
    char *printbuffer;

    float *depth_buffer;
} tr_t;

typedef enum {
    TR_BYTE = sizeof(int8_t),
    TR_UBYTE = sizeof(uint8_t),
    TR_SHORT = sizeof(int16_t),
    TR_USHORT = sizeof(uint16_t),
    TR_INT = sizeof(int32_t),
    TR_UINT = sizeof(uint32_t),
    TR_INT64 = sizeof(int64_t),
    TR_UINT64 = sizeof(uint64_t),
    TR_FLOAT = sizeof(float),
    TR_DOUBLE = sizeof(double),
} attrib_type;

typedef struct {
    uint8_t size;
    attrib_type type;
} vertex_attrib_t;

typedef struct {
    void *verticies;
    size_t size;
} vertex_buffer_t;

typedef struct {
    vertex_attrib_t *attribs;
    size_t size;
} vertex_attrib_buffer_t;

typedef struct {
    uint32_t *indices;
    size_t size;
} index_buffer_t;

typedef struct shader_t shader_t;

typedef void (*vertex_program_func_t)(shader_t *shader, void *in_vertex, v3f *position);
typedef void (*fragment_program_func_t)(shader_t *shader, char *color);

/**
 * Holds user-defined vertex and fragment shader function pointers.
 * 
*/
struct shader_t {
    vertex_program_func_t vertex_program;
    fragment_program_func_t fragment_program; 
    vertex_attrib_buffer_t vertex_attrib_buffer;
    void *pipe_data;
    size_t vertex_attrib_buffer_size;
};

/// Global renderer state ///

// Global shading program being used by the renderer
static shader_t _shader = {0};

static const char *character_grayscale_ramp = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";

tr_t tr_create(size_t width, size_t height);
void tr_render(tr_t *tr);
void tr_clear(tr_t *tr);
void tr_set_clear_color(tr_t *tr, char color);
void tr_draw_text(tr_t *tr, char *text, size_t x, size_t y, size_t len);
void tr_draw_line(tr_t *tr, size_t x0, size_t y0, size_t x1, size_t y1);
void tr_draw_mesh(tr_t *tr, index_buffer_t indices, vertex_buffer_t verticies, bool wireframe);
void tr_make_shader(vertex_program_func_t vs, fragment_program_func_t fs, vertex_attrib_buffer_t attrib_buffer, size_t pipe_size);
void tr_set_shader(shader_t shader);
shader_t *tr_get_shader();
void tr_set_vertex_attribute_buffer(vertex_attrib_buffer_t buffer);
vertex_attrib_buffer_t tr_get_vertex_attribute_buffer();
vertex_attrib_t tr_make_vertex_attrib(uint8_t size, attrib_type type);

// v3f tr_shader_transform_ndc_to_viewport()

static void setc(tr_t *tr, size_t x, size_t y, char c);

#ifdef TR_IMPLEMENTATION

tr_t tr_create(size_t width, size_t height) {
    char *framebuffer = (char *) malloc(width * height);
    memset(framebuffer, ' ', width * height);

    char *depth_buffer = (char *) malloc(sizeof(float) * width * height);
    for (int i = 0; i < width * height; i++) {
        depth_buffer[i] = 1.0f;
    }

    return (tr_t) {
        .width = width,
        .height = height,
        .framebuffer = framebuffer,
        .printbuffer = (char *) malloc((width * height) + height + 1),
        .depth_buffer = depth_buffer,
        .clear_color = ' '
    };
}

static void setc(tr_t *tr, size_t x, size_t y, char c) {
    if (x >= tr->width && y >= tr->height)
        return;

    tr->framebuffer[xytoi(x, y, tr->width)] = c;
}

void tr_set_clear_color(tr_t *tr, char color) {
    tr->clear_color = color;
}

void tr_clear(tr_t *tr) {
    memset(tr->framebuffer, tr->clear_color, tr->width * tr->height);
}

void tr_render(tr_t *tr) {
    int i = 0;
    for (int y = 0; y < tr->height; y++) {
        for (int x = 0; x < tr->width; x++) {
            tr->printbuffer[i++] = tr->framebuffer[xytoi(x, y, tr->width)];
            if (x == tr->width - 1) {
                tr->printbuffer[i++] = '\n';
            }
        }
    }

    tr->printbuffer[i] = '\0';

    printf("%s\n", tr->printbuffer);
}

void tr_draw_line(tr_t *tr, size_t x0, size_t y0, size_t x1, size_t y1) {
    if (x0 >= tr->width && y0 >= tr->height &&
        x1 >= tr->width && y1 >= tr->height) {
        return;
    }

    bool steep = false;
    int tmp;
    if (abs(x0-x1)<abs(y0-y1)) { 
        tmp = x0;
        x0 = y0;
        y0 = tmp;
        tmp = x1;
        x1 = y1;
        y1 = tmp;
        steep = true; 
    } 
    if (x0>x1) { 
        tmp = x0;
        x0 = x1;
        x1 = tmp;
        tmp = y0;
        y0 = y1;
        y1 = tmp;
    } 
    int dx = x1-x0; 
    int dy = y1-y0; 
    float derror = fabsf(dy/(float)dx); 
    float error = 0; 
    int y = y0; 
    for (int x=x0; x<=x1; x++) { 
        if (steep) { 
            setc(tr, y, x, '0');
        } else { 
            setc(tr, x, y, '0');
        } 
        error += derror; 
        if (error>0.5) { 
            y += (y1>y0?1:-1); 
            error -= 1.; 
        } 
    } 
}

void tr_draw_mesh(tr_t *tr, index_buffer_t indices, vertex_buffer_t verticies, bool wireframe) {
    for (int i = 0; i < indices.size; i++) {
        ASSERT(i < verticies.size, "index buffer index out of bounds\n");

        v3f vpos;
        _shader.vertex_program(&_shader, verticies.verticies + (_shader.vertex_attrib_buffer_size * indices.indices[i]), &vpos);

        char color;
        _shader.fragment_program(&_shader, &color);

        // printf("%f %f\n", vpos.x, vpos.y);
        setc(tr, vpos.x, vpos.y, color);
    }
}


void tr_draw_text(tr_t *tr, char *text, size_t x, size_t y, size_t len) {
    if (x >= tr->width && y >= tr->height)
        return;

    size_t idx = xytoi(x, y, tr->width);
    memcpy(&tr->framebuffer[idx], text, len > (tr->width * tr->height) - idx ? (tr->width * tr->height) - idx : len);
}

void tr_make_shader(vertex_program_func_t vs, fragment_program_func_t fs, vertex_attrib_buffer_t attrib_buffer, size_t pipe_size) {
    size_t size = 0;

    for (int i = 0; i < attrib_buffer.size; i++) {
        size += attrib_buffer.attribs[i].type * attrib_buffer.attribs[i].size;
    }
    
    _shader = (shader_t) {
        .vertex_program = vs,
        .fragment_program = fs,
        .vertex_attrib_buffer = attrib_buffer,
        .vertex_attrib_buffer_size = size,
        .pipe_data = malloc(pipe_size),
    };
}

void tr_set_shader(shader_t shader) {
    _shader = shader;
}

shader_t *tr_get_shader() {
    return &_shader;
}

vertex_attrib_t tr_make_vertex_attrib(uint8_t size, attrib_type type) {
    return (vertex_attrib_t) {
        .size = size,
        .type = type
    };
}

#endif // TR_IMPLEMENTATION

#endif // TERMINAL_RENDERER_H
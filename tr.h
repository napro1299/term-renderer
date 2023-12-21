#ifndef TERMINAL_RENDERER_H
#define TERMINAL_RENDERER_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

///                           ///
/// Terminal Renderer Library ///
///                           ///

#define xytoi(x, y, width) (((y) * (width)) + (x))
#define ASSERT(_e, ...) if (!(_e)) { fprintf(stderr, __VA_ARGS__); exit(1); }

#define ASSERT_BOUNDS(x, y, width, height) ASSERT(x <= width - 1 && y <= height - 1, "Draw out of bounds")

typedef struct {
    size_t width;
    size_t height;

    char *framebuffer;
    char *printbuffer;
} tr_t;

tr_t tr_create(size_t width, size_t height);
void tr_render(tr_t *tr);
void tr_clear(tr_t *tr);
void tr_draw_line(tr_t *tr, size_t x0, size_t y0, size_t x1, size_t y1);
void tr_draw_text(tr_t *tr, size_t x, size_t y, char *text, size_t len);

static void setc(tr_t *tr, size_t x, size_t y, char c);

#ifdef TR_IMPLEMENTATION

tr_t tr_create(size_t width, size_t height) {
    return (tr_t) {
        .width = width,
        .height = height,
        .framebuffer = (char *) calloc(width * height, 1),
        .printbuffer = (char *) malloc((width * height) + height + 1)
    };
}

static void setc(tr_t *tr, size_t x, size_t y, char c) {
    ASSERT_BOUNDS(x, y, tr->width, tr->height);
    tr->framebuffer[xytoi(x, y, tr->width)] = c;
}

void tr_clear(tr_t *tr) {
    memset(tr->framebuffer, ' ', tr->width * tr->height);
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

void tr_draw_text(tr_t *tr, size_t x, size_t y, char *text, size_t len) {
    ASSERT_BOUNDS(x, y, tr->width, tr->height);
    size_t idx = xytoi(x, y, tr->width);
    memcpy(&tr->framebuffer[idx], text, len > (tr->width * tr->height) - idx ? (tr->width * tr->height) - idx : len);
}

#endif // TR_IMPLEMENTATION

#endif // TERMINAL_RENDERER_H
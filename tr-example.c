#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define TR_IMPLEMENTATION
#include "tr.h"

#define WIDTH 75
#define HEIGHT 16*3

int main() {
    tr_t tr = tr_create(WIDTH, HEIGHT);
    tr_clear(&tr);

    float timer = 0.0;
    while (1) {
        timer += 0.001;
        tr_draw_line(&tr, 0, 0, fabsf(cosf(timer/10)) * WIDTH, fabsf(sinf(timer/14)) * HEIGHT);
        tr_draw_text(&tr, 10, 16*2, "hello", 5);
        tr_render(&tr);
        sleep(0.2);

        // Clear scr
        tr_clear(&tr);
    }
}
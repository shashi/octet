#ifndef _CUBE_H
#define _CUBE_H

#include "cube.h"

#include "stdlib.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>


#define delay_ms(x) usleep((x) * 1000)

char cube[8][8][8];

void set_led(int x, int y, int z, int level) {
    cube[x][y][z] = level;
}

int get_led(int x, int y, int z) {
    return cube[x][y][z];
}
/*
#define CUBE_SIZE 8
#define BITS_PER_LED 2
#define Z_MAX CUBE_SIZE * BITS_PER_LED / sizeof(char)

char cube[CUBE_SIZE][CUBE_SIZE][Z_MAX];

void set_led(int x, int y, int z, int level) {
    // special case here
    int k = (z >> 2) & 1, bits_pos = (z & 3) << 1;

    cube[x][y][k] |= 3 << bits_pos;
    cube[x][y][k] &= level << bits_pos;
}

int get_led(int x, int y, int z) {
    int k = (z >> 2) & 1, bit_pos = (z & 3) << 1;
    return (cube[x][y][k] >> bit_pos) & 3;
}
*/

// XXX: remove this.
void print_cube() {
    int x, y, z;
    for (x=0; x < 8; x++) {
        printf("x=%d\n---\n", x);
        for(y=0; y < 8; y++) {
            for(z=0; z < 8; z++) {
                printf("%d ", get_led(x, y, z));
            }
            printf("\n");
        }
    }
}

void set_plane(int axis, int plane, int level) {
    int i, j;

    switch (axis) {
    case X_AXIS:
        for (i=0; i < 8; i++) {
            for (j=0; j < 8; j++) {
                set_led(plane, i, j, level);
            }
        }
        break;
    case Y_AXIS:
        for (i=0; i < 8; i++) {
            for (j=0; j < 8; j++) {
                set_led(i, plane, j, level);
            }
        }
        break;
    case Z_AXIS:
        for (i=0; i < 8; i++) {
            for (j=0; j < 8; j++) {
                set_led(i, j, plane, level);
            }
        }
        break;
    }
}

void plane_bounce(int axis, int delay) {
    int prev_1=1, prev_2=1,prev_3=1, i;
    for (i=0; i < 8; i++) {
        set_plane(axis, i, 3);
        //set_plane(axis, prev_1, 2);
        //set_plane(axis, prev_2, 1);
        set_plane(axis, prev_3, 0);

        prev_3 = i;
        //prev_3 = prev_2;
        //prev_2 = prev_1;
        //prev_1 = i;
        delay_ms(delay);
    }

    for (i=7; i >= 0; i--) {
        set_plane(axis, i, 3);
        //set_plane(axis, prev_1, 2);
        //set_plane(axis, prev_2, 1);
        set_plane(axis, prev_3, 0);

        prev_3 = i;
        //prev_3 = prev_2;
        //prev_2 = prev_1;
        //prev_1 = i;
        delay_ms(delay);
    }
}

void fill_cube(char c) {
    int i, j, k;
    for (i=0; i < 8; i++)
        for(j=0; j < 8; j++)
            for(k=0; k < 8; k++)
            { cube[i][j][k] = c;}
}

void sine_wave(int delay) {
    int i, x, y, z; double angle;
    for (i=0; i<8; i++) {
        for (x=0; x < 8; x++) {
            for (y=0; y < 8; y++) {
                angle = 3.14 / 4 * i / 7.0 * (sqrt(pow(x-3.5, 2) + pow(y - 3.5, 2)) / 3.5 * sqrt(2));
                z = round((8) * sin(angle)) + i;
                set_led(x, y, z, 3);
            }
        }
        delay_ms(delay);
        fill_cube(0x00);
    }

    for (i=7; i>=0; i--) {
        for (x=0; x < 8; x++) {
            for (y=0; y < 8; y++) {
                angle = 3.14 / 4 * i / 7.0 * (sqrt(pow(x-3.5, 2) + pow(y - 3.5, 2)) / 3.5 * sqrt(2));
                z = round(8*sin(angle)) + 7 - i;
                set_led(x, y, z, 3);
            }
        }
        delay_ms(delay);
        fill_cube(0x00);
    }
}

void wireframe_cube(int x, int y, int z, int edge, int level) {
    int i;
    edge--;
    for (i=0; i <= edge; i++) {
        set_led(x + i, y, z, level);
        set_led(x + i, y + edge, z, level);
        set_led(x + i, y, z + edge, level);
        set_led(x + i, y + edge, z + edge, level);

        set_led(x, y + i, z, level);
        set_led(x + edge, y + i, z, level);
        set_led(x + edge, y + i, z + edge, level);
        set_led(x, y + i, z + edge, level);

        set_led(x, y, z + i, level);
        set_led(x + edge, y, z + i, level);
        set_led(x + edge, y + edge, z + i, level);
        set_led(x, y + edge, z + i, level);
    }
}

void cube_dance(int delay) {
    int i;
    for(i=0; i< 8; i++) {
        wireframe_cube(0, 0, 0, i, 3);
        delay_ms(delay);
        wireframe_cube(0, 0, 0, i, 0);
    }

    for(i=0; i< 8; i++) {
        wireframe_cube(i, i, i, 8 - i, 3);
        delay_ms(delay);
        wireframe_cube(i, i, i, 8 - i, 0);
    }
}

void * cube_main(void * arg) {
    /*
     * This function is called as a separate thread,
     * here we call functions that generate patterns
     * and store them in the cube. Parallelly, the screen
     * keeps getting redrawn by the opengl code with the
     * pattern stored in memory. There can be cases where
     * the cube is read for rendering when it is being
     * written, this is okay since the refresh rate is high
     * and the anomalies escape the human eye.
     *
     * The code in this file will also be burnt onto the
     * microcontroller, we will make sure later on that this
     * will occur without any porting whatsoever.
     */

    while(1) {
        cube_dance(100);
        //plane_bounce(X_AXIS, 20);
        //sine_wave(100);
    }

    return NULL;
}

#endif

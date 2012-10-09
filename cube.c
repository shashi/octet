#include "stdlib.h"

#define CUBE_SIZE 8
#define BITS_PER_LED 2
#define Z_MAX CUBE_SIZE * BITS_PER_LED / sizeof(char)

char cube[CUBE_SIZE][CUBE_SIZE][Z_MAX];

void setvoxel(int x, int y, int z, int level) {
    // special case here
    int k = z >> 2;
    cube[x][y][k & 1] |= level << ((z & 3) << 1);
}

int getvoxel(int x, int y, int z) {
    int k = z >> 2;
    return (cube[x][y][k & 1] >> ((z & 3) << 1)) & 3;
}


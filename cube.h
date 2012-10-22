#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

void set_led(int x, int y, int z, int level);
int get_led(int x, int y, int z);
void set_plane(int axis, int plane, int level);
void fill_cube(char val);
void call_function(char f, char args[5]);
void * cube_main(void *);

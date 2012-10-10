#include <GL/glut.h>
#include <pthread.h>

#include "cube.h"
#include "opengl.h"

int main (int argc, char* argv[]) {
    glutInit(&argc,argv);

    pthread_t opengl_thread, cube_thread;
    int v=0;
    void * exit_status;

    pthread_create(&opengl_thread, NULL, opengl_init, &v);
    pthread_create(&cube_thread, NULL, cube_main, &v);

    pthread_join(opengl_thread, &exit_status);
    pthread_join(cube_thread, &exit_status);

    return 0;
}

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "cube.h"

#define SPACING .1
 
// ----------------------------------------------------------
// Function Prototypes
// ----------------------------------------------------------
void display();
void specialKeys();
 
// ----------------------------------------------------------
// Global Variables
// ----------------------------------------------------------
double rotate_y=0; 
double rotate_x=0;

// ----------------------------------------------------------
// display() Callback function
// ----------------------------------------------------------
void display(){

  int x, y, z, lvl;

  GLfloat LightAmbient[] = { 0.8, 0.0, 0.0, 1.0 };  /* reddish ambient light  */
  GLfloat LightDiffuse[] = { 0.1, 0.1, 0.9, 1.0 };  /* bluish  diffuse light. */
  GLfloat LightPosition[] = { 0.2, 0.2, 0.2, 0.0 };    /* position */

  //  Clear screen and Z-buffer
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 
  // Reset transformations
  glLoadIdentity();
  glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);  /*  add lighting. (ambient) */
  glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);  /*  add lighting. (diffuse). */
  glLightfv(GL_LIGHT0, GL_POSITION,LightPosition); /*  set light position. */
  glEnable(GL_LIGHT0);                      /*  turn light 0 on. */
 
  // Other Transformations
  // glTranslatef( 0.1, 0.0, 0.0 );      // Not included
  // glRotatef( 180, 0.0, 1.0, 0.0 );    // Not included
 
  // Rotate when user changes rotate_x and rotate_y
  glRotatef( rotate_x, 1.0, 0.0, 0.0 );
  glRotatef( rotate_y, 0.0, 1.0, 0.0 );
 
  // Other Transformations
  // glScalef( 2.0, 2.0, 0.0 );          // Not included
  glPointSize(10.0); // 10 pixel dot!
 
  //Multi-colored side - FRONT
  //set_plane(X_AXIS, 0, 3);
  //set_plane(Z_AXIS, 3, 3);

  for(x=0; x < 8; x++) {
    for(y=0; y < 8; y++) {
      for(z=0; z < 8; z++) {
          lvl = get_led(x, y, z);
          glPushMatrix();
          glTranslatef(SPACING * (x - 4), SPACING * (y - 4), -SPACING * (z - 4));
          if (lvl > 0) {
              glColor3f(lvl / 3.0, 0, 0);
              glutSolidCube(0.03);
          }
          glPopMatrix();
      }
    }
  }
  glFlush();
  glutSwapBuffers();

}
 
// ----------------------------------------------------------
// specialKeys() Callback Function
// ----------------------------------------------------------
void specialKeys( int key, int x, int y ) {
 
  //  Right arrow - increase rotation by 5 degree
  if (key == GLUT_KEY_RIGHT)
    rotate_y += 5;
 
  //  Left arrow - decrease rotation by 5 degree
  else if (key == GLUT_KEY_LEFT)
    rotate_y -= 5;
 
  else if (key == GLUT_KEY_UP)
    rotate_x += 5;
 
  else if (key == GLUT_KEY_DOWN)
    rotate_x -= 5;
 
  //  Request display update
  glutPostRedisplay();
 
}
 
// ----------------------------------------------------------
// main() function
// ----------------------------------------------------------
void * opengl_init(void * arg){

  //  Request double buffered true color window with Z-buffer
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
 
  glutInitWindowSize(800, 800);
  // Create window
  glutCreateWindow("Awesome Cube");
 
  //  Enable Z-buffer depth test
  glEnable(GL_DEPTH_TEST);
  // Callback functions
  glutDisplayFunc(display);
  glutIdleFunc(display);
  glutSpecialFunc(specialKeys);
 
  //  Pass control to GLUT for events
  glutMainLoop();
  return NULL;
}

#ifndef Go_AR_drawingutils_H
#define Go_AR_drawingutils_H
//opengl
#include <GL/glew.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
#include <math.h> 
#define BLACK 0
#define WHITE 1

void drawGoStone(GLfloat a, GLfloat b, GLfloat c, GLint nSlice, GLint nStack, float* origin, int color);
void DrawPoint(float* origin,int size);

void drawCoordinateAxis();
void buildProjectionMatrix(float* m, float intrinsic_array[][3], int screen_width, int screen_height);
//  printf prints to file. printw prints to window
void printw (float x, float y, float z, char* format, ...);

#endif
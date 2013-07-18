#ifndef Go_AR_drawingutils_H
#define Go_AR_drawingutils_H
//opengl
/*
#include <GL/glew.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
*/
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <math.h> 
#include "../GamePlayUtils/GameConstant.h"
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

static std::string floatToString (float number){
      std::ostringstream buff;
	  buff << std::fixed << std::setprecision( 2 ) << number;

      return buff.str();

      }
/*
std::string intToString (int number){
      std::ostringstream buff;
      buff<<number;
      return buff.str();

      }
	  */
static const float HALF_TRAN_RED_COLOR[]		= {1.0f, 0.0f, 0.0f, 0.8f};
static const float HALF_TRAN_GREEN_COLOR[]	= {0.0f, 1.0f, 0.0f, 0.5f};
static const float HALF_TRAN_BLUE_COLOR[]		= {0.0f, 0.0f, 1.0f, 0.5f};
static const float HALF_TRAN_BLACK_COLOR[]		= {0.0f, 0.0f, 0.0f, 0.5f};
static const float HALF_TRAN_WHITE_COLOR[]		= {1.0f, 1.0f, 1.0f, 0.5f};

static const float RED_COLOR[]		= {1.0f, 0.0f, 0.0f, 0.75f};
static const float SOLID_RED_COLOR[]		= {1.0f, 0.0f, 0.0f, 1.0f};
static const float GREEN_COLOR[]		= {0.0f, 1.0f, 0.0f, 0.75f};
static const float BLUE_COLOR[]		= {0.0f, 0.0f, 1.0f, 0.75f};

static const float BLACK_COLOR[]		= {0.0f, 0.0f, 0.0f, 0.75f};
static const float WHITE_COLOR[]		= {1.0f, 1.0f, 1.0f, 0.75f};

void drawGoStone(GLfloat a, GLfloat b, GLfloat c, GLint nSlice, GLint nStack, float* origin, int color);
void DrawPoint(float* origin,float size, const float* color);
void DrawSquare(float* origin, float size, const float* color);

void drawCoordinateAxis();
void buildProjectionMatrix(float* m, float intrinsic_array[][3], int screen_width, int screen_height);
//  printf prints to file. printw prints to window
void printw (float x, float y, float z, char* format, ...);

void draw_circle(float x, float y, float radius, const float* color);
void draw_text(float x, float y, const float* color, std::string msg);
#endif
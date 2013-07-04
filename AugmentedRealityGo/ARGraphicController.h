#ifndef ARGraphicController_H
#define ARGraphicController_H

////////////////////////////////////////////////////////////////////
// Video Image PSNR and SSIM
#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion

//opencv
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/calib3d/calib3d.hpp>

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

//self library
#include "BoardTracking/GoBoardDetector.h"
#include "GoARdrawingutils.h"
#include "GamePlayUtils/GoBoard.h"
#include "GamePlayUtils/FuegoAssistant.h"
#include "GamePlayUtils/GameConstant.h"
/**
	Responsible for main OpenGL functions. display augmented reality graphics
*/

class ARGraphicController
{
public:
	ARGraphicController(int sw, int sh, GoBoard* b, FuegoAssistant* f);
	~ARGraphicController();
	
	static void calculateFPS();
	void start(int argc, char *argv[]);
	static void drawBackGround();
	static void drawBoard();
	static void RenderSceneCB();
	static void gl_idle_func();
	static void init();
	static void keyFunc(unsigned char key, int x, int y);

	//static volatile char board[361];
	static volatile bool genMove;
	static volatile int newMoveColor;
	static GoBoard* board;
	static FuegoAssistant* fuego;
	//volatile char board[361];
	static void changeGen(int c);
private:
	int screen_width, screen_height;
	static int assistant_mode;
	
};
#endif
	
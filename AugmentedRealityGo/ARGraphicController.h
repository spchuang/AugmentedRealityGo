#ifndef ARGraphicController_H
#define ARGraphicController_H

////////////////////////////////////////////////////////////////////
// Video Image PSNR and SSIM
#include <iostream> // for standard I/O
#include <string>   // for strings

//opencv
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/calib3d/calib3d.hpp>

//opengl
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

//self library
#include "GamePlayUtils/GameConstant.h"
#include "BoardTracking/GoBoardDetector.h"
#include "HelperClass/config.h"
#include "HelperClass/GoARdrawingutils.h"
#include "GamePlayUtils/GoBoard.h"
#include "GamePlayUtils/GoAssistantController.h"


/**
	Responsible for main OpenGL functions. display augmented reality graphics
*/

class ARGraphicController
{
public:
	ARGraphicController(Config* c, GoBoard* b, GoAssistantController* ass);
	~ARGraphicController();
	
	static void calculateFPS();
	void start(int argc, char *argv[]);
	//draw background image from openCV
	static void drawBackGround();
	//draw the board stones and other assistance graphics in perspective of the camera
	static void drawBoard();
	//main openGL drawing method
	static void RenderSceneCB();
	//reads image from camera and run board detector
	static void gl_idle_func();
	static void init();
	static void keyFunc(unsigned char key, int x, int y);

	static void changeGen(int c);
	static volatile bool genMove;
	static GoBoard* board;
	static GoAssistantController* goAssistant;
	static Config* config;
	static GoBoardDetector* d;
	
private:
	static int loadingMsg;
	static volatile int newMoveColor;
	static int assistant_mode;

	
	
};
#endif
	
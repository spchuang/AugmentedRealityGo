#ifndef ARGoController_H
#define ARGoController_H

////////////////////////////////////////////////////////////////////
//opencv
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/calib3d/calib3d.hpp>

#include <boost/thread.hpp>

#include "ARGraphicController.h"
#include "GamePlayUtils/GoBoard.h"
#include "GamePlayUtils/FuegoAssistant.h"

/**
	control the game status, methods switch, ar and vision 
*/

class ARGoController
{
public:
	ARGoController(int sw, int sh);
	~ARGoController();
	void startAR();
  
private:

	ARGraphicController graphic_controller;
	boost::thread* graphic_handler;
	static GoBoard board;
	static FuegoAssistant fuego;

};
	
#endif
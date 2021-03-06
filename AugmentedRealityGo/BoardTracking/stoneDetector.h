#ifndef StoneDetector_H
#define StoneDetector_H

////////////////////////////////////////////////////////////////////
// Standard includes:
#include <vector>
#include <opencv2/opencv.hpp>
#include "CVHelper.h"
#include "../GamePlayUtils/GameConstant.h"
////////////////////////////////////////////////////////////////////

/*
This class is responsible for detecting the stones presented on the Go Board
*/

class StoneDetector
{
public:
	StoneDetector(int w_thresh, int b_thresh, int dist);
	//this creates a bar to see which threhold is the best
	void testThrehold();
	void setupDetection(cv::Mat b, std::vector<cv::Point2f> sp);
	void readStones(char BoardStonesStates[361]);
	void displayStoneDetection(std::vector<cv::Point2f> b, std::vector<cv::Point2f> w, char  bss[361]);


	void findCandidateStones(bool findBlack,std::vector<cv::Point2f> &potentialStoneCenters, char BoardStonesStates[361]);
private:
	cv::Mat boardImage;
	cv::Mat greyBoardImage;
	std::vector<cv::Point2f> StonePoints;

	void drawGrid(cv::Mat input, cv::Mat& output, cv::Scalar color);
	int white_thresh;
	int black_thresh;
	int min_radius;
	int max_radius;
	int max_dist;
	int border;
	
};

#endif
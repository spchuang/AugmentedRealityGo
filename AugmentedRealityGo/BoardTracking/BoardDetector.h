#ifndef BoardDetector_H
#define BoardDetector_H

////////////////////////////////////////////////////////////////////
// Standard includes:
#include <vector>
#include <opencv2/opencv.hpp>

////////////////////////////////////////////////////////////////////
// Forward declaration:
//class MarkerDetector;


/**
* A top-level class that encapsulate marker detector algorithm
*/

class BoardDetector
{
public:

	//constructor
	BoardDetector(cv::Mat camM, cv::Mat camD);	
	
	//! Main marker detection routine
	//bool detectMove(cv::Mat& srcImage);

	//void performGrayscale(const cv::Mat& srcImg, cv::Mat& preparedImg) const;
    
protected:
	bool findBoard(cv::Mat& srcImage);
	void undistortBoard();


private:

	std::vector<cv::Point3f> Board3DPoint;
	std::vector<cv::Point2f> BoardImagePoint;
	std::vector<cv::Point2f> UnidistortBoardPoint;

	//true = black
	//false = white
	std::vector<bool>  BoardStones;

	
	std::vector<aruco::Marker> m_detectedMarkers;
	cv::Mat m_grayscaleImage;
	cv::Mat undistortBoardImage;
	cv::Mat undistortImage;
	//MarkerDetector m_md;

	//board parameter
	int numPoint;
	double markerLength;
	double widthGap;
	double heightGap;
	double boardHeight;
	double boardWidth;
	double boardLength;
	double gap;
	double widthInterval;
	double heightInterval;
	double blockLength;

	cv::Mat camMatrix;
	cv::Mat distCoeff;
};

#endif
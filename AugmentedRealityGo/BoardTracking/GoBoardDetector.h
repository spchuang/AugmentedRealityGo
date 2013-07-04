#ifndef GoBoardDetector_H
#define GoBoardDetector_H

////////////////////////////////////////////////////////////////////
// Standard includes:
#include <vector>
#include <opencv2/opencv.hpp>
#include "Marker.h"
#include "stoneDetector.h"
//#include "aruco-1.2.4/aruco.h"

////////////////////////////////////////////////////////////////////
// Forward declaration:



/**
* A top-level class that encapsulate marker detector algorithm
*/

class GoBoardDetector
{
public:
	//make board properties public
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

	std::vector<cv::Point3f> Board3DPoint;

	//camera world position transformation 
	cv::Mat GoBoardRaux,GoBoardTaux;
	//constructor
	GoBoardDetector(cv::Mat camM, cv::Mat camD);
	void setCameraIntrinsics(cv::Mat camM, cv::Mat camD);

	cv::Mat getUndistortImage(cv::Mat& srcImage);
	bool detectMove();
	bool calculateCameraIntrinsix();
	void changePostMethod();
	void readStone(char  new_BoardStonesStates[361]);
	void detectOcclusionObject();
	void saveBackGroundBoard();
	void testThrehold();
protected:
	bool findBoard(cv::Mat& srcImage);
	void undistortBoard();
private:
	std::vector<Marker> m_detectedMarkers;
	std::vector<int> boardMarkerID;
	
	//points of board on original image
	std::vector<cv::Point2f> BoardImagePoint;
	//points of board in the unwraped image
	std::vector<cv::Point2f> UnidistortBoardPoint;

	bool detectBoard;
	//0 = black
	//1 = white
	//2 = none

	StoneDetector sd;

	cv::Mat m_grayscaleImage;
	cv::Mat undistortBoardImage;
	cv::Mat undistortImage;
	cv::Mat srcFrame;
	
	
	//variables for occlusion object detection (ex. hand)

	cv::Mat backGroundBoard;
	bool detectOcclusion;
	//MarkerDetector m_md;
	cv::Mat camMatrix;
	cv::Mat distCoeff;
	
	//we have two ways of finding camera position relative to the models
	enum PoseEstimationMethods {SOLVEPNP,RPP};
	PoseEstimationMethods m_PoseMethod;
};

#endif
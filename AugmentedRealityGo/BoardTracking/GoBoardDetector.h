#ifndef GoBoardDetector_H
#define GoBoardDetector_H

////////////////////////////////////////////////////////////////////
// Standard includes:
#include <vector>
#include <opencv2/opencv.hpp>
#include "Marker.h"
#include "stoneDetector.h"
#include "../HelperClass/config.h"
//#include "aruco-1.2.4/aruco.h"

////////////////////////////////////////////////////////////////////
// Forward declaration:



/**
* A top-level class that encapsulate marker detector algorithm
*/

class GoBoardDetector
{
public:
	//constructor
	GoBoardDetector(Config* c);

	std::vector<cv::Point3f> Board3DPoint;

	//camera world position transformation 
	cv::Mat GoBoardRaux,GoBoardTaux;
	bool handsOnBoard;
	bool fullBoardInScene;
	
	void setCameraIntrinsics(cv::Mat camM, cv::Mat camD);

	cv::Mat getUndistortImage(cv::Mat& srcImage);
	cv::Mat getProcessedFrame();
	void putFrame(cv::Mat& srcImage);
	bool detectMove();
	bool calculateCameraIntrinsix();
	void changePostMethod();
	void readStone(char  new_BoardStonesStates[361]);
	void detectOcclusionObject();
	void saveBackGroundBoard();
	void testThrehold();
	void detectHand(); //with red blob

	bool controllerMarkerMove();
protected:
	bool findBoard(cv::Mat& srcImage);
	void undistortBoard();
private:
	std::vector<Marker> m_detectedMarkers;
	std::vector<int> boardMarkerID;
	
	int xMarkerNumber, yMarkerNumber;
	int topLeft, topRight, botLeft, botRight;
	bool showMarkers;
	int board_width ;
	int board_height;
	//points of board on original image
	std::vector<cv::Point2f> BoardImagePoint;
	//points of board in the unwraped image
	std::vector<cv::Point2f> UnidistortBoardPoint;

	bool detectBoard;
	StoneDetector sd;

	cv::Mat m_grayscaleImage;
	cv::Mat undistortBoardImage;
	cv::Mat undistortImage;

	cv::Mat srcFrame;
	
	double gap;
	int numPoint;
	//variables for occlusion object detection (ex. hand)
	cv::Mat backGroundBoard;

	//MarkerDetector m_md;
	cv::Mat camMatrix;
	cv::Mat distCoeff;
	
	//we have two ways of finding camera position relative to the models
	enum PoseEstimationMethods {SOLVEPNP,RPP};
	PoseEstimationMethods m_PoseMethod;

	int frameWidth, frameHeight;

	//controller marker
	int controllerMarkerID;
	float markerPrevAngle;
	std::vector<float> controlMarkerAngle;
};

#endif
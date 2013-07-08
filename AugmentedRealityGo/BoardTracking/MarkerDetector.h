#ifndef MarkerDetector_H
#define MarkerDetector_H

////////////////////////////////////////////////////////////////////
// Standard includes:
#include <vector>
#include <opencv2/opencv.hpp>

////////////////////////////////////////////////////////////////////
// Forward declaration:
class Marker;

/**
* A top-level class that encapsulate marker detector algorithm
NOTE: this is based on the open source Fuego marker detector library
*/

class MarkerDetector
{
public:
	typedef std::vector<cv::Point> PointsVector;
	typedef std::vector<PointsVector> ContoursVector;


	//constructor
	MarkerDetector(int ml, cv::Mat camM, cv::Mat camD);	
	
	//! Main marker detection routine
	bool findMarkers(const cv::Mat& grayscale, std::vector<Marker>& detectedMarkers);
  
protected:
	
	
	
	

	//! Converts image to grayscale
	//void prepareImage(const cv::Mat& srcImg, cv::Mat& preparedImg) const;

	//! Performs binary threshold
	 void performThreshold(const cv::Mat& grayscale, cv::Mat& thresholdImg) const;

	//! Detects appropriate contours
	void findCandidates(cv::Mat src, std::vector<Marker>& detectedMarkers, int minPoints, int maxPoints);

	//! Finds marker candidates among all contours
	//void findCandidates(const ContoursVector& contours, std::vector<Marker>& detectedMarkers);
  
	//! Tries to recognize markers by detecting marker code
	void recognizeMarkers(const cv::Mat& grayscale, std::vector<Marker>& detectedMarkers);

	//! Calculates marker poses in 3D
	void estimatePosition(std::vector<Marker>& detectedMarkers);

private:
	float m_minContourLengthAllowed;
  
	int markerLength;
	cv::Mat camMatrix;
	cv::Mat distCoeff;
  
	cv::Mat m_grayscaleImage;
	cv::Mat m_thresholdImg;
	cv::Mat canonicalMarkerImage;

	
	std::vector<cv::Point3f> m_markerCorners3d;
	std::vector<cv::Point2f> m_markerCorners2d;
	
	//test to see if this will help
	cv::Mat objPoints;
    cv::Mat imagePoints;

};

#endif
#ifndef Marker_H
#define Marker_H

////////////////////////////////////////////////////////////////////
// Standard includes:
#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>

////////////////////////////////////////////////////////////////////

/**
* This class represents a marker
*/
class Marker
{
public:
  Marker();
  
  //friend bool operator<(const Marker &M1,const Marker&M2);
  
public:
	bool operator < (const Marker& other) const
    {
        return (id < other.id);
    }
	void draw(cv::Mat &in, cv::Scalar color, int lineWidth);
   cv::Mat Tvec;
   cv::Mat Rvec;
  
  // Id of the marker
  int id;

  static cv::Mat rotate(cv::Mat in);
  static int hammDistMarker(cv::Mat bits);
  static int mat2id(const cv::Mat &bits);
  static int getMarkerId(cv::Mat &in,int &nRotations);

  // Marker transformation with regards to the camera
  //Transformation transformation;
 // Transformation transformation;

  std::vector<cv::Point2f> points;
  void calculateExtrinsics_Custom(cv::Mat camMatrix, cv::Mat distCoeff);
  // Helper function to draw the marker contour over the image
  void drawContour(cv::Mat& image, cv::Scalar color = CV_RGB(0,250,0)) const;
};

#endif
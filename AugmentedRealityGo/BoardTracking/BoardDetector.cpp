///////////////////////////////////////////////////////////////////
// Standard includes:
#include <iostream>
#include <sstream>
#include <string>
////////////////////////////////////////////////////////////////////
// File includes:
#include "BoardDetector.h"
//#include "MarkerDetector.h"

#include "Helper.h"
//#include "Marker.h"
//use aruco to more accurately detect marker corners


BoardDetector::BoardDetector(cv::Mat camM, cv::Mat camD)
	: camMatrix(camM)
	, distCoeff(camD)
{
	
	//board parameter
	numPoint		= 19;
	markerLength	= 90;
	widthGap		= 9.3/markerLength;
	heightGap		= 13.5/markerLength;
	boardHeight		= 31.5/markerLength;
	boardWidth		= 279/markerLength;
	boardLength		= 306/markerLength;
	gap				= (markerLength+16.1)/markerLength;
	
	widthInterval	= 14.6/markerLength;
	heightInterval	= 15.45/markerLength;
	blockLength		= 10/markerLength;

	Board3DPoint.push_back(cv::Point3f(0,0,0));
	Board3DPoint.push_back(cv::Point3f(0,1,0));
	Board3DPoint.push_back(cv::Point3f(1,1,0));
	Board3DPoint.push_back(cv::Point3f(1,0,0));
	//push the 4 wider corner first
	
	Board3DPoint.push_back(cv::Point3f(gap+widthGap-blockLength,		
											   heightGap-blockLength,
											   boardHeight));
	Board3DPoint.push_back(cv::Point3f( gap+widthGap-blockLength,	
											   heightGap+heightInterval*18+blockLength,
											   boardHeight));
	Board3DPoint.push_back(cv::Point3f(gap+widthGap+widthInterval*18+blockLength,	
											   heightGap+heightInterval*18+blockLength,
											   boardHeight));
	Board3DPoint.push_back(cv::Point3f(  gap+widthGap+widthInterval*18+blockLength,
											   heightGap-blockLength,
											   boardHeight));


	//horizontal points
	for(int i=0; i<numPoint; i++)
	{
		//vertical points
		for(int j=0; j<numPoint; j++)
		{

			Board3DPoint.push_back(cv::Point3f(gap+widthGap+widthInterval*j,	
												heightGap+heightInterval*i,	
											   boardHeight));

			/*Board3DPoint.push_back(cv::Point3f(heightGap+heightInterval*i,
											  (gap+widthGap+widthInterval*18),
											   boardHeight));*/
		}
	}
	

}

bool BoardDetector::detectMove(cv::Mat& srcImage)
{
	findBoard(srcImage);

	
	//undistortBoard();

	return true;
}

bool BoardDetector::findBoard(cv::Mat &srcImage)
{
	//detect marker
	//MarkerDetector m_md(100, camMatrix, distCoeff);
	performGrayscale(srcImage,m_grayscaleImage);
	//m_md.findMarkers(m_grayscaleImage, m_detectedMarkers);
	/*aruco::MarkerDetector MDetector;
	std::vector<aruco::Marker> Markers;
	aruco::CameraParameters CamParam(camMatrix, distCoeff, cv::Size(640,480));
	MDetector.detect(srcImage,Markers,CamParam,1);

	//undistort image based on calibration parameters
	cv::undistort(srcImage, undistortImage, camMatrix, distCoeff);
	*/
	//calculate board points based on perspective projection transform
	/*for(size_t i=0; i<m_detectedMarkers.size(); i++)
	{
		//presumably, only one marker is in the environment (for now?)
		aruco::Marker& m = m_detectedMarkers[i];
		BoardImagePoint.clear();

		//project 3d points on the image plane
		cv::projectPoints(Board3DPoint, m.Rvec, m.Tvec, camMatrix, distCoeff, BoardImagePoint);

		for(size_t c=0; c<BoardImagePoint.size(); c++){
			//MyFilledCircle(frame, m_detectedMarkers[i].points[c],cv::Scalar(0,255,0));
			//if(c%2==0)
			//	cv::line(test,  BoardImagePoint[c],  BoardImagePoint[(c+1)],  cv::Scalar(255,0,0), 1, CV_AA);
			MyFilledCircle(undistortImage, BoardImagePoint[c], cv::Scalar(0,0,255));			
		}

	}*/

	cv::namedWindow("result", CV_WINDOW_AUTOSIZE);
	cv::imshow("result", undistortImage);
	cv::waitKey(1);
	//the marker we have right now is id=213
	return true;
}

void BoardDetector::undistortBoard()
{
	if(m_detectedMarkers.size()==0)
		return;
	//testing...
	std::vector<cv::Point2f> m_markerCorners2d(4);
	m_markerCorners2d[0] = (cv::Point2f(0,0));
	m_markerCorners2d[1] = (cv::Point2f(306,0));
	m_markerCorners2d[2] = (cv::Point2f(306,279));
	m_markerCorners2d[3] = (cv::Point2f(0,279));

	std::vector<cv::Point2f> test_center;
	test_center.push_back(BoardImagePoint[0]);
	test_center.push_back(BoardImagePoint[1]);
	test_center.push_back(BoardImagePoint[2]);
	test_center.push_back(BoardImagePoint[3]);
	
	
	UnidistortBoardPoint.clear();
	cv::Mat boardTransform = cv::findHomography(test_center, m_markerCorners2d);
	cv::warpPerspective(undistortImage, undistortBoardImage, boardTransform, cv::Size(307,280));
	
	cv::perspectiveTransform(BoardImagePoint, UnidistortBoardPoint, boardTransform);
	

	for(int c=343; c<346; c++){
		MyFilledCircle(undistortBoardImage, UnidistortBoardPoint[c], cv::Scalar(0,0,255));			
		//std::cout<<c<<" . x: " <<UnidistortBoardPoint[c].x <<" , y: "<<UnidistortBoardPoint[c].y<<std::endl;
	}
	cv::imshow("resul2t", undistortBoardImage);
	cv::waitKey(1);

	int b=8;
	for(int c=343; c<346; c++){
		cv::Rect roi(UnidistortBoardPoint[c].x-b,
			 UnidistortBoardPoint[c].y-b, 
             b*2, 
             b*2);
		cv::Mat subimage(undistortBoardImage, roi);
		std::ostringstream oss;
		oss << c;
		std::string a(oss.str());
	
		cv::imshow(a.c_str(),subimage);
		cv::waitKey(1);
	}

	

	//performGrayscale(canonicalMarkerImage,canonicalMarkerImage);
	//cv::threshold( canonicalMarkerImage, canonicalMarkerImage, 127, 255, cv::THRESH_BINARY);
	/*cv::adaptiveThreshold(canonicalMarkerImage, canonicalMarkerImage,
					255,
					cv::ADAPTIVE_THRESH_MEAN_C,
					cv::THRESH_BINARY,
					5,5);*/
	
	
	std::cout << std::endl;

		
	//findBoard(m_grayscaleImage, t_center);
}


void BoardDetector::performGrayscale(const cv::Mat& bgraMat, cv::Mat& grayscale) const
{
	//cv::GaussianBlur(bgraMat, bgraMat, cv::Size(5,5), 0);
	if (bgraMat.type()!=CV_8UC3) 
		throw cv::Exception(9000,"input.type()!=CV_8UC3","MarkerDetector::findMarkers",__FILE__,__LINE__);
    // Convert to grayscale
    cv::cvtColor(bgraMat, grayscale, CV_BGRA2GRAY);
	//
}

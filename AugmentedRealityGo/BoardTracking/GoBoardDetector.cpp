///////////////////////////////////////////////////////////////////
// Standard includes:
#include <iostream>
#include <sstream>
#include <string>
#include <math.h>
////////////////////////////////////////////////////////////////////
// File includes:
#include "GoBoardDetector.h"
#include "Marker.h"
#include "MarkerDetector.h"


//#include "aruco-1.2.4/aruco.h"
#include "CVHelper.h"
#include "RPP/RPP.h"
GoBoardDetector::GoBoardDetector(Config* c)
	:sd(c->board.whiteStoneThresh, c->board.blackStoneThresh)
{

	boardMarkerID = c->marker.boardMarkerID;
	xMarkerNumber = c->marker.xMarkerNumber;
	yMarkerNumber = c->marker.yMarkerNumber;
	topLeft  = 0;
	topRight = xMarkerNumber-1;
	botLeft  = xMarkerNumber+2*yMarkerNumber-4;
	botRight = 2*xMarkerNumber + 2*yMarkerNumber-5;
	showMarkers = c->marker.showMarkers;
	numPoint		= c->board.numPoint;

	//board parameter
	double markerLength		= c->marker.markerLength;
	double widthGap			= c->board.widthGap/markerLength;
	double heightGap		= c->board.heightGap/markerLength;
	double boardHeight		= c->board.boardHeight/markerLength;
	double boardWidth		= c->board.boardWidth/markerLength;
	double boardLength		= c->board.boardLength/markerLength;
	gap						= (markerLength+c->board.gap)/markerLength;
	double widthInterval	= c->board.widthInterval/markerLength;
	double heightInterval	= c->board.heightInterval/markerLength;
	double blockLength		= c->board.blockLength / markerLength;

	m_PoseMethod = RPP;

	//push the 4 wider corner first
	
	Board3DPoint.push_back(cv::Point3f(	gap+widthGap-blockLength,
											   heightGap-blockLength+gap,
											   boardHeight));
	Board3DPoint.push_back(cv::Point3f( gap+widthGap-blockLength,	
											   heightGap+heightInterval*18+blockLength+gap,
											   boardHeight));
	Board3DPoint.push_back(cv::Point3f(gap+widthGap+widthInterval*18+blockLength,	
											   heightGap+heightInterval*18+blockLength+gap,
											   boardHeight));
	Board3DPoint.push_back(cv::Point3f(  gap+widthGap+widthInterval*18+blockLength,
											   heightGap-blockLength+gap,
											   boardHeight));
	//horizontal points
	for(int i=0; i<numPoint; i++)
	{
		//vertical points
		for(int j=0; j<numPoint; j++)
		{
			Board3DPoint.push_back(cv::Point3f(	gap+widthGap+widthInterval*j,
												heightGap+heightInterval*i+gap,							
											   boardHeight));
		}
	}
	

	camMatrix = cv::Mat(3, 3, CV_32FC1, &(c->cam.intrinsic_array));
	distCoeff = cv::Mat(5, 1, CV_32FC1, &(c->cam.distCoeffs_array));
	handsOnBoard = false;
	fullBoardInScene = false;
	frameWidth = c->cam.width;
	frameHeight = c->cam.height;
}

void GoBoardDetector::setCameraIntrinsics(cv::Mat camM, cv::Mat camD)
{
	camMatrix = camM;
	distCoeff = camD;

}
cv::Mat GoBoardDetector::getUndistortImage(cv::Mat& srcImage)
{
	srcFrame = srcImage;
	cv::undistort(srcImage, undistortImage, camMatrix, distCoeff);
	return undistortImage;
}


bool GoBoardDetector::detectMove()
{
	
	detectBoard = findBoard(srcFrame);
	if(!detectBoard){
		fullBoardInScene = false;
		return false;
	}
	
	undistortBoard();
	detectHand();
	return true;
}


bool GoBoardDetector::findBoard(cv::Mat &srcImage)
{
	//detect marker using aruco library
	//aruco::MarkerDetector MDetector;
	cv::cvtColor(srcImage, m_grayscaleImage, CV_BGRA2GRAY);
	
	bool returnVal = true;
	//custom marker detection based on aruco library
	MarkerDetector m_md(100, camMatrix, distCoeff);
	
	m_md.findMarkers(m_grayscaleImage, m_detectedMarkers);
	
	if(m_detectedMarkers.size()==0)
		return false;
	
	//undistort image based on calibration parameters
	
	//calculate board points based on perspective projection transform
	cv::Mat Rvec;
    cv::Mat Tvec;
	if(!calculateCameraIntrinsix())
		returnVal= false;

	//cv::imshow("result", undistortImage);
	//cv::waitKey(1);
	
	return returnVal;
}

bool GoBoardDetector::calculateCameraIntrinsix()
{
	//cv::showAndSave("marker_board_pre", undistortImage);	

	std::vector<cv::Point3f> m_markerCorners3d;
	cv::Mat test;
	undistortImage.copyTo(test);


	m_markerCorners3d.clear();
	std::vector<cv::Point2f> imgPoints;
	bool hasBoardMarkers = false;
	for(size_t i=0; i<m_detectedMarkers.size(); i++)
	{
		Marker& m = m_detectedMarkers[i];
		m.draw(undistortImage,cv::Scalar(0,0,255),2);
		for(size_t j=0; j<boardMarkerID.size(); j++)
		{
			//if the marker is one of the board marker ids
			if(m.id == boardMarkerID[j])
			{
				bool CorrectMarker = false;
				float botL_x;
				float botL_y;
				//bottom row
				if(j>= botLeft && j<= botRight)
				{
					
					CorrectMarker = true;
					hasBoardMarkers = true;
					int factor = j- botLeft;
					
					botL_x = gap*factor;
					botL_y = 0;
					imgPoints.push_back(m.points[3]);
					imgPoints.push_back(m.points[0]);
					imgPoints.push_back(m.points[1]);
					imgPoints.push_back(m.points[2]);
					m_markerCorners3d.push_back(cv::Point3f(botL_x,botL_y,0));
					m_markerCorners3d.push_back(cv::Point3f(botL_x,botL_y+1,0));
					m_markerCorners3d.push_back(cv::Point3f(botL_x+1,botL_y+1,0));
					m_markerCorners3d.push_back(cv::Point3f(botL_x+1,botL_y,0));

				}//top row
				else if(j>=0 && j<=topRight)
				{
					CorrectMarker = true;
					hasBoardMarkers = true;
					int factor = j;
					botL_x = gap*factor;
					botL_y = (yMarkerNumber-1)*gap;

					imgPoints.push_back(m.points[3]);
					imgPoints.push_back(m.points[0]);
					imgPoints.push_back(m.points[1]);
					imgPoints.push_back(m.points[2]);
					m_markerCorners3d.push_back(cv::Point3f(botL_x,botL_y,0));
					m_markerCorners3d.push_back(cv::Point3f(botL_x,botL_y+1,0));
					m_markerCorners3d.push_back(cv::Point3f(botL_x+1,botL_y+1,0));
					m_markerCorners3d.push_back(cv::Point3f(botL_x+1,botL_y,0));

				}//left row
				else if((j-topRight)%2 == 1)
				{
					CorrectMarker = true;
					hasBoardMarkers = true;
					int factor = (botLeft-j)/2;
					botL_x = 0;
					botL_y = gap*factor;
					
					imgPoints.push_back(m.points[3]);
					imgPoints.push_back(m.points[0]);
					imgPoints.push_back(m.points[1]);
					imgPoints.push_back(m.points[2]);
					m_markerCorners3d.push_back(cv::Point3f(botL_x,botL_y,0));
					m_markerCorners3d.push_back(cv::Point3f(botL_x,botL_y+1,0));
					m_markerCorners3d.push_back(cv::Point3f(botL_x+1,botL_y+1,0));
					m_markerCorners3d.push_back(cv::Point3f(botL_x+1,botL_y,0));
				}//right row
				else if((j-topRight)%2 == 0)
				{
					CorrectMarker = true;
					hasBoardMarkers = true;
					
					int factor = (botLeft-(j-1))/2;
				
					botL_x = (xMarkerNumber-1)*gap;
					botL_y = gap*factor;

					imgPoints.push_back(m.points[3]);
					imgPoints.push_back(m.points[0]);
					imgPoints.push_back(m.points[1]);
					imgPoints.push_back(m.points[2]);
					m_markerCorners3d.push_back(cv::Point3f(botL_x,botL_y,0));
					m_markerCorners3d.push_back(cv::Point3f(botL_x,botL_y+1,0));
					m_markerCorners3d.push_back(cv::Point3f(botL_x+1,botL_y+1,0));
					m_markerCorners3d.push_back(cv::Point3f(botL_x+1,botL_y,0));
				}
				
				if(CorrectMarker){
					if(showMarkers){
						m.draw(undistortImage,cv::Scalar(0,0,255),2);
					}
				}
			}
			
		}
	}


	if(hasBoardMarkers == false){
		m_detectedMarkers.clear();
		GoBoardRaux = cv::Mat();
		GoBoardTaux = cv::Mat();
		return false;
	}


	cv::Mat transform_r;
	BoardImagePoint.clear();
	//two methods
	if(m_PoseMethod == SOLVEPNP){
		cv::Mat iprts;
		cv::Mat raux, taux;
		cv::solvePnP(m_markerCorners3d, imgPoints, camMatrix, distCoeff,transform_r,taux);

		transform_r.convertTo(GoBoardRaux,CV_32F);
		taux.convertTo(GoBoardTaux ,CV_32F);
		cv::Rodrigues(transform_r, GoBoardRaux);

	}else{
		//test
		cv::Mat m_markerCorners3d_RPP = cv::Mat::zeros(3, m_markerCorners3d.size(), CV_64F); // 3D points, z is zero
		cv::Mat imgPoints_RPP = cv::Mat::ones(3, m_markerCorners3d.size(), CV_64F); // 2D points, homogenous points

		cv::Mat_<float> rotMat(3,3); 
		int iterations;
		double obj_err;
		double img_err;
		//cv::Mat iprts;
		std::vector<cv::Point2f> iprts;

		cv::undistortPoints(cv::Mat(imgPoints),iprts,camMatrix, distCoeff);
		for(size_t i=0; i<iprts.size();i++)
		{
			m_markerCorners3d_RPP.at<double>(0,i) = m_markerCorners3d[i].x;
			m_markerCorners3d_RPP.at<double>(1,i) = m_markerCorners3d[i].y;
			imgPoints_RPP.at<double>(0,i) = iprts[i].x;
			imgPoints_RPP.at<double>(1,i) = iprts[i].y;	
		}

		if(!RPP::Rpp(m_markerCorners3d_RPP, imgPoints_RPP, GoBoardRaux, GoBoardTaux, iterations, obj_err, img_err)) {
			fprintf(stderr, "Error with RPP\n");
			return 1;
		}
		cv::solvePnP(m_markerCorners3d, imgPoints, camMatrix, distCoeff,transform_r,GoBoardTaux);

		cv::Rodrigues(GoBoardRaux, transform_r);

	}

	//project 3d points on the image plane
	//std::cout<<transform_r<<std::endl;
	cv::projectPoints(Board3DPoint, transform_r, GoBoardTaux, camMatrix, distCoeff, BoardImagePoint);
	//cv::showAndSave("marker_board", undistortImage);
	for(size_t c=0;c<BoardImagePoint.size() ; c++){
		//MyFilledCircle(undistortImage, m_detectedMarkers[i].points[c],cv::Scalar(0,255,0));

		//if(c<4)
		//	cv::line(undistortImage,  BoardImagePoint[c],  BoardImagePoint[(c+1)%4],  cv::Scalar(255,0,0), 2, CV_AA);
		MyFilledCircle(undistortImage, BoardImagePoint[c], cv::Scalar(0,0,255));	
	}

	fullBoardInScene = true;
	for(int i=0; i<4;i++){
		if(BoardImagePoint[i].x<0 || BoardImagePoint[i].x >frameWidth ||
		   BoardImagePoint[i].y<0 || BoardImagePoint[i].y >frameHeight)
			fullBoardInScene = false;
	
	}

	//boardImagePoint[0-3] are the four corners
	//if(BoardImagePoint[0].x
	
	//cv::showAndSave("marker_board_with_board_pts", undistortImage);
	if(showMarkers){
		cv::imshow("markers",undistortImage);
		cv::waitKey(1);
	}
	return true;
		

}
void GoBoardDetector::changePostMethod()
{
	if(m_PoseMethod == SOLVEPNP){
		m_PoseMethod=RPP;
		std::cout<<"change pose estimation to RPP"<<std::endl;
	}else if(m_PoseMethod==RPP){
		m_PoseMethod = SOLVEPNP;
		std::cout<<"change pose estimation to SOLVEPNP"<<std::endl;
	}
}

//perspective transform the board into a square image
void GoBoardDetector::undistortBoard()
{
	int board_width = 306;
	int board_height = 279;
	if(m_detectedMarkers.size()==0)
		return;
	
	//testing...
	std::vector<cv::Point2f> board_undistort_corner;
	board_undistort_corner.push_back(cv::Point2f(0,0));
	board_undistort_corner.push_back(cv::Point2f(board_width,0));
	board_undistort_corner.push_back(cv::Point2f(board_width,board_height));
	board_undistort_corner.push_back(cv::Point2f(0,board_height));
	
	std::vector<cv::Point2f> board_img_corner;
	board_img_corner.push_back(BoardImagePoint[0]);
	board_img_corner.push_back(BoardImagePoint[1]);
	board_img_corner.push_back(BoardImagePoint[2]);
	board_img_corner.push_back(BoardImagePoint[3]);
	
	UnidistortBoardPoint.clear();
	cv::Mat boardTransform = cv::findHomography(board_img_corner, board_undistort_corner);
	cv::GaussianBlur(srcFrame,srcFrame,cv::Size(3,3),0,0);
	cv::warpPerspective(srcFrame, undistortBoardImage, boardTransform, cv::Size(board_width+1,board_height+1));
	cv::perspectiveTransform(BoardImagePoint, UnidistortBoardPoint, boardTransform);
	

	for(size_t c=4; c<UnidistortBoardPoint.size(); c++){
		//MyFilledCircle(undistortBoardImage, UnidistortBoardPoint[c], cv::Scalar(0,0,255));	
	}
	//cv::imshow("undistort board", undistortBoardImage);
	//cv::waitKey(1);
	//readStone();
}


void GoBoardDetector::detectOcclusionObject()
{
	cv::Mat fore;
	cv::Mat back;
	/*cv::BackgroundSubtractorMOG2 bg; 
	bg.set("nmixtures",3); // set number of gaussian mixtures
	//bg.set("bShadowDetection",false); // turn the shadow detection off

	std::vector<std::vector<cv::Point> > contours;
	bg.operator ()(undistortBoardImage,fore);
	bg.getBackgroundImage(back);
	cv::erode(fore,fore,cv::Mat());
	cv::dilate(fore,fore,cv::Mat());
	cv::findContours(fore,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	cv::drawContours(undistortBoardImage,contours,-1,cv::Scalar(0,0,255),2);
	cv::imshow("Frame",undistortBoardImage);
	cv::imshow("Background",back);
	cv::waitKey(1);*/
	cv::Mat result;
	cv::subtract(undistortBoardImage, backGroundBoard, result);
	cv::imshow("BackgroundBoard",backGroundBoard);cv::waitKey(1);
	cv::imshow("new frame",undistortBoardImage);cv::waitKey(1);
	
	
	cv::cvtColor(result,result,CV_BGR2GRAY ); 
	cv::imshow("dif333f",result);cv::waitKey(1);cv::waitKey(1);

	
	cv::threshold( result, result, 40, 255, cv::THRESH_BINARY );
	//cv::imshow("diff",result);
	//cv::waitKey(1);
}

void GoBoardDetector::saveBackGroundBoard()
{
	std::cout<<"saved current board image as background"<<std::endl;
	//backGroundBoard = undistortBoardImage;
	undistortBoardImage.copyTo(backGroundBoard);
}
void GoBoardDetector::testThrehold()
{
	sd.setupDetection(undistortBoardImage, UnidistortBoardPoint);
	sd.testThrehold();
}


void GoBoardDetector::readStone(char new_BoardStonesStates[361])
{
	sd.setupDetection(undistortBoardImage, UnidistortBoardPoint);
	sd.readStones(new_BoardStonesStates);
}
//with a finger wearing a red ring, count as a hand
void GoBoardDetector::detectHand()
{
	
	//convert to HSV image
	cv::Mat test,imgThreshed, imgThreshed2 ;
	cv::cvtColor(undistortBoardImage,test,CV_BGR2HSV);  
	cv::Mat dst;

	cv::inRange(test, cv::Scalar(105, 135, 135), cv::Scalar(135, 255, 255), imgThreshed);
	//cv::inRange(test, cv::Scalar(0, 135, 135), cv::Scalar(15, 255, 255), imgThreshed);
	//cv::inRange(test, cv::Scalar(159, 135, 135), cv::Scalar(15, 255, 255), imgThreshed2);
	//cvOr(imgThreshed, imgThreshed2, dst);
	

	/// Find contours
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours( imgThreshed, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

	handsOnBoard = (contours.size() >0);
	/*
	std::cout<<"countours: "<< contours.size()<<std::endl;
	for( size_t i = 0; i < contours.size(); i++ )
	{ 
		std::vector<cv::Point> contour_poly;
		cv::Point2f center;
		float radius;
		approxPolyDP( cv::Mat(contours[i]), contour_poly, 3, true );
		cv::minEnclosingCircle( (cv::Mat)contour_poly, center, radius );
		std::cout<<"radius: "<<radius<<std::endl;
	}

	cv::imshow("board",undistortBoardImage);
	cv::waitKey(1);
	cv::imshow("diff",imgThreshed);
	cv::waitKey(1);
	*/
}
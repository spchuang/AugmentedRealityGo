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
GoBoardDetector::GoBoardDetector(cv::Mat camM, cv::Mat camD)
	:boardMarkerID(18)
{
	boardMarkerID[0] = 664; 
	boardMarkerID[1] = 562;
	boardMarkerID[2] = 906;
	boardMarkerID[3] = 508;
	boardMarkerID[4] = 680;
	boardMarkerID[5] = 915;
	boardMarkerID[6] = 518;
	boardMarkerID[7] = 360;
	boardMarkerID[8] = 767;
	boardMarkerID[9] = 108;
	boardMarkerID[10] = 376;
	boardMarkerID[11] = 961;
	boardMarkerID[12] = 786;
	boardMarkerID[13] = 913;
	boardMarkerID[14] = 112;
	boardMarkerID[15] = 708;
	boardMarkerID[16] = 156;
	boardMarkerID[17] = 804;
	//board parameter
	numPoint		= 19;
	//markerLength	= 90;
	markerLength	= 81.8;
	widthGap		= 9.2/markerLength;
	heightGap		= 13/markerLength;
	boardHeight		= 31.5/markerLength;
	boardWidth		= 279/markerLength;
	boardLength		= 306/markerLength;
	gap				= (markerLength+15.7)/markerLength;
	
	widthInterval	= 14.55/markerLength;
	heightInterval	= 15.7/markerLength;
	blockLength		= 9/markerLength;
	m_PoseMethod = RPP;
	//Board3DPoint.push_back(cv::Point3f(0,0,0));
	//push the 4 wider corner first
	Board3DPoint.push_back(cv::Point3f(		gap+widthGap-blockLength,
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
	//same with GL board points with slightly higher values of width interval and height intervals
	/*widthInterval	= 14.67/markerLength;
	heightInterval	= 15.57/markerLength;
	Board3DPoint_GL.push_back(cv::Point3f(		gap+widthGap-blockLength,
											   heightGap-blockLength+gap,
											   boardHeight));
	Board3DPoint_GL.push_back(cv::Point3f( gap+widthGap-blockLength,	
											   heightGap+heightInterval*18+blockLength+gap,
											   boardHeight));
	Board3DPoint_GL.push_back(cv::Point3f(gap+widthGap+widthInterval*18+blockLength,	
											   heightGap+heightInterval*18+blockLength+gap,
											   boardHeight));
	Board3DPoint_GL.push_back(cv::Point3f(  gap+widthGap+widthInterval*18+blockLength,
											   heightGap-blockLength+gap,
											   boardHeight));
	*/
	detectOcclusion = false;

	camMatrix = camM;
	distCoeff = camD;

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
	if(!detectBoard)
		return false;
	
	undistortBoard();
	if(detectOcclusion)
		detectOcclusionObject();

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

	cv::imshow("result", undistortImage);
	cv::waitKey(1);
	
	return returnVal;
}

bool GoBoardDetector::calculateCameraIntrinsix()
{
	std::vector<cv::Point3f> m_markerCorners3d;
	cv::Mat test;
	undistortImage.copyTo(test);

	m_markerCorners3d.clear();
	std::vector<cv::Point2f> imgPoints;
	bool hasBoardMarkers = false;
	for(size_t i=0; i<m_detectedMarkers.size(); i++)
	{
		Marker& m = m_detectedMarkers[i];
		for(size_t j=0; j<boardMarkerID.size(); j++)
		{
			//if the marker is one of the board marker ids
			if(m.id == boardMarkerID[j])
			{
				//front row
				if(j>=14 && j<=17)
				{
					hasBoardMarkers = true;
					m.draw(undistortImage,cv::Scalar(0,0,255),2);
					int factor = j-13;
					
					imgPoints.push_back(m.points[3]);
					imgPoints.push_back(m.points[0]);
					imgPoints.push_back(m.points[1]);
					imgPoints.push_back(m.points[2]);
					
					m_markerCorners3d.push_back(cv::Point3f(gap*factor,0,0));
					m_markerCorners3d.push_back(cv::Point3f(gap*factor,1,0));
					m_markerCorners3d.push_back(cv::Point3f(gap*factor+1,1,0));
					m_markerCorners3d.push_back(cv::Point3f(gap*factor+1,0,0)); 
				}
				//left row
				else if(j==7 || j==9|| j==11 || j==13)
				{
					hasBoardMarkers = true;
					m.draw(undistortImage,cv::Scalar(0,0,255),2);
					int factor = (13-j)/2;
					//std::cout<<factor<<std::endl;

					imgPoints.push_back(m.points[3]);
					imgPoints.push_back(m.points[0]);
					imgPoints.push_back(m.points[1]);
					imgPoints.push_back(m.points[2]);

					
					m_markerCorners3d.push_back(cv::Point3f(0,gap*factor,0));
					m_markerCorners3d.push_back(cv::Point3f(0,gap*factor+1,0));
					m_markerCorners3d.push_back(cv::Point3f(1,gap*factor+1,0));
					m_markerCorners3d.push_back(cv::Point3f(1,gap*factor,0)); 
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
		//std::cout<<"data for Rpp"<<std::endl;
		//std::cout<<m_markerCorners3d_RPP<<std::endl;
		//std::cout<<imgPoints_RPP<<std::endl;
		//std::cout<<GoBoardRaux<<std::endl;

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
	
	for(size_t c=0;c<BoardImagePoint.size() ; c++){
		//MyFilledCircle(undistortImage, m_detectedMarkers[i].points[c],cv::Scalar(0,255,0));

		//if(c<4)
		//	cv::line(undistortImage,  BoardImagePoint[c],  BoardImagePoint[(c+1)%4],  cv::Scalar(255,0,0), 2, CV_AA);
		MyFilledCircle(undistortImage, BoardImagePoint[c], cv::Scalar(0,0,255));	
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
	detectOcclusion= true;
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
	cv::imshow("diff",result);
	cv::waitKey(1);
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
	/*
	
	std::cout<<"old board"<<std::endl;
	for(int i=18; i>=0;i--){
		for(int j=0; j<19;j++)
		{
			if(BoardStonesStates[i*19+j]==2)
				std::cout <<"o ";
			else if(BoardStonesStates[i*19+j]==1)
				std::cout <<"W ";
			else if(BoardStonesStates[i*19+j]==0)
				std::cout <<"B ";
		}
		std::cout<<std::endl;
	}
	*/
	//see which move is new

	/*
	for(int i=0; i<19*19; i++)
	{
		if(BoardStonesStates[i] != new_BoardStonesStates[i])
		{
			newMove[0] = BoardStonesStates[i];
			newMove[1] = i;
			break;
		}

	}*/
	/*
	if(newMove[1] == -1)
	{
		std::cout<<"#didn't read the move, try again#"<<std::endl;
		return false;
	}
	
	BoardStonesStates = new_BoardStonesStates;
	return true;
	*/
}
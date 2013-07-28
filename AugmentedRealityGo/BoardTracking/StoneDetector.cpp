///////////////////////////////////////////////////////////////////
// Standard includes:
#include <iostream>
#include <sstream>

////////////////////////////////////////////////////////////////////
// File includes:
#include "StoneDetector.h"


StoneDetector::StoneDetector(int w_thresh = 190, int b_thresh = 60, int dist=6)
{
	//testing values
	//if it's dark, black_thresh shoudl be lower and vice versa for white
	white_thresh = w_thresh;
	black_thresh = b_thresh;
	min_radius = 3;
	max_radius = 15;
	border = 5; //border padding
	max_dist = dist;
}

int thresh = 100;
int max_thresh = 255;
cv::Mat test;


/** @function thresh_callback */
void thresh_callback(int, void* )
{
  cv::Mat threshold_output;
  std::vector<std::vector<cv::Point> > contours;
  std::vector<cv::Vec4i> hierarchy;

  /// Detect edges using Threshold
  cv::threshold( test, threshold_output, thresh, 255, cv::THRESH_BINARY );
  /// Find contours
  cv::findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
   cv::imshow( "thre", threshold_output );
   cv:: waitKey(1);
  /// Approximate contours to polygons + get bounding rects and circles
  std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
  std::vector<cv::Rect> boundRect( contours.size() );
  std::vector<cv::Point2f>center( contours.size() );
  std::vector<float>radius( contours.size() );

  for( size_t i = 0; i < contours.size(); i++ )
     { approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
       boundRect[i] = cv::boundingRect( cv::Mat(contours_poly[i]) );
       cv::minEnclosingCircle( (cv::Mat)contours_poly[i], center[i], radius[i] );
     }


  /// Draw polygonal contour + bonding rects + circles
 cv::Mat drawing = cv::Mat::zeros( threshold_output.size(), CV_8UC3 );
  for( size_t i = 0; i< contours.size(); i++ )
     {
       cv::Scalar color = cv::Scalar(255,0,0 );
       drawContours( test, contours_poly, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
       //rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
       cv::circle( test, center[i], (int)radius[i], color, 2, 8, 0 );
     }

  /// Show in a window
  cv::namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
  cv::imshow( "Contours", test );
   cv:: waitKey(1);
  
}

void StoneDetector::testThrehold()
{
	cv::cvtColor(boardImage,test,CV_BGR2GRAY ); 

	char* source_window = "Source";
	cv::namedWindow( source_window, CV_WINDOW_AUTOSIZE );
	cv::imshow( source_window, test );
	std::cout<<"start threhold"<<std::endl;
	cv::createTrackbar( " Threshold:", "Source", &thresh, max_thresh, thresh_callback );
	thresh_callback( 0, 0 );
	cv:: waitKey(1);
}
void StoneDetector::setupDetection(cv::Mat b, std::vector<cv::Point2f> bp)
{
	boardImage =b;
	StonePoints = bp;
	//setup borders to the image
	cv::cvtColor(boardImage,greyBoardImage,CV_BGR2GRAY ); 
	/*
	float a=0;
	for(int i=0; i<greyBoardImage.cols; i++)
		for(int j=0;j<greyBoardImage.rows; j++)
			a+=greyBoardImage.at<uchar>(j,i);
	a/=greyBoardImage.cols*greyBoardImage.rows;
	std::cout <<a <<std::endl;
	*/
	cv::copyMakeBorder( greyBoardImage, greyBoardImage, border, border, border, border, cv::BORDER_CONSTANT, cv::Scalar(165) );
	
	// cv::imshow( "border", t );
	//cv:: waitKey(1);

}
void StoneDetector::displayStoneDetection(std::vector<cv::Point2f> b, std::vector<cv::Point2f> w, char  bss[361])
{
	//std::cout<<"black: "<<b.size()<<std::endl<<"white: "<<w.size()<<std::endl;
	for(size_t i=0; i<b.size(); i++)
	{
		cv::circle( boardImage, b[i], (int)3, cv::Scalar(255,0,0), 2, 8, 0 );
	}
	for(size_t i=0; i<w.size(); i++)
	{
		cv::circle( boardImage, w[i], (int)3, cv::Scalar(0,0,255), 2, 8, 0 );
	}
	for(size_t c=4; c<StonePoints.size(); c++){
		MyFilledCircle(boardImage, StonePoints[c], cv::Scalar(0,255,0));	
	}
	
	for(size_t i=18; i>=0;i--){
		for(size_t j=0; j<19;j++)
		{
			if(bss[i*19+j]==COLOR_NONE)
				std::cout <<"o ";
			else if(bss[i*19+j]==COLOR_WHITE)
				std::cout <<"W ";
			else if(bss[i*19+j]==COLOR_BLACK)
				std::cout <<"B ";
		}
		std::cout<<std::endl;
	}
	
	//cv::imshow( "2", boardImage );
	//cv:: waitKey(1);
}



void StoneDetector::readStones(char BoardStonesStates[361])
{
	
	//initialize? or check from previous
	for(int i=0; i<19*19; i++)
	{
		BoardStonesStates[i] = 2;
	}

	//detect white and black stones separately by threholding the board image
	std::vector<cv::Point2f> whiteStones;
	findCandidateStones(false, whiteStones, BoardStonesStates);

	std::vector<cv::Point2f> blackStones;
	findCandidateStones(true, blackStones, BoardStonesStates);

	
	/*
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
	
	//show result
	//displayStoneDetection(blackStones, whiteStones, BoardStonesStates);

}
void StoneDetector::drawGrid(cv::Mat input, cv::Mat& output, cv::Scalar color)
{
	cv::Point2f sw, se, nw,ne;
	sw = cv::Point2f(StonePoints[4].x+border, StonePoints[4].y+border);
	se = cv::Point2f(StonePoints[22].x+border, StonePoints[22].y+border);
	nw = cv::Point2f(StonePoints[346].x+border, StonePoints[346].y+border);
	ne = cv::Point2f(StonePoints[364].x+border, StonePoints[364].y+border);
	output = input;

	//cv::line(output, sw, nw, color);
	//vertocal lines
	float interval = (abs((se.y-sw.y)/18)+ abs((ne.y-nw.y)/18))/2;
	for(int i=0; i<18; i++)
	{
		cv::line(output, cv::Point2f(0, sw.y+interval/2+interval*i),cv::Point2f(299,nw.y+interval/2+interval*i), color,3);
	}

	//horizontal
	interval = (abs((nw.x-sw.x)/18)+ abs((ne.x-se.x)/18))/2;
	for(int i=0; i<18; i++)
	{
		cv::line(output, cv::Point2f(sw.x+interval/2+interval*i, 0),cv::Point2f(se.x+interval/2+interval*i,299), color,3);
	}

	/*
	cv::imshow( "test grid", output );
	cv:: waitKey(1);*/
}

void StoneDetector::findCandidateStones(bool findBlack,std::vector<cv::Point2f> &potentialStoneCenters, char BoardStonesStates[361])
{
	

	cv::Mat thre_output;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3,3));

	/// Detect edges using Threshold
	if(findBlack){
		cv::threshold( greyBoardImage, thre_output, black_thresh, 255, cv::THRESH_BINARY );
		cv::bitwise_not(thre_output, thre_output);

	
		cv::morphologyEx( thre_output, thre_output, cv::MORPH_OPEN, kernel,cv::Point(-1,-1),3);
	

		//cv::imshow( "blackthresh", thre_output );
		//cv:: waitKey(1);
		drawGrid(thre_output, test, cv::Scalar(0));


		cv::morphologyEx( test, test, cv::MORPH_OPEN, kernel,cv::Point(-1,-1),2);
		
		//cv::dilate(test,test,kernel);
		//cv::imshow( "black grid", test );
		cv:: waitKey(1);
		/*
		cv::cvtColor(test, t, CV_GRAY2BGR);
		for(size_t i=4; i<StonePoints.size(); i++){

			cv::Point2f realCenter = cv::Point2f(StonePoints[i].x+border, StonePoints[i].y+border);
			MyFilledCircle(t, realCenter, cv::Scalar(255,0,0));	
		}
		*/
		thre_output = test;
	}
	else{

		cv::threshold( greyBoardImage, thre_output, white_thresh, 255, cv::THRESH_BINARY );	
		
		cv::Mat test;
		cv::morphologyEx( thre_output, thre_output, cv::MORPH_OPEN, kernel,cv::Point(-1,-1),3);
		
		//cv::imshow( "whitethresh", thre_output );
		//cv:: waitKey(1);

		drawGrid(thre_output, test, cv::Scalar(0));

		
		cv::morphologyEx( test, test, cv::MORPH_OPEN, kernel,cv::Point(-1,-1),2);

		
		cv::dilate(test,test,kernel);
		//cv::imshow( "white grid", test );
		//cv:: waitKey(1);

		/*
		cv::cvtColor(test, t, CV_GRAY2BGR);
		for(size_t i=4; i<StonePoints.size(); i++){

			cv::Point2f realCenter = cv::Point2f(StonePoints[i].x+border, StonePoints[i].y+border);
			MyFilledCircle(t, realCenter, cv::Scalar(255,0,0));	
		}
		/*
		cv::imshow( "test_dilate+white", t );
		 cv:: waitKey(1);
		*/
		thre_output = test;
	}
	
	//cv::Mat a;
	//cv::cvtColor(thre_output, a, CV_GRAY2BGR);
	/// Find contours
	cv::findContours( thre_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
	
	/// Approximate contours to find potential stone locations
	
	for( size_t i = 0; i < contours.size(); i++ )
	{ 
		std::vector<cv::Point> contour_poly;
		cv::Point2f center;
		float radius;
		approxPolyDP( cv::Mat(contours[i]), contour_poly, 3, true );
		cv::minEnclosingCircle( (cv::Mat)contour_poly, center, radius );


		//select only those big enough
		if(radius < max_radius && radius >min_radius)
		{
			//MyFilledCircle(a, center, cv::Scalar(0,0,255));	
			bool tooNear = false;

			//remove the ones that are too close to each other
			for(size_t j=0; j<potentialStoneCenters.size(); j++)
			{
				double distSquared = sqrt (SQ( center.x-potentialStoneCenters[j].x)+SQ( center.y-potentialStoneCenters[j].y));
				if(distSquared <9)
					tooNear = true;
			}
			
			if(!tooNear){
				// convert the coordinate to the one before adding borders
				cv::Point2f realCenter = cv::Point2f(center.x-border, center.y-border);

				//check if the point is close enough to each stone positions
				//this rejects the light points that are inbetween points
				for(size_t i=4; i<StonePoints.size(); i++){
					double distSquared = sqrt (SQ(realCenter.x-StonePoints[i].x)+SQ(realCenter.y-StonePoints[i].y));
	
					
					if(distSquared <max_dist){
						//push this to candidate list
						potentialStoneCenters.push_back(realCenter);	

						//assign the value of stone status
						if(BoardStonesStates[i-4] == 2)
						{
							if(findBlack)
								BoardStonesStates[i-4] = COLOR_BLACK;
							else
								BoardStonesStates[i-4] = COLOR_WHITE;
						}
						break;
					}

		

				}




			}
				
		}
	}
	/*
	for(size_t i=4; i<StonePoints.size(); i++){

		cv::Point2f realCenter = cv::Point2f(StonePoints[i].x+border, StonePoints[i].y+border);
		MyFilledCircle(a, realCenter, cv::Scalar(255,0,0));	
	}
	if(!findBlack){
	 cv::imshow( "white", a );
		 cv:: waitKey(1);

		 
	}else{
		 cv::imshow( "black", a );
		 cv:: waitKey(1);
	}
	
	 */
}


//original attempt(failed)
/*
void GoBoardDetector::triggerReadEmptyBoardHSV()
{
	if(readEmptyBoardHSV==true){
		
		readEmptyBoardHSV=false;
		std::cout<<"stop reading empty board HSV"<<std::endl;
		std::cout<<"total sampels: "<<readLoop<<std::endl;

		for(int i=0; i<19*19;i++)
		{
			float avgH,avgS,avgV;
			avgH = avgS = avgV =0;
			for(int j=0; j<totalSampleStoneBoxHSV.size();j++)
			{
				avgH+=totalSampleStoneBoxHSV[j][i].x;
				avgS+=totalSampleStoneBoxHSV[j][i].y;
				avgV+=totalSampleStoneBoxHSV[j][i].z;
			}
			avgH/=totalSampleStoneBoxHSV.size();
			avgS/=totalSampleStoneBoxHSV.size();
			avgV/=totalSampleStoneBoxHSV.size();

			float devH, devS, devV;
			devH = devS = devV =0;
			for(int j=0; j<totalSampleStoneBoxHSV.size();j++)
			{
				devH+= SQ(totalSampleStoneBoxHSV[j][i].x-avgH);
				devS+= SQ(totalSampleStoneBoxHSV[j][i].y-avgS);
				devV+= SQ(totalSampleStoneBoxHSV[j][i].z-avgV);
			}

			devH=sqrt(devH/(totalSampleStoneBoxHSV.size()-1));
			devS=sqrt(devS/(totalSampleStoneBoxHSV.size()-1));
			devV=sqrt(devV/(totalSampleStoneBoxHSV.size()-1));

			AvgStoneBoxHSV[i].x = avgH;
			AvgStoneBoxHSV[i].y = avgS;
			AvgStoneBoxHSV[i].z = avgV;
			
			DeviationStoneBoxHSV[i].x = devH;
			DeviationStoneBoxHSV[i].y = devS;
			DeviationStoneBoxHSV[i].z = devV;
		}


	}else{
		
		readEmptyBoardHSV=true;
		std::cout<<"start reading empty board HSV.."<<std::endl;
	}
	
	
}
void GoBoardDetector::averageEmptyBoardHSV()
{
	std::vector<cv::Point3f> newBoxHSV;
	readBoardBoxHSV(newBoxHSV);
	totalSampleStoneBoxHSV.push_back(newBoxHSV);
	readLoop++;

	for(int i=0; i<19*19;i++)
	{


		AvgStoneBoxHSV[i].x = (AvgStoneBoxHSV[i].x*(readLoop-1)+newBoxHSV[i].x)/readLoop;
		AvgStoneBoxHSV[i].y = (AvgStoneBoxHSV[i].y*(readLoop-1)+newBoxHSV[i].y)/readLoop;
		AvgStoneBoxHSV[i].z = (AvgStoneBoxHSV[i].z*(readLoop-1)+newBoxHSV[i].z)/readLoop;
	}

	
}

//read the average BoardBoxHSV 
void GoBoardDetector::readBoardBoxHSV(std::vector<cv::Point3f>& currentBoardStongeBoxHSV)
{
	cv::Mat test;
	cv::GaussianBlur(undistortBoardImage,test,cv::Size(3,3),0,0);
	cv::cvtColor(test,test,CV_BGR2HSV);  
	std::vector<cv::Mat> s;
	cv::split(test,s);
	cv::equalizeHist( s[2], s[2] );
	cv::merge(s,test);

	for(int c=4;c<UnidistortBoardPoint.size();c++)
	{
		int th=0;
		int ts=0;
		int tv=0;
		cv::Point2f &p= UnidistortBoardPoint[c];
		
		if(p.x<0)return;
		for(int i=p.x-block_length; i<=p.x+block_length;i++)
		{
			for(int j=p.y-block_length; j<p.y+block_length;j++)
			{
				cv::Vec3b &pixel = test.at<cv::Vec3b>(j,i);
				th+=pixel[0];
				ts+=pixel[1];
				tv+=pixel[2];
			}
		}

		currentBoardStongeBoxHSV.push_back(cv::Point3f(th/(block_length*block_length*4),
													   ts/(block_length*block_length*4),
													   tv/(block_length*block_length*4)));
		//std::cout<<p<<"v average for "<<c<<": "<<th<<" ,  "<<ts<<" , "<<tv<<std::endl;
	}
}

void GoBoardDetector::readGoStone()
{
	//average 8 frames
	if(detectStonesRound==7){
		std::cout<<"---------------------------------------------------------------"<<std::endl<<std::endl;
		int black =0, white=1;
		int stones[19*19];
		int size = AvgStoneBoxHSV_Read.size();
		for(int i=0; i<19*19;i++)
		{
			//if(i>=19*8 && i<19*9){
				std::cout<<i+1<<": avg         "<<AvgStoneBoxHSV[i]<<std::endl;;
				std::cout<<i+1<<": dev         "<<DeviationStoneBoxHSV[i]<<std::endl;;
				std::cout<<i+1<<": read        "<<
					
					(AvgStoneBoxHSV_Read[i].x-AvgStoneBoxHSV[i].x)/DeviationStoneBoxHSV[i].x<<" , "<<
					(AvgStoneBoxHSV_Read[i].y-AvgStoneBoxHSV[i].y)/DeviationStoneBoxHSV[i].y<<" , "<<
					(AvgStoneBoxHSV_Read[i].z-AvgStoneBoxHSV[i].z)/DeviationStoneBoxHSV[i].z<<std::endl<<std::endl;
			//}
			//first determine if there is even an color object by using the Hue value
			cv::Point3f diff = AvgStoneBoxHSV_Read[i] - AvgStoneBoxHSV[i];

			if(diff.x > 2*DeviationStoneBoxHSV[i].x){
				if(diff.y >DeviationStoneBoxHSV[i].y && diff.z < -2*DeviationStoneBoxHSV[i].z)
					stones[i] = black;
				else if(diff.z < -2.5*DeviationStoneBoxHSV[i].z)
					stones[i] = black;
				else if(diff.y <-DeviationStoneBoxHSV[i].y && diff.z > 2*DeviationStoneBoxHSV[i].z)
					stones[i] = white;
				else if(diff.z > 2.5*DeviationStoneBoxHSV[i].z)
					stones[i] = white;
				else if(diff.y > DeviationStoneBoxHSV[i].y && diff.z > 2*DeviationStoneBoxHSV[i].z)
					stones[i] = white;
				else
					stones[i] = 3;
			}else{
				
				if(diff.x > DeviationStoneBoxHSV[i].x && diff.z < -2.5*DeviationStoneBoxHSV[i].z)
					stones[i] = black;
				else if(diff.y > 2*DeviationStoneBoxHSV[i].y && diff.z < -3*DeviationStoneBoxHSV[i].z)
					stones[i] = black;
				
				else if(diff.x > DeviationStoneBoxHSV[i].x &&  diff.z > 2.5*DeviationStoneBoxHSV[i].z)
					stones[i] = white;
				else if(diff.y < -2*DeviationStoneBoxHSV[i].y &&  diff.z > 3*DeviationStoneBoxHSV[i].z)
					stones[i] = white;
				
				else if (diff.x > 0.5*DeviationStoneBoxHSV[i].x && diff.z < -3*DeviationStoneBoxHSV[i].z)
					stones[i] = 3;
				else if(diff.x > 0.5*DeviationStoneBoxHSV[i].x && diff.z > 3*DeviationStoneBoxHSV[i].z)
					stones[i] = 3;
				else
					stones[i] = 2;
			}

		}

		for(int i=18; i>=0;i--)
		{
			for(int j=0;j<19;j++)
			{
				if(stones[i*19+j] == black){
					std::cout<<"B";
				}else if(stones[i*19+j] == white){
					std::cout<<"W";
				}else if(stones[i*19+j]==3){
					std::cout<<"?";
				}else if(stones[i*19+j]==2){
					std::cout<<"o";
				}else{
					std::cout <<"m";
				}
				std::cout<<" ";

			}

			std::cout<<std::endl;
		}
		
		std::cout<<std::endl<<std::endl;


		AvgStoneBoxHSV_Read.clear();
		for(int i=0; i<19*19;i++)
			AvgStoneBoxHSV_Read.push_back(cv::Point3f(0,0,0));
		detectStonesRound=-1;
	}else{
		std::vector<cv::Point3f> newBoxHSV;
		readBoardBoxHSV(newBoxHSV);
		detectStonesRound++;
		for(int i=0; i<19*19;i++)
		{
			AvgStoneBoxHSV_Read[i].x = (AvgStoneBoxHSV_Read[i].x*(detectStonesRound)+newBoxHSV[i].x)/(detectStonesRound+1);
			AvgStoneBoxHSV_Read[i].y = (AvgStoneBoxHSV_Read[i].y*(detectStonesRound)+newBoxHSV[i].y)/(detectStonesRound+1);
			AvgStoneBoxHSV_Read[i].z = (AvgStoneBoxHSV_Read[i].z*(detectStonesRound)+newBoxHSV[i].z)/(detectStonesRound+1);
		}
		
	}

	
}*/
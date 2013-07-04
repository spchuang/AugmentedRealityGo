///////////////////////////////////////////////////////////////////
// Standard includes:
#include <iostream>
#include <sstream>

////////////////////////////////////////////////////////////////////
// File includes:
#include "MarkerDetector.h"
#include "CVHelper.h"
#include "Marker.h"


MarkerDetector::MarkerDetector(int ml, cv::Mat camM, cv::Mat camD)
    : m_minContourLengthAllowed(100)
    , markerLength(ml)
	, m_markerCorners2d(4)
	, camMatrix(camM),
	  distCoeff(camD),
	   objPoints (4,3,CV_32FC1 ),
	   imagePoints (4,2,CV_32FC1 )
{
 /*
	for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
			m_markerCorners3d.push_back(cv::Point3f(0.5*(float)i,0.5*(float)j,0));*/
			//m_markerCorners3d.push_back(cv::Point3f(i,j,0));
//    m_markerCorners3d.push_back(cv::Point3f(1,0,0));
 //   m_markerCorners3d.push_back(cv::Point3f(1,1,0));
  //  m_markerCorners3d.push_back(cv::Point3f(0,1,0)); 


    m_markerCorners2d[0] = (cv::Point2f(0,0));
	m_markerCorners2d[1] = (cv::Point2f(markerLength-1,0));
	m_markerCorners2d[2] = (cv::Point2f(markerLength-1,markerLength-1));
	m_markerCorners2d[3] = (cv::Point2f(0,markerLength-1));
		/*dst.push_back(cv::Point2f(0,markerLength-1));
		dst.push_back(cv::Point2f(0,0));
		dst.push_back(cv::Point2f(markerLength-1,0));
		dst.push_back(cv::Point2f(markerLength-1,markerLength-1));*/
}
/*
void MarkerDetector::processFrame(const BGRAVideoFrame& frame)
{
    std::vector<Marker> markers;
    findMarkers(frame, markers);

    m_transformations.clear();
    for (size_t i=0; i<markers.size(); i++)
    {
        m_transformations.push_back(markers[i].transformation);
    }
}
*/
/*
const std::vector<Transformation>& MarkerDetector::getTransformations() const
{
    return m_transformations;
}
*/

bool MarkerDetector::findMarkers(const cv::Mat& grayscale, std::vector<Marker>& detectedMarkers)
{

	if (grayscale.type()!=CV_8UC1) 
		throw cv::Exception(9000,"input.type()!=CV_8UC1","MarkerDetector::findMarkers",__FILE__,__LINE__);
    // Prepare the image for processing
    //prepareImage(frame, m_grayscaleImage);

    // Make it binary
    performThreshold(grayscale, m_thresholdImg);
	//cv::imshow("threshold", m_thresholdImg);
	//cv::waitKey(1);
	//cv::imshow("resul2", m_thresholdImg);
	//cv::waitKey(1);

    // Detect contours
	
	findCandidates(m_thresholdImg, detectedMarkers, m_thresholdImg.cols*0.16,m_thresholdImg.cols*2);

	
	/*
	for (size_t i=0; i<m_contours.size(); i++)
    {
			drawContours( frame, m_contours, i,  cv::Scalar(0,0,255), CV_FILLED, 8);
		
    }
	cv::imshow("resul2", frame);
	cv::waitKey(0);*/

    // Find closed contours that can be approximated with 4 points
   
    // Find is them are markers
    recognizeMarkers(grayscale, detectedMarkers);
	
    // Calculate their poses
    //estimatePosition(detectedMarkers);

    //sort by id
    //std::sort(detectedMarkers.begin(), detectedMarkers.end());
    return false;
}


void MarkerDetector::performThreshold(const cv::Mat& grey, cv::Mat& out) const
{	
	//apply threhold (ADAPTIVE_THRESH_GAUSSIAN_C, ADAPTIVE_THRESH_MEAN_C)
	cv::adaptiveThreshold ( grey,out,255,cv::ADAPTIVE_THRESH_MEAN_C,cv::THRESH_BINARY_INV,7,7 );
}

void MarkerDetector::findCandidates(cv::Mat src, std::vector<Marker>& detectedMarkers, int minPoints, int maxPoints)
{
	
	cv::Mat thres2;
	src.copyTo ( thres2 );
	std::vector<cv::Vec4i> hierarchy2;
    ContoursVector allContours;
	std::vector<cv::Point> approxCurve;
	std::vector<Marker> possibleMarkers;

    cv::findContours(thres2, allContours,hierarchy2, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    for (size_t i=0; i<allContours.size(); i++)
    {
        int contourSize = allContours[i].size();
		if ( minPoints< contourSize && contourSize<maxPoints  )
        {
			//drawContours( src, allContours, i,  cv::Scalar(0,0,255), CV_FILLED, 8);
            
			cv::approxPolyDP(allContours[i], approxCurve, double(contourSize * 0.05), true);

			 // Selecting only those with 4 points (corners)
			if (approxCurve.size() != 4)
				continue;

			// Selecting only convex polygons
			if (!cv::isContourConvex(cv::Mat(approxCurve)))
				continue;

			 // Ensure that the distance between consecutive points is large enough
			float minDist = std::numeric_limits<float>::max();

			for (int i = 0; i < 4; i++)
			{
				cv::Point side = approxCurve[i] - approxCurve[(i+1)%4];
				//float squaredSideLength = side.dot(side);
				float squaredSideLength= std::sqrt ( ( float ) ( approxCurve[i].x-approxCurve[ ( i+1 ) %4].x ) * ( approxCurve[i].x-approxCurve[ ( i+1 ) %4].x ) +
												 ( approxCurve[i].y-approxCurve[ ( i+1 ) %4].y ) * ( approxCurve[i].y-approxCurve[ ( i+1 ) %4].y ) );
			   //std::cout<<squaredSideLength<< "    " <<test<<std::endl; 
			   //minDist = std::min(minDist, squaredSideLength);
				if ( squaredSideLength<minDist ) minDist=squaredSideLength;
			}

			// Selecting those whose minimum side length is still greater than the limit
			if (minDist <= 10)
				continue;

			 // All tests are passed. Save marker candidate:
			Marker m;

			for (int i = 0; i<4; i++)
				m.points.push_back( cv::Point2f(approxCurve[i].x,approxCurve[i].y) );

			// Sort the points in anti-clockwise order
			// Trace a line between the first and second point.
			// If the third point is at the right side, then the points are anti-clockwise
			cv::Point v1 = m.points[1] - m.points[0];
			cv::Point v2 = m.points[2] - m.points[0];

			double o = (v1.x * v2.y) - (v1.y * v2.x);

			if (o < 0.0)	//if the third point is in the left side, then sort in anti-clockwise order
				std::swap(m.points[1], m.points[3]);

			possibleMarkers.push_back(m);
        }
		
    }
	

	// Remove these elements which corners are too close to each other.
    // First detect candidates for removal:
    std::vector< std::pair<int,int> > tooNearCandidates;
    for (size_t i=0;i<possibleMarkers.size();i++)
    {
        const Marker& m1 = possibleMarkers[i];

        //calculate the average distance of each corner to the nearest corner of the other marker candidate
        for (size_t j=i+1;j<possibleMarkers.size();j++)
        {
            const Marker& m2 = possibleMarkers[j];
            float distSquared = 0;
            for (int c = 0; c < 4; c++)
            {
				/*
				distSquared+= sqrt ( ( m1.points[c].x-m2.points[c].x ) * ( m1.points[c].x-m2.points[c].x ) +
								( m1.points[c].y-m2.points[c].y ) * ( m1.points[c].y-m2.points[c].y ) );
				*/
				distSquared+= sqrt ( ( possibleMarkers[i].points[c].x-possibleMarkers[j].points[c].x ) * 
										( possibleMarkers[i].points[c].x-possibleMarkers[j].points[c].x ) + 
										( possibleMarkers[i].points[c].y-possibleMarkers[j].points[c].y ) * 
										( possibleMarkers[i].points[c].y-possibleMarkers[j].points[c].y ) );
                //cv::Point v = m1.points[c] - m2.points[c];
                //distSquared2 += v.dot(v);
            }

            distSquared /= 4;

            if (distSquared < 10)
            {
                tooNearCandidates.push_back(std::pair<int,int>(i,j));
            }
        }	
    }

    // Mark for removal the element of the pair with smaller perimeter
    std::vector<bool> removalMask (possibleMarkers.size(), false);

    for (size_t i=0; i<tooNearCandidates.size(); i++)
    {
        float p1 = getPerimeter(possibleMarkers[tooNearCandidates[i].first ].points);
        float p2 = getPerimeter(possibleMarkers[tooNearCandidates[i].second].points);

        size_t removalIndex;
        if (p1 > p2)
            removalIndex = tooNearCandidates[i].second;
        else
            removalIndex = tooNearCandidates[i].first;

        removalMask[removalIndex] = true;
    }

    // Return candidates
    detectedMarkers.clear();
    for (size_t i=0;i<possibleMarkers.size();i++)
    {
        if (!removalMask[i])
            detectedMarkers.push_back(possibleMarkers[i]);
    }
}

/*void MarkerDetector::findCandidates(const ContoursVector& contours,
									 std::vector<Marker>& detectedMarkers)
{
    std::vector<cv::Point> approxCurve;
    std::vector<Marker> possibleMarkers;
	
    // For each contour, analyze if it is a parallelepiped likely to be the marker
    for (size_t i=0; i<contours.size(); i++)
    {
        // Approximate to a polygon
		// Maximum distance between the original curve and its approximation
        double eps = contours[i].size() * 0.05;

		//uses Douglas-Peucker algorithm to approximate polygons
        cv::approxPolyDP(contours[i], approxCurve, eps, true);

        // Selecting only those with 4 points (corners)
        if (approxCurve.size() != 4)
            continue;

        // Selecting only convex polygons
        if (!cv::isContourConvex(approxCurve))
            continue;

        // Ensure that the distance between consecutive points is large enough
        float minDist = std::numeric_limits<float>::max();

        for (int i = 0; i < 4; i++)
        {
            cv::Point side = approxCurve[i] - approxCurve[(i+1)%4];
            //float squaredSideLength = side.dot(side);
			float squaredSideLength= std::sqrt ( ( float ) ( approxCurve[i].x-approxCurve[ ( i+1 ) %4].x ) * ( approxCurve[i].x-approxCurve[ ( i+1 ) %4].x ) +
                                             ( approxCurve[i].y-approxCurve[ ( i+1 ) %4].y ) * ( approxCurve[i].y-approxCurve[ ( i+1 ) %4].y ) );
           //std::cout<<squaredSideLength<< "    " <<test<<std::endl; 
		   minDist = std::min(minDist, squaredSideLength);
        }

        // Selecting those whose minimum side length is still greater than the limit
        if (minDist < 10)
            continue;

        // All tests are passed. Save marker candidate:
        Marker m;

        for (int i = 0; i<4; i++)
            m.points.push_back( cv::Point2f(approxCurve[i].x,approxCurve[i].y) );

        // Sort the points in anti-clockwise order
        // Trace a line between the first and second point.
        // If the third point is at the right side, then the points are anti-clockwise
        cv::Point v1 = m.points[1] - m.points[0];
        cv::Point v2 = m.points[2] - m.points[0];

        double o = (v1.x * v2.y) - (v1.y * v2.x);

        if (o < 0.0)	//if the third point is in the left side, then sort in anti-clockwise order
            std::swap(m.points[1], m.points[3]);

		

        possibleMarkers.push_back(m);
    }
	//detectedMarkers = possibleMarkers;

    // Remove these elements which corners are too close to each other.
    // First detect candidates for removal:
    std::vector< std::pair<int,int> > tooNearCandidates;
    for (size_t i=0;i<possibleMarkers.size();i++)
    {
        const Marker& m1 = possibleMarkers[i];

        //calculate the average distance of each corner to the nearest corner of the other marker candidate
        for (size_t j=i+1;j<possibleMarkers.size();j++)
        {
            const Marker& m2 = possibleMarkers[j];
            float distSquared = 0;
            for (int c = 0; c < 4; c++)
            {
				distSquared+= sqrt ( ( m1.points[c].x-m2.points[c].x ) * ( m1.points[c].x-m2.points[c].x ) +
								( m1.points[c].y-m2.points[c].y ) * ( m1.points[c].y-m2.points[c].y ) );
                //cv::Point v = m1.points[c] - m2.points[c];
                //distSquared2 += v.dot(v);
            }

            distSquared /= 4;

            if (distSquared < 10)
            {
                tooNearCandidates.push_back(std::pair<int,int>(i,j));
            }
        }	
    }

    // Mark for removal the element of the pair with smaller perimeter
    std::vector<bool> removalMask (possibleMarkers.size(), false);

    for (size_t i=0; i<tooNearCandidates.size(); i++)
    {
        float p1 = getPerimeter(possibleMarkers[tooNearCandidates[i].first ].points);
        float p2 = getPerimeter(possibleMarkers[tooNearCandidates[i].second].points);

        size_t removalIndex;
        if (p1 > p2)
            removalIndex = tooNearCandidates[i].second;
        else
            removalIndex = tooNearCandidates[i].first;

        removalMask[removalIndex] = true;
    }

    // Return candidates
    detectedMarkers.clear();
    for (size_t i=0;i<possibleMarkers.size();i++)
    {
        if (!removalMask[i])
            detectedMarkers.push_back(possibleMarkers[i]);
    }
	
}

*/
void MarkerDetector::recognizeMarkers(const cv::Mat& grayscale, std::vector<Marker>& detectedMarkers)
{
    std::vector<Marker> goodMarkers;

    // Identify the markers
    for (size_t i=0;i<detectedMarkers.size();i++)
    {
        Marker& marker = detectedMarkers[i];

        // Find the perspective transformation that brings current marker to rectangular form
        //cv::Mat markerTransform = cv::getPerspectiveTransform(marker.points, m_markerCorners2d);
		cv::Mat markerTransform = cv::findHomography(marker.points, m_markerCorners2d);

        // Transform image to get a canonical marker image
        cv::warpPerspective(grayscale, canonicalMarkerImage, markerTransform, cv::Size(markerLength,markerLength));

		
        int nRotations;
        int id = Marker::getMarkerId(canonicalMarkerImage, nRotations);
		
        if (id !=- 1)
        {
            marker.id = id;
            //sort the points so that they are always in the same order no matter the camera orientation
            std::rotate(marker.points.begin(), marker.points.begin() + 4 - nRotations, marker.points.end());
			
            goodMarkers.push_back(marker);

        }
    }

    // Refine marker corners using sub pixel accuracy
    if (goodMarkers.size() > 0)
    {
        std::vector<cv::Point2f> preciseCorners(4 * goodMarkers.size());

        for (size_t i=0; i<goodMarkers.size(); i++)
        {
            const Marker& marker = goodMarkers[i];

            for (int c = 0; c <4; c++)
            {
                preciseCorners[i*4 + c] = marker.points[c];
            }
        }
		//cv::TermCriteria termCriteria = cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 30, 0.01);
        //cv::TermCriteria termCriteria = cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 3, 0.05);
		cv::TermCriteria termCriteria = cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 30, 0.01);
        cv::cornerSubPix(grayscale, preciseCorners, cvSize(5,5), cvSize(-1,-1), termCriteria);

        // Copy refined corners position back to markers
        for (size_t i=0; i<goodMarkers.size(); i++)
        {
            Marker& marker = goodMarkers[i];

            for (int c=0;c<4;c++)
            {
                marker.points[c] = preciseCorners[i*4 + c];
            }

        }
    }
	//sort by id
    std::sort ( goodMarkers.begin(),goodMarkers.end() );

	//there might be still the case that a marker is detected twice because of the double border indicated earlier,
    //detect and remove these cases
    std::vector<bool> toRemove ( goodMarkers.size(),false );
    for ( int i=0;i<int ( goodMarkers.size() )-1;i++ )
    {
        if ( goodMarkers[i].id==goodMarkers[i+1].id && !toRemove[i+1] )
        {
			//std::cout<<"remove: "<<goodMarkers[i].id<<std::endl;
            //deletes the one with smaller perimeter
            if ( getPerimeter ( goodMarkers[i].points ) >getPerimeter ( goodMarkers[i+1].points ) ) toRemove[i+1]=true;
            else toRemove[i]=true;
        }
    }

	detectedMarkers.clear();
    for (size_t i=0;i<goodMarkers.size();i++)
    {
        if (!toRemove[i])
            detectedMarkers.push_back(goodMarkers[i]);
    }
  
}


void MarkerDetector::estimatePosition(std::vector<Marker>& detectedMarkers)
{ 
	
	
    for (size_t i=0; i<detectedMarkers.size(); i++)
    {	
        Marker& m = detectedMarkers[i];

		//calculate 9 points of the marker
		std::vector<cv::Point2f> imgPoints(9,0);
		imgPoints[0] = m.points[0];
		imgPoints[1] = midPointOfTwoPoints(m.points[0],m.points[1]);
		imgPoints[2] = m.points[1];
		imgPoints[3] = midPointOfTwoPoints(m.points[0],m.points[3]);
		imgPoints[5] = midPointOfTwoPoints(m.points[1],m.points[2]);
		imgPoints[4] = midPointOfTwoPoints(imgPoints[3],imgPoints[5]);
		imgPoints[6] = m.points[3];
		imgPoints[7] = midPointOfTwoPoints(m.points[2],m.points[3]);
		imgPoints[8] = m.points[2];
		
	
        cv::Mat Rvec;
        cv::Mat_<float> Tvec;
        cv::Mat raux,taux;
		 cv::Mat_<float> rotMat(3,3); 
        cv::solvePnP(m_markerCorners3d, imgPoints, camMatrix, distCoeff,raux,taux);
        raux.convertTo(Rvec,CV_32F);
        taux.convertTo(Tvec ,CV_32F);

		

		m.Tvec = Tvec;
		m.Rvec = Rvec;
    }
}


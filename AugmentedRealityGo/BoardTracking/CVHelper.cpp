#include "CVHelper.h"

float getPerimeter(const std::vector<cv::Point2f>& a)
{
  float sum=0, dx, dy;
  
  for (size_t i=0;i<a.size();i++)
  {
    size_t i2=(i+1) % a.size();
    
    dx = a[i].x - a[i2].x;
    dy = a[i].y - a[i2].y;
    
    sum += sqrt(dx*dx + dy*dy);
  }
  
  return sum;
}

void MyFilledCircle( cv::Mat img, cv::Point center, cv::Scalar color )
{
 int thickness = 1;
 int lineType = 8;

 circle( img,
         center,
         80/32.0,
         color,
         thickness,
         lineType );
}

cv::Mat skeleton(cv::Mat src)
{
    cv::Mat skel(src.size(), CV_8UC1, cv::Scalar(0));
    cv::Mat temp(src.size(), CV_8UC1);      
    cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
    bool done;
    do
    {
      cv::morphologyEx(src, temp, cv::MORPH_OPEN, element);
      cv::bitwise_not(temp, temp);
      cv::bitwise_and(src, temp, temp);
      cv::bitwise_or(skel, temp, skel);
      cv::erode(src, src, element);

      double max;
      cv::minMaxLoc(src, 0, &max);
      done = (max == 0);
    } while (!done);
    return skel;
    
}

cv::Point2f midPointOfTwoPoints(cv::Point2f p1, cv::Point2f p2)
{
	cv::Point2f p;
	p.x = (p1.x+p2.x)/2;
	p.y = (p1.y+p2.y)/2;
	return p;
}


cv::Mat normalizeRGBImage(cv::Mat src)
{
//see normalized rgb
	//std::vector<cv::Mat> img_rgb;
	//cv::split(undistortBoardImage,img_rgb);
	cv::Mat r;
	src.copyTo(r);
	unsigned char *input = (unsigned char*)(r.data);
	cv::MatIterator_<cv::Vec3b> it = r.begin<cv::Vec3b>(),
								it_end = r.end<cv::Vec3b>();
	for(; it != it_end; ++it)
	{
		cv::Vec3b& pixel = *it; // reference to pixel in image}
		int b = (*it)[0] ;
        int g = (*it)[1];
        int r = (*it)[2];
		double sum = sqrt((double)(r*r+g*g+b*b));
			
		(*it)[0]=b/sum*255;
		(*it)[1]=g/sum*255;
		(*it)[2]=r/sum*255;
	}
	return r;

}

void drawHisogram(std::vector<cv::Mat> src)
{
	/// Establish the number of bins
  int histSize = 256;

  /// Set the ranges ( for B,G,R) )
  float range[] = { 0, 256 } ;
  const float* histRange = { range };

  bool uniform = true; bool accumulate = false;

  cv::Mat b_hist, g_hist, r_hist;

  /// Compute the histograms:
  calcHist( &src[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &src[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &src[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

  // Draw the histograms for B, G and R
  int hist_w = 512; int hist_h = 400;
  int bin_w = cvRound( (double) hist_w/histSize );

  cv::Mat histImage( hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0) );

  /// Normalize the result to [ 0, histImage.rows ]
  normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
  normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
  normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );

  /// Draw for each channel
  for( int i = 1; i < histSize; i++ )
  {
      line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                       cv::Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                       cv::Scalar( 255, 0, 0), 2, 8, 0  );
      line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                       cv::Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                       cv::Scalar( 0, 255, 0), 2, 8, 0  );
      line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                       cv::Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                       cv::Scalar( 0, 0, 255), 2, 8, 0  );
  }

  /// Display
  cv::namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE );
  cv::imshow("calcHist Demo", histImage );
  cv::waitKey(1);
}
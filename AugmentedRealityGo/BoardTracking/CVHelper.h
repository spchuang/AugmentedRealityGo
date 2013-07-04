#ifndef CVHELPER_H
#define CVHELPER_H

#include <string>
#include <sstream>
#include <vector>
#include <opencv2/opencv.hpp>



float getPerimeter(const std::vector<cv::Point2f>& a);
void MyFilledCircle( cv::Mat img, cv::Point center, cv::Scalar color);
cv::Mat skeleton(cv::Mat src);
cv::Point2f midPointOfTwoPoints(cv::Point2f p1, cv::Point2f p2);
cv::Mat normalizeRGBImage(cv::Mat src);
void drawHisogram(std::vector<cv::Mat> src);

inline double SQ(double x)
{
    return x*x;
}

template <typename T>
std::string ToString(const T& value)
{
    std::ostringstream stream;
    stream << value;
    return stream.str();
}



namespace cv
{
    inline void showAndSave(std::string name, const cv::Mat& m)
    {
        cv::imshow(name, m);
        cv::imwrite(name + ".png", m);
    }
}



#endif
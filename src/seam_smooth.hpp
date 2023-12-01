#pragma once

#include <string>
#include <opencv2/opencv.hpp>
#include "utils.hpp"

class SeamSmooth
{
public:
    int Run(const cv::Mat& img);
    cv::Mat seam_smooth(const cv::Mat& img);

protected:

private:
    int seam_width_ = 40;
    int left_bottom_y_ = 637;

};

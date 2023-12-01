#pragma once

#include <string>
#include <opencv2/opencv.hpp>
#include "utils.hpp"

class SeamSmooth
{
public:
    int Run(const cv::Mat& img);

protected:
    cv::Mat seam_smooth(const cv::Mat& img);

private:
    int seam_width_ = 60;

};

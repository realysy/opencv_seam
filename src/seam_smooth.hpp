#pragma once

#include <string>
#include <opencv2/opencv.hpp>
#include "utils.hpp"

class SeamSmooth
{
public:
    int Run(const cv::Mat& img);

protected:
    cv::Mat gen_seam_mask(const cv::Mat& img);
    cv::Mat seam_smooth(const cv::Mat& img, const cv::Mat& mask);

private:
    std::vector<cv::Point2i> seam_line_;
    int smooth_width_ = 30;

};

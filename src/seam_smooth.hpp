#pragma once

#include <string>
#include <opencv2/opencv.hpp>
#include "utils.hpp"

class SeamSmooth
{
public:
    int Run(const cv::Mat& img);

    cv::Mat seam_smooth(const cv::Mat& img);
    cv::Mat join_left_right(const cv::Mat& img);
    void apply_left_right(const cv::Mat& img_smooth, cv::Mat& img);

    cv::Mat seam_alpha_combine(const cv::Mat& img);
    cv::Mat seam_LaplacianPyramid(const cv::Mat& img);
    cv::Mat seam_possion(const cv::Mat& img);

    cv::Mat seam_inpaint(const cv::Mat& img);
    cv::Mat gen_seam_mask(const cv::Mat& img, int half_w);
    
protected:

private:
    int seam_width_ = 300 * 2;
    int left_bottom_y_ = 637;

};

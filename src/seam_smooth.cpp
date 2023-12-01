#include "seam_smooth.hpp"

#include <iostream>
#include <string>
#include <thread>

int SeamSmooth::Run(const cv::Mat& img)
{
    printf("Input wh = %d,%d\n", img.cols, img.rows);

    cv::Mat img_smoothed = seam_smooth(img);
    cv::imwrite("../data/pano_ros_aseam_20230922_161441/seam_1.png", img_smoothed);
    // cv::namedWindow("img", cv::WINDOW_NORMAL);
    // cv::resizeWindow("img", img_smoothed.size() / 2);
    // cv::imshow("img", img_smoothed);
    // cv::waitKey(0);

    // std::cout << img_smoothed.at<
    return 0;
}

cv::Mat SeamSmooth::seam_smooth(const cv::Mat& img)
{
    cv::Mat img_smooth = img.clone();

    // 每个点的颜色 = 周围一些点的平均值, 如果8邻域某个点颜色差异较大，则忽略该邻域点
    auto smooth_color = [&] (int i, int j, int channel) {
        int color = 0;
        int count = 0;
        // 3*3 区域
        const int avg_width = 20;

        uchar color_ij = img.at<cv::Vec4b>(i, j)[channel];
        for (int m=i-avg_width; m<=i+avg_width; ++m) {
            for (int n=j-avg_width; n<=j+avg_width; ++n) {
                if (m < 0 || m >= img.rows || n < 0 || n >= img.cols) {
                    continue;
                }

                uchar color_mn = img.at<cv::Vec4b>(m, n)[channel];
                if (std::abs(color_mn - color_ij) > 15) {
                    continue;
                }

                color += img.at<cv::Vec4b>(m, n)[channel];
                count += 1;
            }
        }

        return color / count;
    };

    for (int i=0; i<img.rows; ++i) {
        for (int j=0; j<img.cols; ++j) {
            img_smooth.at<cv::Vec4b>(i, j)[0] = smooth_color(i, j, 0);
            img_smooth.at<cv::Vec4b>(i, j)[1] = smooth_color(i, j, 1);
            img_smooth.at<cv::Vec4b>(i, j)[2] = smooth_color(i, j, 2);
        }
    }

    return img_smooth;
}


#include "seam_smooth.hpp"

#include <iostream>
#include <string>
#include <thread>

int SeamSmooth::Run(const cv::Mat& img)
{
    printf("Input wh = %d,%d\n", img.cols, img.rows);

    cv::Mat img_smoothed = seam_smooth(img);
    cv::imwrite("../data/pano_ros_aseam_20230922_161441/seam_2.png", img_smoothed);
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

    auto diff_color_vec4b = [&] (const cv::Vec4b& color1, const cv::Vec4b& color2) {
        return std::pow(color1[0] - color2[0], 2) +     // b
                std::pow(color1[1] - color2[1], 2) +    // g
                std::pow(color1[2] - color2[2], 2);     // r
    };

    // 每个点的颜色 = 周围一些点的平均值, 如果8邻域某个点颜色差异较大，则忽略该邻域点
    auto smooth_color = [&] (int i, int j) {
        cv::Vec4b color_current = img.at<cv::Vec4b>(i, j);
        int color_sum_b = color_current[0];
        int color_sum_g = color_current[1];
        int color_sum_r = color_current[2];
        int count = 1;

        // 在一个小区域内求均值(比如5*5)
        const int avg_height = 5;
        const int avg_width = 10;
        for (int m=i-avg_height; m<=i+avg_height; ++m) {
            for (int n=j-avg_width; n<=j+avg_width; ++n) {
                if (m < 0 || m >= img.rows || n < 0 || n >= img.cols) {
                    continue;
                }

                // 颜色差异过大则不用于求均值
                cv::Vec4b color_around = img.at<cv::Vec4b>(m, n);
                if (diff_color_vec4b(color_current, color_around) > 440) {
                    continue;
                }

                color_sum_b += color_around[0];
                color_sum_g += color_around[1];
                color_sum_r += color_around[2];
                count += 1;
            }
        }

        cv::Vec4b color_result(color_sum_b/count, color_sum_g/count, color_sum_r/count, color_current[3]);
        return color_result;
    };

    for (int i=0; i<img.rows; ++i) {
        for (int j=0; j<img.cols; ++j) {
            img_smooth.at<cv::Vec4b>(i, j) = smooth_color(i, j);
        }
    }

    return img_smooth;
}


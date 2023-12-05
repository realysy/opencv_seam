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

/*
img: final pano
*/
cv::Mat SeamSmooth::join_left_right(const cv::Mat& img)
{
    cv::Mat img_seam(cv::Size2i(seam_width_, img.rows), img.type());
    const int half_width = seam_width_ / 2;
    img.rowRange(0, img.rows).colRange(img.cols - half_width, img.cols).copyTo(img_seam(cv::Rect(0,0, half_width, img.rows)));
    img.rowRange(0, img.rows).colRange(0, half_width).copyTo(img_seam(cv::Rect(half_width, 0, half_width, img.rows)));

    return img_seam;
}

/*
img: join of left and right
*/
cv::Mat SeamSmooth::seam_smooth(const cv::Mat& img)
{
    cv::Mat img_smooth = img.clone();

    auto diff_color_vec4b = [&] (const cv::Vec4b& color1, const cv::Vec4b& color2) {
        return std::pow(color1[0] - color2[0], 2) +     // b
                std::pow(color1[1] - color2[1], 2) +    // g
                std::pow(color1[2] - color2[2], 2);     // r
    };
    auto diff_color_vec3b_single = [&] (const cv::Vec3b& color1, const cv::Vec3b& color2, int channel) {
        return std::abs(color1[channel] - color2[channel]);
    };
    auto diff_color_vec3b_two = [&] (const cv::Vec3b& color1, const cv::Vec3b& color2, int c1, int c2) {
        return std::pow(color1[c1] - color2[c1], 2) +
                std::pow(color1[c2] - color2[c2], 2);
    };

    // 每个点的颜色 = 周围一些点的平均值, 如果8邻域某个点颜色差异较大，则忽略该邻域点
    auto smooth_color = [&] (int i, int j, int kernel_w, int kernel_h, int threshold) {
        cv::Vec4b color_current = img.at<cv::Vec4b>(i, j);
        int color_sum_b = color_current[0];
        int color_sum_g = color_current[1];
        int color_sum_r = color_current[2];
        int count = 1;

        // 在一个小区域内求均值(比如5*5)
        for (int m=i-kernel_h; m<=i+kernel_h; ++m) {
            for (int n=j-kernel_w; n<=j+kernel_w; ++n) {
                if (m < 0 || m >= img.rows || n < 0 || n >= img.cols) {
                    continue;
                }

                // 颜色差异过大则不用于求均值
                cv::Vec4b color_around = img.at<cv::Vec4b>(m, n);
                if (diff_color_vec4b(color_current, color_around) > threshold) {
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

    // 有效图像部分
    for (int i = 0; i < left_bottom_y_; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            img_smooth.at<cv::Vec4b>(i, j) = smooth_color(i, j, 10, 5, 440);
        }
    }

    // 拉伸模糊的部分
    const int kernel_h = 1;
    const int target_i = std::min(left_bottom_y_ + kernel_h + 10, img.rows - kernel_h - 10);
    for (int j = 0; j < img.cols; ++j) {
        cv::Vec4b new_color = smooth_color(target_i, j, 10, kernel_h, 2500);
        img_smooth.rowRange(left_bottom_y_, img.rows).col(j) = new_color;
    }

    return img_smooth;
}

void SeamSmooth::apply_left_right(const cv::Mat& img_smooth, cv::Mat& img)
{
    const int half_width = seam_width_ / 2;
    img_smooth.rowRange(0, img_smooth.rows).colRange(0, half_width).copyTo(img(cv::Rect(img.cols - half_width, 0, half_width, img_smooth.rows)));
    img_smooth.rowRange(0, img_smooth.rows).colRange(half_width, img_smooth.cols).copyTo(img(cv::Rect(0, 0, half_width, img_smooth.rows)));
}



cv::Mat SeamSmooth::seam_inpaint(const cv::Mat& img)
{
    cv::Mat mask = gen_seam_mask(img);
    cv::imwrite("./mask.png", mask);
    
    cv::Mat img_3;
    cv::cvtColor(img, img_3, cv::COLOR_BGRA2BGR);
    cv::Mat img_smooth;
    int radius = std::min(9, seam_width_ / 2);
    cv::inpaint(img_3, mask, img_smooth, radius, cv::INPAINT_TELEA);
    cv::imwrite("./img_smooth.png", img_smooth);

    cv::cvtColor(img_smooth, img_smooth, cv::COLOR_BGR2BGRA);
    return img_smooth;
}

// call once
cv::Mat SeamSmooth::gen_seam_mask(const cv::Mat& img)
{
    cv::Mat mask = cv::Mat::zeros(img.size(), CV_8UC1);
    int line_x = img.cols / 2;
    int line_half_w = std::min(40, seam_width_ / 2);
    mask.rowRange(0, img.rows).colRange(line_x - line_half_w, line_x + line_half_w) = 255;

    return mask;
}



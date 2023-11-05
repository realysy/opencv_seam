#include "seam_smooth.hpp"

#include <iostream>
#include <string>
#include <thread>

int SeamSmooth::Run(const cv::Mat& img)
{
    printf("Input wh = %d,%d\n", img.cols, img.rows);
    for (int r=0; r<img.rows; ++r) {
        seam_line_.emplace_back(cv::Point2i(302, r));
    }

    cv::Mat mask = gen_seam_mask(img);
    cv::Mat img_smoothed = seam_smooth(img, mask);

    cv::imshow("Filtered Image", img_smoothed);
    cv::waitKey(0);


    return 0;
}

cv::Mat SeamSmooth::gen_seam_mask(const cv::Mat& img)
{
    cv::Mat mask = cv::Mat::zeros(img.size(), CV_8UC1);

    // 在mask上画出一条连续的线段
    int thickness = 1; // 线段宽度
    for (int i = 0; i < seam_line_.size() - 1; i++) {
        cv::line(mask, seam_line_[i], seam_line_[i + 1], cv::Scalar(255), thickness);
    }

    // 对mask进行膨胀操作
    int dilation_size = smooth_width_; // 膨胀卷积核大小
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
                        cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1),
                        cv::Point(dilation_size, dilation_size));
    cv::dilate(mask, mask, element);

    return mask;
}

cv::Mat SeamSmooth::seam_smooth(const cv::Mat& img, const cv::Mat& mask)
{
    cv::Mat img_target = cv::Mat::zeros(img.size(), img.type());
    img.copyTo(img_target, mask);

    auto smooth_color = [&] (int i, int j, int channel) {
        int color = 0;
        int count = 0;
        // 3*3 区域
        const int avg_width = 10;
        for (int m=i-avg_width; m<=i+avg_width; ++m) {
            for (int n=j-avg_width; n<=j+avg_width; ++n) {
                if (m < 0 || m >= img.rows || n < 0 || n >= img.cols) {
                    continue;
                }

                color += img.at<cv::Vec3b>(m, n)[channel];
                count += 1;
            }
        }

        return color / count;
    };

    for (int i=0; i<mask.rows; ++i) {
        for (int j=0; j<mask.cols; ++j) {
            if (mask.at<uchar>(i, j) == 0) {
                continue;
            }

            // 每个点的颜色 = 周围9个点的平均值
            img_target.at<cv::Vec3b>(i, j)[0] = smooth_color(i, j, 0);
            img_target.at<cv::Vec3b>(i, j)[1] = smooth_color(i, j, 1);
            img_target.at<cv::Vec3b>(i, j)[2] = smooth_color(i, j, 2);
        }
    }



    img.copyTo(img_target, (255 - mask));
    return img_target;
}


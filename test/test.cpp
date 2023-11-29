#include "test.hpp"
#include "seam_smooth.hpp"
// #include "utils.hpp"

#include <iostream>
#include <string>
#include <thread>

// #include <opencv2/opencv.hpp>

int main(int argc, char* argv[])
{
    std::string path_img = "../data/test3.png";
    cv::Mat img = cv::imread(path_img);
    
    // xy=(357,425), (385,425)
    // BGR
    uchar b1 = img.at<cv::Vec3b>(425, 357)[0];
    uchar g1 = img.at<cv::Vec3b>(425, 357)[1];
    uchar r1 = img.at<cv::Vec3b>(425, 357)[2];
    uchar b2 = img.at<cv::Vec3b>(425, 385)[0];
    uchar g2 = img.at<cv::Vec3b>(425, 385)[1];
    uchar r2 = img.at<cv::Vec3b>(425, 385)[2];

    using std::pow;
    unsigned dist_bgr = pow(b2-b1, 2) + pow(g2-g1, 2) + pow(r2-r1, 2);

    cv::Mat img_hsv;
    cv::cvtColor(img, img_hsv, cv::COLOR_BGR2HSV);
    cv::Mat img_hls;
    cv::cvtColor(img, img_hls, cv::COLOR_BGR2HLS);

    uchar hsv_h1 = img_hsv.at<cv::Vec3b>(425, 357)[0];
    uchar hsv_s1 = img_hsv.at<cv::Vec3b>(425, 357)[1];
    uchar hsv_v1 = img_hsv.at<cv::Vec3b>(425, 357)[2];
    uchar hsv_h2 = img_hsv.at<cv::Vec3b>(425, 385)[0];
    uchar hsv_s2 = img_hsv.at<cv::Vec3b>(425, 385)[1];
    uchar hsv_v2 = img_hsv.at<cv::Vec3b>(425, 385)[2];
    unsigned dist_hsv = pow(hsv_h2-hsv_h1, 2) + pow(hsv_s2-hsv_s1, 2) + pow(hsv_v2-hsv_v1, 2);

    uchar hls_h1 = img_hls.at<cv::Vec3b>(425, 357)[0];
    uchar hls_l1 = img_hls.at<cv::Vec3b>(425, 357)[1];
    uchar hls_s1 = img_hls.at<cv::Vec3b>(425, 357)[2];
    uchar hls_h2 = img_hls.at<cv::Vec3b>(425, 385)[0];
    uchar hls_l2 = img_hls.at<cv::Vec3b>(425, 385)[1];
    uchar hls_s2 = img_hls.at<cv::Vec3b>(425, 385)[2];
    unsigned dist_hls = pow(hls_h2-hls_h1, 2) + pow(hls_s2-hls_s1, 2) + pow(hls_l2-hls_l1, 2);

    printf("BGR: (%u,%u,%u), (%u,%u,%u), dist = %u\n", b1, g1, r1, b2, g2, r2,   dist_hsv);
    printf("HSV: (%u,%u,%u), (%u,%u,%u), dist = %u, v_dist = %u\n", hsv_h1, hsv_s1, hsv_v1, hsv_h2, hsv_s2, hsv_v2,   dist_hsv,   hsv_v2-hsv_v1);
    printf("HLS: (%u,%u,%u), (%u,%u,%u), dist = %u, v_dist = %u\n", hls_h1, hls_s1, hls_l1, hls_h2, hls_s2, hls_l2,   dist_hls,   hls_l2-hls_l1);
    // TODO 采用hsv空间。仅平滑: h/s相近, v差异较大的(6)

    SeamSmooth seam_smoother;
    seam_smoother.Run(img);

    return 0;
}

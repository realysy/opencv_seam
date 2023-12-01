#include "test.hpp"
#include "seam_smooth.hpp"
// #include "utils.hpp"

#include <iostream>
#include <string>
#include <thread>


int main(int argc, char* argv[])
{
    std::string path_img = "../data/pano_ros_aseam_20230922_161441/aa.tif";
    // std::string path_img = "../data/2-0-16.tif";

    cv::Mat img = cv::imread(path_img, cv::IMREAD_UNCHANGED);
    cv::Mat img_seam = join_left_right(img);

    // cv::Mat img_hls;
    // cv::cvtColor(img_seam, img_hls, cv::COLOR_BGR2HLS);
    // std::vector<cv::Mat> channel_hls;
    // cv::split(img_hls, channel_hls);
    // cv::imwrite("../data/pano_ros_aseam_20230922_161441/hls_H.png", channel_hls[0]);
    // cv::imwrite("../data/pano_ros_aseam_20230922_161441/hls_L.png", channel_hls[1]);
    // cv::imwrite("../data/pano_ros_aseam_20230922_161441/hls_S.png", channel_hls[2]);
    // exit(0);
    // std::cout << "point2 = \n" << point2 << std::endl;

    SeamSmooth seam_smoother;
    seam_smoother.Run(img_seam);

    return 0;
}

#include "test.hpp"
#include "seam_smooth.hpp"
// #include "utils.hpp"

#include <iostream>
#include <string>
#include <thread>


int main(int argc, char* argv[])
{
    std::vector<std::string> folders = {
        "../data/pano_ros_aseam_20230922_161441", 
        "../data/pano_ros_aseam_20231130_105234", 
        "../data/pano_ros_bak_20231130_091450_1992", 
        "../data/pano_ros_bak_20231130_104537_75310", 
        "../data/pano_ros_bak_20231130_104728_73912", 
        "../data/pano_ros_bak_20231130_104906_1321", 
        "../data/pano_ros_bak_20231130_105058_49283", 
        "../data/pano_ros_bak_20231130_105420_86502", 
    };
    // std::string path_img = "../data/2-0-16.tif";

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

    // cv::namedWindow("compare", cv::WINDOW_NORMAL);
    // cv::resizeWindow("compare", cv::Size(100, 800));

    double time_start = get_time();
    for (const std::string& folder : folders) {
        std::string path_img = folder + "/aa.tif";
        std::string path_seam_raw = folder + "/seam_0.png";
        std::string path_seam_smooth = folder + "/seam_2-1.png";

        cv::Mat img = cv::imread(path_img, cv::IMREAD_UNCHANGED);
        cv::Mat img_seam = join_left_right(img);
        cv::Mat img_smooth = seam_smoother.seam_smooth(img_seam);

        // cv::imwrite(path_seam_raw, img_seam);
        // cv::imwrite(path_seam_smooth, img_smooth);

        // cv::Mat img_compare(cv::Size2i(img_seam.cols * 2 + 20, img_seam.rows), img_seam.type(), cv::Scalar(0,0,0,0));
        // img_seam.copyTo(img_compare.rowRange(0, img_seam.rows).colRange(0, img_seam.cols));
        // img_smooth.copyTo(img_compare.rowRange(0, img_seam.rows).colRange(img_seam.cols + 20, img_compare.cols));
        // cv::imshow("compare", img_compare);
        // cv::waitKey(0);
    }
    
    double time_end = get_time();
    printf("time cost = %lf\n", (time_end - time_start) / folders.size());


    return 0;
}

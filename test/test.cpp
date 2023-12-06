#include "test.hpp"
#include "seam_smooth.hpp"
// #include "utils.hpp"

#include <iostream>
#include <string>
#include <thread>


int main(int argc, char* argv[])
{
    std::vector<std::string> folders = {
        "../../../syyan2/HH_Panorama/scenes/12.04-seam_test/engine_dump/pano_ros_bak_20231204_143225_67964", 
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
        printf("%s\n", path_img.c_str());
        std::string path_seam_raw = folder + "/seam_0.png";
        std::string path_seam_smooth = folder + "/seam_2-1.png";

        cv::Mat img = cv::imread(path_img, cv::IMREAD_UNCHANGED);

        // cv::imwrite(folder + "/left.tif", img(cv::Rect(0, 0, img.cols/2, img.rows)));
        // cv::imwrite(folder + "/right.tif", img(cv::Rect(img.cols/2, 0, img.cols/2, img.rows)));

        int vis_width = 200;
        cv::Mat img_compare(cv::Size2i(vis_width * 4 + 30, img.rows), img.type(), cv::Scalar(0,0,0,0));
        // 平滑前的接缝
        img.rowRange(0, img.rows).colRange(img.cols - vis_width, img.cols).copyTo(img_compare.rowRange(0, img_compare.rows).colRange(0, vis_width));
        img.rowRange(0, img.rows).colRange(0, vis_width).copyTo(img_compare(cv::Rect(vis_width, 0, vis_width, img.rows)));

        cv::Mat img_seam = seam_smoother.join_left_right(img);
        // cv::Mat img_smooth = seam_smoother.seam_smooth(img_seam);
        cv::Mat img_smooth = seam_smoother.seam_possion(img_seam);
        seam_smoother.apply_left_right(img_smooth, img);

        // cv::imwrite(path_seam_raw, img_seam);
        // cv::imwrite(path_seam_smooth, img_smooth);

        // 平滑后的接缝
        img.rowRange(0, img.rows).colRange(img.cols - vis_width, img.cols).copyTo(img_compare(cv::Rect(vis_width * 2 + 30, 0, vis_width, img.rows)));
        img.rowRange(0, img.rows).colRange(0, vis_width).copyTo(img_compare(cv::Rect(vis_width * 3 + 30, 0, vis_width, img.rows)));
        cv::namedWindow("compare", cv::WINDOW_NORMAL);
        cv::resizeWindow("compare", img_compare.size() / 2);
        cv::imshow("compare", img_compare);
        char key = cv::waitKey(0);
        if (key == 'q') {
            break;
        }
    }
    
    double time_end = get_time();
    printf("time cost = %lf\n", (time_end - time_start) / folders.size());


    return 0;
}

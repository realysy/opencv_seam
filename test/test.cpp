#include "test.hpp"
#include "seam_smooth.hpp"

#include <iostream>
#include <string>
#include <thread>

// #include <opencv2/opencv.hpp>

int main(int argc, char* argv[])
{
    std::string path_img = "../data/test.png";
    cv::Mat image = cv::imread(path_img);
    
    SeamSmooth seam_smoother;
    seam_smoother.Run(image);

  
    return 0;
}

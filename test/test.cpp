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
    cv::Mat image = cv::imread(path_img);
    
    SeamSmooth seam_smoother;
    seam_smoother.Run(image);



    double R = 0;
    double G = 0;
    double B = 0;
    if (argc == 4) {
        R = std::atof(argv[1]);
        G = std::atof(argv[2]);
        B = std::atof(argv[3]);
    }

    double H, S, L;
    RGB2HSL(R, G, B, H, S, L);
    printf("HSL = %lf, %lf, %lf\n", H*255, S*255, L*255);

    return 0;
}

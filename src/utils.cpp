#include "utils.hpp"

/* 
RGB: 0~255
*/
void RGB2HSL(double R,double G,double B, double &H,double &S,double &L)
{
    double Max,Min,del_R,del_G,del_B,del_Max;
    R /= 255.0;       //Where RGB values = 0 ÷ 255
    G /= 255.0;
    B /= 255.0;

    Min = std::min(R, std::min(G, B));    //Min. value of RGB
    Max = std::max(R, std::max(G, B));    //Max. value of RGB
    del_Max = Max - Min;        //Delta RGB value

    L = (Max + Min) / 2.0;

    if (del_Max == 0)           //This is a gray, no chroma...
    {
        //H = 2.0/3.0;          //Windows下S值为0时，H值始终为160（2/3*240）
        H = 0;                  //HSL results = 0 ÷ 1
        S = 0;
    }
    else                        //Chromatic data...
    {
        if (L < 0.5) S = del_Max / (Max + Min);
        else         S = del_Max / (2 - Max - Min);

        del_R = (((Max - R) / 6.0) + (del_Max / 2.0)) / del_Max;
        del_G = (((Max - G) / 6.0) + (del_Max / 2.0)) / del_Max;
        del_B = (((Max - B) / 6.0) + (del_Max / 2.0)) / del_Max;

        if      (R == Max) H = del_B - del_G;
        else if (G == Max) H = (1.0 / 3.0) + del_R - del_B;
        else if (B == Max) H = (2.0 / 3.0) + del_G - del_R;

        if (H < 0)  H += 1;
        if (H > 1)  H -= 1;
    }
}

double Hue2RGB(double v1, double v2, double vH)
{
    if (vH < 0) vH += 1;
    if (vH > 1) vH -= 1;
    if (6.0 * vH < 1) return v1 + (v2 - v1) * 6.0 * vH;
    if (2.0 * vH < 1) return v2;
    if (3.0 * vH < 2) return v1 + (v2 - v1) * ((2.0 / 3.0) - vH) * 6.0;
    return (v1);
}

void HSL2RGB(double H,double S,double L, double &R,double &G,double &B)
{
    double var_1, var_2;
    if (S == 0)                       //HSL values = 0 ÷ 1
    {
        R = L * 255.0;                   //RGB results = 0 ÷ 255
        G = L * 255.0;
        B = L * 255.0;
    }
    else
    {
        if (L < 0.5) var_2 = L * (1 + S);
        else         var_2 = (L + S) - (S * L);

        var_1 = 2.0 * L - var_2;

        R = 255.0 * Hue2RGB(var_1, var_2, H + (1.0 / 3.0));
        G = 255.0 * Hue2RGB(var_1, var_2, H);
        B = 255.0 * Hue2RGB(var_1, var_2, H - (1.0 / 3.0));
    }
}

cv::Mat join_left_right(const cv::Mat& img)
{
    int seam_width = 40;
    cv::Mat img_seam(cv::Size2i(seam_width, img.rows), img.type());
    img.rowRange(0, img.rows).colRange(img.cols - seam_width/2, img.cols).copyTo(img_seam(cv::Rect(0,0, seam_width/2, img.rows)));
    img.rowRange(0, img.rows).colRange(0, seam_width/2).copyTo(img_seam(cv::Rect(seam_width/2, 0, seam_width/2, img.rows)));

    return img_seam;
}

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


/* 方案: 
 * left的右边一点.flip, 按权重添加到right的左侧
 * right的左边一点.flip, 按权重添加到left的右侧
 * 权重: 越远离接缝, 原图的权重越大(1.0)； 约接近接缝, 原图的权重越小(0.5)

*/
cv::Mat SeamSmooth::seam_alpha_combine(const cv::Mat& img)
{
    const int blendWidth = 10;
    const int half_w = seam_width_ / 2;

    const float max_alpha = 1.0;
    const float min_alpha = 0.5;
    const float delta = max_alpha - min_alpha;
    // max ... min
    std::vector<float> weights_left(blendWidth);
    for (int j = 0; j < blendWidth; ++j) {
        weights_left[j] = max_alpha - delta * j / blendWidth;
    }
    std::vector<float> weights_right = weights_left;
    std::reverse(weights_right.begin(), weights_right.end());

    cv::Mat out_img = img.clone();
    cv::Mat seam_left = out_img.colRange(half_w - blendWidth, half_w);
    cv::Mat seam_right = out_img.colRange(half_w, half_w + blendWidth);
    cv::Mat seam_left_flip;
    cv::Mat seam_right_flip;
    cv::flip(seam_left, seam_left_flip, 1);
    cv::flip(seam_right, seam_right_flip, 1);

    // seam_left = seam_left.mul(mask_left) + seam_right_flip.mul(1 - mask_left);
    // seam_right = seam_right.mul(mask_right) + seam_left_flip.mul(1 - mask_right);

    // left
    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < blendWidth; ++j) {
            seam_left.at<cv::Vec4b>(i, j) = seam_left.at<cv::Vec4b>(i, j) * weights_left[j] + seam_right_flip.at<cv::Vec4b>(i, j) * (1 - weights_left[j]);
            seam_right.at<cv::Vec4b>(i, j) = seam_right.at<cv::Vec4b>(i, j) * weights_right[j] + seam_left_flip.at<cv::Vec4b>(i, j) * (1 - weights_right[j]);
        }
    }

    // seam_left.copyTo(out_img(cv::Rect(half_w - blendWidth, 0, blendWidth, img.rows)));
    // seam_right.copyTo(out_img(cv::Rect(half_w, 0, blendWidth, img.rows)));

    return out_img;
}

/* 方案：对接缝位置平滑
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
            img_smooth.at<cv::Vec4b>(i, j) = smooth_color(i, j, 10, 5, 600);
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


/*
方案1: 接缝附近都是白色
空白dst, right粘贴到右边，left clone到左边，right clone到右边

方案2: 接缝附近变成黑白; 接缝被模糊了
dst = img.clone(); 缝高斯模糊；clone right, clone left

方案3: 
dst = left+left.flip; clone right, mask只设置接缝位置

方案3: 
方案3: 
方案3: 
*/
cv::Mat SeamSmooth::seam_possion(const cv::Mat& img)
{
    const int half_w = seam_width_ / 2;

    cv::Mat dst(img.size(), img.type(), cv::Scalar(255,255,255,0));
    cv::imwrite("./dst.png", dst);

    // 底图全图进行高斯模糊
    // cv::Mat dst_mid = dst(cv::Rect(half_w - 40, 0, 80, dst.rows));
    // cv::Mat dst_gauss;
    // cv::GaussianBlur(dst_mid, dst_gauss, cv::Size(17, 17), 15.0, 15.0);
    // dst_gauss.copyTo(dst(cv::Rect(half_w - 40, 0, 80, dst.rows)));
    // cv::imwrite("./dst_gauss.png", dst);

    cv::Mat right = img.rowRange(0, img.rows).colRange(img.cols - half_w, img.cols).clone();
    cv::Mat left = img.rowRange(0, img.rows).colRange(0, half_w).clone();

    cv::Mat mask = cv::Mat::zeros(right.size(), CV_8UC1);
    // mask.rowRange(0, mask.rows).colRange(half_w - 10, half_w + 10) = 255;
    mask += 255;

    cv::imwrite("./src.png", right);
    cv::imwrite("./mask.png", mask);

    cv::seamlessClone(right, dst, mask, cv::Point2i(half_w + half_w / 2, dst.rows / 2), dst, 3);
    cv::imwrite("./result_0.png", dst);
    cv::seamlessClone(left, dst, mask, cv::Point2i(half_w / 2, dst.rows / 2), dst, 3);
    cv::imwrite("./result_1.png", dst);
    cv::seamlessClone(right, dst, mask, cv::Point2i(half_w + half_w / 2, dst.rows / 2), dst, 3);
    cv::imwrite("./result_2.png", dst);

    return dst;
}

void SeamSmooth::apply_left_right(const cv::Mat& img_smooth, cv::Mat& img)
{
    const int half_width = seam_width_ / 2;
    img_smooth.rowRange(0, img_smooth.rows).colRange(0, half_width).copyTo(img(cv::Rect(img.cols - half_width, 0, half_width, img_smooth.rows)));
    img_smooth.rowRange(0, img_smooth.rows).colRange(half_width, img_smooth.cols).copyTo(img(cv::Rect(0, 0, half_width, img_smooth.rows)));
}



cv::Mat SeamSmooth::seam_inpaint(const cv::Mat& img)
{
    cv::Mat mask = gen_seam_mask(img, 40);
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
cv::Mat SeamSmooth::gen_seam_mask(const cv::Mat& img, int half_w)
{
    cv::Mat mask = cv::Mat::zeros(img.size(), CV_8UC1);
    int line_x = img.cols / 2;
    int line_half_w = std::min(half_w, seam_width_ / 2);
    mask.rowRange(0, img.rows).colRange(line_x - line_half_w, line_x + line_half_w) = 255;

    return mask;
}


/* cv::distanceTransform()方案
* 分别计算两张图的dist transform
* 接缝附近的transform进行融合，得到最终alpha
*/
cv::Mat SeamSmooth::seam_LaplacianPyramid(const cv::Mat& img)
{
    cv::Mat gray;

    const int blendWidth = 50;
    const int half_w = seam_width_ / 2;
    cv::Mat left = img.rowRange(0, img.rows).colRange(0, half_w);
    cv::Mat right = img.rowRange(0, img.rows).colRange(img.cols - half_w, img.cols);
    
    // 生成拉普拉斯金字塔
    std::vector<cv::Mat> leftPyramid, rightPyramid;
    cv::Mat leftGaussian = left.clone();
    cv::Mat rightGaussian = right.clone();

    for (int i = 0; i < 3; ++i) {
        cv::Mat leftDown, rightDown;
        cv::pyrDown(leftGaussian, leftDown);
        cv::pyrDown(rightGaussian, rightDown);
        
        cv::Mat leftUp, rightUp;
        cv::pyrUp(leftDown, leftUp, leftGaussian.size());
        cv::pyrUp(rightDown, rightUp, rightGaussian.size());

        cv::Mat leftDiff = leftGaussian - leftUp;
        cv::Mat rightDiff = rightGaussian - rightUp;

        leftPyramid.push_back(leftDiff);
        rightPyramid.push_back(rightDiff);

        leftGaussian = leftDown;
        rightGaussian = rightDown;
    }

    // 获取金字塔的层数
    int numLevels = leftPyramid.size();

    // 拼接金字塔
    std::vector<cv::Mat> blendedPyramid(numLevels);
    for (int i = 0; i < numLevels; ++i) {
        int blendWidthLevel = blendWidth * (1 << (numLevels - i - 1));

        cv::Mat blendedLevel;
        cv::addWeighted(leftPyramid[i].colRange(0, leftPyramid[i].cols - blendWidthLevel),
                        1.0, rightPyramid[i].colRange(blendWidthLevel, rightPyramid[i].cols), 1.0,
                        0.0, blendedLevel);
        blendedPyramid[i] = blendedLevel;
    }

    // 重建图像
    cv::Mat blendedImage = blendedPyramid[numLevels - 1];
    for (int i = numLevels - 2; i >= 0; --i) {
        cv::pyrUp(blendedImage, blendedImage);
        if (blendedImage.size() != blendedPyramid[i].size()) {
            cv::resize(blendedImage, blendedImage, blendedPyramid[i].size());
        }

        cv::add(blendedPyramid[i], blendedImage, blendedImage);
    }
    cv::imwrite("./pyramid.png", blendedImage);

    cv::Mat seam_left = left.colRange(left.cols - blendWidth, left.cols);
    seam_left = seam_left.mul(blendedImage.colRange(0, blendWidth));
    cv::Mat seam_right = right.colRange(0, blendWidth);
    seam_right = seam_right.mul(blendedImage.colRange(blendWidth, blendedImage.cols));
    cv::imwrite("./seam_left.png", seam_left);
    cv::imwrite("./seam_right.png", seam_right);

    cv::Mat out_mat = img.clone();
    seam_left.copyTo(out_mat(cv::Rect(half_w - blendWidth, 0, blendWidth, img.rows)));
    seam_right.copyTo(out_mat(cv::Rect(half_w, 0, blendWidth, img.rows)));
    return out_mat;
}




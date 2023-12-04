#pragma once

#include <algorithm>
#include <vector>
#include <set>
#include <map>


#include <fmt/core.h>
#include <fmt/format.h>

#include <opencv2/opencv.hpp>

#ifdef WIN32
#include <inttypes.h>
#include <time.h>
#else
#include <sys/time.h>
#include <sys/resource.h>
#endif

static double get_time(void) {
#ifdef WIN32
    struct timespec tv;
    timespec_get(&tv, 1);
    return (double)((int64_t)tv.tv_sec * 1000 + (int64_t)tv.tv_nsec / 1000000.0);
#else
    timeval tv;
    gettimeofday(&tv, 0);
    return (double)(tv.tv_sec * 1000000 + tv.tv_usec) / 1000.0;
#endif  /* WIN32 */
}

void RGB2HSL(double R,double G,double B, double &H,double &S,double &L);
double Hue2RGB(double v1, double v2, double vH);
void HSL2RGB(double H,double S,double L, double &R,double &G,double &B);

template<typename Tp>
void print_vec(const std::vector<Tp>& vec)
{
    fmt::print("[{}]\n", fmt::join(vec, ", "));
}

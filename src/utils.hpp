#pragma once

#include <algorithm>
#include <vector>
#include <set>
#include <map>


#include <fmt/core.h>
#include <fmt/format.h>

void RGB2HSL(double R,double G,double B, double &H,double &S,double &L);
double Hue2RGB(double v1, double v2, double vH);
void HSL2RGB(double H,double S,double L, double &R,double &G,double &B);

template<typename Tp>
void print_vec(const std::vector<Tp>& vec)
{
    fmt::print("[{}]\n", fmt::join(vec, ", "));
}
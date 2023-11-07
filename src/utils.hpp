#pragma once

#include <algorithm>

void RGB2HSL(double R,double G,double B, double &H,double &S,double &L);
double Hue2RGB(double v1, double v2, double vH);
void HSL2RGB(double H,double S,double L, double &R,double &G,double &B);

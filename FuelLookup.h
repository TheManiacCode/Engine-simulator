#pragma once

#include "FuelMap.h"
#include <algorithm>

inline double bilinearInterpolate(double x0, double x1, double y0, double y1,
                                  double f00, double f10, double f01, double f11,
                                  double x, double y)
{
    if (x1 == x0 && y1 == y0) return f00;
    if (x1 == x0) {
        double t = (y - y0) / (y1 - y0);
        return f00 + t * (f01 - f00);
    }
    if (y1 == y0) {
        double s = (x - x0) / (x1 - x0);
        return f00 + s * (f10 - f00);
    }

    double s = (x - x0) / (x1 - x0);
    double t = (y - y0) / (y1 - y0);

    double fxy0 = f00 + s * (f10 - f00);
    double fxy1 = f01 + s * (f11 - f01);
    return fxy0 + t * (fxy1 - fxy0);
}

inline double lookupFuelPulseWidth(double rpm, double load)
{
    rpm = std::clamp(rpm, FuelMap::rpmAxis[0], FuelMap::rpmAxis[FuelMap::rpmPoints - 1]);
    load = std::clamp(load, FuelMap::loadAxis[0], FuelMap::loadAxis[FuelMap::loadPoints - 1]);

    int ri = 0;
    for (int i = 0; i < FuelMap::rpmPoints - 1; ++i) {
        if (rpm >= FuelMap::rpmAxis[i] && rpm <= FuelMap::rpmAxis[i+1]) { ri = i; break; }
    }

    int li = 0;
    for (int j = 0; j < FuelMap::loadPoints - 1; ++j) {
        if (load >= FuelMap::loadAxis[j] && load <= FuelMap::loadAxis[j+1]) { li = j; break; }
    }

    double x0 = FuelMap::rpmAxis[ri];
    double x1 = FuelMap::rpmAxis[ri+1];
    double y0 = FuelMap::loadAxis[li];
    double y1 = FuelMap::loadAxis[li+1];

    double f00 = FuelMap::pulseWidth[ri][li];
    double f10 = FuelMap::pulseWidth[ri+1][li];
    double f01 = FuelMap::pulseWidth[ri][li+1];
    double f11 = FuelMap::pulseWidth[ri+1][li+1];

    return bilinearInterpolate(x0, x1, y0, y1, f00, f10, f01, f11, rpm, load);
}
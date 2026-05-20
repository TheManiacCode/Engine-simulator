#include "CoolingSystem.h"
#include <algorithm>

CoolingSystem::CoolingSystem()
    : coolantTemp_(90.0), oilTemp_(95.0), exhaustTemp_(260.0)
{
}

void CoolingSystem::update(double dt, double engineLoad, double ambientTemp, double turboTemp)
{
    double heatInput = 15.0 * engineLoad + 0.05 * turboTemp;
    coolantTemp_ += (heatInput - (coolantTemp_ - ambientTemp) * 3.0) * dt;
    coolantTemp_ = std::clamp(coolantTemp_, ambientTemp, 130.0);

    oilTemp_ += ((coolantTemp_ + 15.0) - oilTemp_) * dt * 0.4;
    oilTemp_ = std::clamp(oilTemp_, ambientTemp, 150.0);

    exhaustTemp_ += ((220.0 + engineLoad * 250.0) - exhaustTemp_) * dt * 0.3;
    exhaustTemp_ = std::clamp(exhaustTemp_, 220.0, 820.0);
}

double CoolingSystem::coolantTemp() const
{
    return coolantTemp_;
}

double CoolingSystem::oilTemp() const
{
    return oilTemp_;
}

double CoolingSystem::exhaustTemp() const
{
    return exhaustTemp_;
}

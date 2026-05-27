#include "CoolingSystem.h"
#include <algorithm>

CoolingSystem::CoolingSystem(CoolantFluidType type)
    : coolantTemp_(90.0), oilTemp_(95.0), exhaustTemp_(260.0), coolantType_(type)
{
}

void CoolingSystem::update(double dt, double engineLoad, double ambientTemp, double turboTemp)
{
    double coolantDissipation = 3.0;
    double coolantMax = 130.0;
    double oilCoolingRate = 0.4;
    double exhaustCoolingRate = 0.3;

    switch (coolantType_) {
        case CoolantFluidType::Standard:
            coolantDissipation = 3.0;
            coolantMax = 130.0;
            oilCoolingRate = 0.4;
            exhaustCoolingRate = 0.3;
            break;
        case CoolantFluidType::HighPerformance:
            coolantDissipation = 3.5;
            coolantMax = 125.0;
            oilCoolingRate = 0.45;
            exhaustCoolingRate = 0.35;
            break;
        case CoolantFluidType::Racing:
            coolantDissipation = 4.0;
            coolantMax = 120.0;
            oilCoolingRate = 0.5;
            exhaustCoolingRate = 0.4;
            break;
        case CoolantFluidType::ExtremePerformance:
            coolantDissipation = 4.5;
            coolantMax = 115.0;
            oilCoolingRate = 0.55;
            exhaustCoolingRate = 0.45;
            break;
        case CoolantFluidType::LiquidNitrogen:
            coolantDissipation = 6.0;
            coolantMax = 90.0;
            oilCoolingRate = 0.75;
            exhaustCoolingRate = 0.7;
            break;
    }

    double heatInput = 15.0 * engineLoad + 0.05 * turboTemp;
    coolantTemp_ += (heatInput - (coolantTemp_ - ambientTemp) * coolantDissipation) * dt;
    coolantTemp_ = std::clamp(coolantTemp_, ambientTemp, coolantMax);

    coolantFlow_ = 1.0 + (coolantTemp_ - 90.0) * 0.02;

    oilTemp_ += ((coolantTemp_ + 15.0) - oilTemp_) * dt * oilCoolingRate;
    oilTemp_ = std::clamp(oilTemp_, ambientTemp, 150.0);

    exhaustTemp_ += ((220.0 + engineLoad * 250.0) - exhaustTemp_) * dt * exhaustCoolingRate;
    exhaustTemp_ = std::clamp(exhaustTemp_, 100.0, 820.0);
}

double CoolingSystem::coolantTemp() const
{
    return coolantTemp_;
}

double CoolingSystem::oilTemp() const
{
    return oilTemp_;
}

double CoolingSystem::coolantPressure() const
{
    return coolantFlow_;
}
double CoolingSystem::exhaustTemp() const
{
    return exhaustTemp_;
}

CoolantFluidType CoolingSystem::coolantFluidType() const
{
    return coolantType_;
}

void CoolingSystem::setCoolantFluidType(CoolantFluidType type)
{
    coolantType_ = type;
}

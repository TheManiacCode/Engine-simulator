#include "FuelSystem.h"
#include "ECU.h"
#include <algorithm>

FuelSystem::FuelSystem()
    : fuelRate_(0.0), pressure_(0.0), filterDelta_(0.0), temperature_(35.0), filterClog_(0.0)
{
}

void FuelSystem::update(double dt, const ECU& ecu, const Sensors& sensors)
{
    double baseFlow = std::clamp(ecu.fuelCommand().pulseWidthMs * 0.22, 0.0, 3.8);
    double boostFactor = 1.0 + std::clamp(sensors.readings().boostPressure, 0.0, 2.0) * 0.15;
    fuelRate_ = std::clamp(baseFlow * boostFactor, 0.05, 6.2);

    pressure_ = std::clamp(35.0 + fuelRate_ * 4.5 - filterClog_ * 2.5, 20.0, 80.0);
    filterDelta_ = std::clamp(fuelRate_ * 0.08 + filterClog_ * 0.9, 0.1, 6.0);
    temperature_ += ((35.0 + fuelRate_ * 5.0) - temperature_) * dt * 0.4;
    temperature_ = std::clamp(temperature_, 25.0, 95.0);

    filterClog_ += std::max(0.0, fuelRate_ * 0.0008 - dt * 0.0002);
    filterClog_ = std::clamp(filterClog_, 0.0, 0.55);
}

double FuelSystem::fuelRate() const
{
    return fuelRate_;
}

double FuelSystem::pressure() const
{
    return pressure_;
}

double FuelSystem::filterDelta() const
{
    return filterDelta_;
}

double FuelSystem::temperature() const
{
    return temperature_;
}

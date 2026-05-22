#include "Turbocharger.h"
#include <algorithm>

Turbocharger::Turbocharger()
    : spoolSpeed_(0.1), boostPressure_(0.0), temperature_(120.0), health_(1.0)
{
}

void Turbocharger::update(double dt, double engineRpm, double exhaustTemp)
{

    double rpmFraction = std::clamp(engineRpm / 6000.0, 0.0, 1.0);
    spoolSpeed_ += (rpmFraction - spoolSpeed_) * dt * 1.3;
    if (health_ <= 0.35) {
        boostPressure_ = 0.6 * (rpmFraction - spoolSpeed_) * dt * 1.3;
    }
    else if (health_ <= 0.5) {
        boostPressure_ = 0.9 * (rpmFraction - spoolSpeed_) * dt * 1.3;
    }
    else {
        boostPressure_ = std::clamp(0.8 + spoolSpeed_ * 1.9, 0.0, 2.9);
    }
    temperature_ += ((exhaustTemp / 20.0) + boostPressure_ * 15.0 - (temperature_ - 100.0) * 0.2) * dt;
    temperature_ = std::clamp(temperature_, 90.0, 480.0);
    health_ -= std::max(0.0, (temperature_ - 420.0) * dt * 0.0005 + (boostPressure_ - 1.8) * dt * 0.001);
    health_ = std::clamp(health_, 0.2, 1.0);
}

double Turbocharger::boostPressure() const
{
    return boostPressure_;
}

double Turbocharger::temperature() const
{
    return temperature_;
}

double Turbocharger::health() const
{
    return health_;
}

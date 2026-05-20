#include "AirIntake.h"
#include "Turbocharger.h"
#include <algorithm>

AirIntake::AirIntake()
    : airMassFlow_(0.0), intakeTemperature_(25.0), mafVoltage_(0.0)
{
}

void AirIntake::update(double dt, const Sensors& sensors, const Turbocharger& turbo)
{
    double throttle = std::clamp(sensors.readings().throttlePosition, 0.0, 1.0);
    double boostFactor = std::clamp(turbo.boostPressure(), 0.0, 2.2);
    airMassFlow_ = std::clamp(0.08 * throttle * (1.0 + boostFactor * 0.4) * 1.0, 0.0, 7.0);
    intakeTemperature_ += ((25.0 + boostFactor * 20.0) - intakeTemperature_) * dt * 0.5;
    mafVoltage_ = std::clamp(0.5 + airMassFlow_ * 0.35, 0.5, 5.0);
}

double AirIntake::airMassFlow() const
{
    return airMassFlow_;
}

double AirIntake::intakeTemperature() const
{
    return intakeTemperature_;
}

double AirIntake::mafVoltage() const
{
    return mafVoltage_;
}

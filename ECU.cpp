#include "ECU.h"
#include <algorithm>

ECU::ECU()
    : faultActive_(false)
{
}

void ECU::update(double dt, const Sensors::Readings& sensors, double currentRpm)
{
    (void)dt;
    double baseLoad = std::clamp(sensors.throttlePosition, 0.0, 1.0);

    command_.pulseWidthMs = lookupFuelPulseWidth(currentRpm, baseLoad);

    double boostCorrection = 1.0 + sensors.boostPressure * 0.05;
    command_.pulseWidthMs *= boostCorrection;

    command_.pulseWidthMs = std::clamp(command_.pulseWidthMs, 0.05, 8.2);

    command_.targetRpm = std::clamp(800.0 + sensors.throttlePosition * 5200.0, 800.0, 6000.0);
    command_.clearFaults = false;

    if (sensors.coolantTemp > 118.0 || sensors.oilTemp > 140.0) {
        faultActive_ = true;
        command_.pulseWidthMs *= 0.75;
    } else if (faultActive_ && sensors.coolantTemp < 105.0) {
        faultActive_ = false;
    }
}

const ECU::FuelCommand& ECU::fuelCommand() const
{
    return command_;
}

bool ECU::faultActive() const
{
    return faultActive_;
}

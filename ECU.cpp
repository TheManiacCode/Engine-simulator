#include "ECU.h"
#include <algorithm>

ECU::ECU()
    : faultActive_(false)
{
}

void ECU::update(double dt, const Sensors::Readings& sensors)
{
    (void)dt;
    command_.targetRpm = std::clamp(800.0 + sensors.throttlePosition * 5200.0, 800.0, 6000.0);
    command_.pulseWidthMs = std::clamp(0.8 + sensors.throttlePosition * 2.9 + sensors.boostPressure * 0.2, 0.5, 4.2);
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

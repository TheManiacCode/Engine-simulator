#include "Sensors.h"
#include <algorithm>

void Sensors::sample(const Engine::State& engineState, double throttlePosition)
{
    current_.throttlePosition = std::clamp(throttlePosition, 0.0, 1.0);
    current_.mafVoltage = std::clamp(engineState.mafVoltage, 0.2, 5.0);
    current_.mapPressure = std::clamp(101.3 + engineState.boostPressure * 100.0, 80.0, 350.0);
    current_.coolantTemp = engineState.coolantTemp;
    current_.oilTemp = engineState.oilTemp;
    current_.intakeTemp = engineState.intakeAirTemp;
    current_.exhaustTemp = engineState.exhaustTemp;
    current_.boostPressure = engineState.boostPressure;
    double loadFactor = throttlePosition;
    double rpmFactor = engineState.rpm / 6000.0;
    current_.dieselContentPPM = 10.0 + (loadFactor * rpmFactor * 450);
    current_.dieselContentPPM = std::clamp(current_.dieselContentPPM, 0.0, 2000.0);
}

const Sensors::Readings& Sensors::readings() const
{
    return current_;
}

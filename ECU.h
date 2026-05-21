#pragma once

#include "Sensors.h"
#include "FuelLookup.h"
class ECU {
public:
    struct FuelCommand {
        double pulseWidthMs = 0.8;
        double targetRpm = 800.0;
        bool clearFaults = false;
    };

    ECU();
    void update(double dt, const Sensors::Readings& sensors, double currentRpm);

    const FuelCommand& fuelCommand() const;
    bool faultActive() const;
    double unburntFuelInExhaust() const;

private:
    double unburntFuelInExhaust_;
    FuelCommand command_;
    bool faultActive_;
};

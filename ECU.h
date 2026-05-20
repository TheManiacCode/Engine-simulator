#pragma once

#include "Sensors.h"

class ECU {
public:
    struct FuelCommand {
        double pulseWidthMs = 0.8;
        double targetRpm = 800.0;
        bool clearFaults = false;
    };

    ECU();
    void update(double dt, const Sensors::Readings& sensors);

    const FuelCommand& fuelCommand() const;
    bool faultActive() const;

private:
    FuelCommand command_;
    bool faultActive_;
};

#pragma once

#include "Sensors.h"

class ECU;

class FuelSystem {
public:
    FuelSystem();
    void update(double dt, const ECU& ecu, const Sensors& sensors);

    double fuelRate() const;
    double pressure() const;
    double filterDelta() const;
    double temperature() const;

private:
    double fuelRate_;
    double pressure_;
    double filterDelta_;
    double temperature_;
    double filterClog_;
};

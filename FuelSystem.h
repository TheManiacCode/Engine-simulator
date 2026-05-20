#pragma once

#include "Sensors.h"

class ECU;

class FuelSystem {
public:
    static constexpr double tankCapacity = 60.0;

    FuelSystem();
    void update(double dt, const ECU& ecu, const Sensors& sensors);

    double fuelRate() const;
    double pressure() const;
    double filterDelta() const;
    double temperature() const;
    double fuelRemaining() const;
    bool hasFuel() const;

private:
    double fuelRate_;
    double pressure_;
    double filterDelta_;
    double temperature_;
    double filterClog_;
    double fuelRemaining_; 
};

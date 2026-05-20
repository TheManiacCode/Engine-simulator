#pragma once

#include "Sensors.h"

class Turbocharger;

class AirIntake {
public:
    AirIntake();
    void update(double dt, const Sensors& sensors, const Turbocharger& turbo);

    double airMassFlow() const;
    double intakeTemperature() const;
    double mafVoltage() const;

private:
    double airMassFlow_;
    double intakeTemperature_;
    double mafVoltage_;
};

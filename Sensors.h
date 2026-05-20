#pragma once

#include "Engine.h"

class Sensors {
public:
    struct Readings {
        double throttlePosition = 0.0;
        double mafVoltage = 0.0;
        double mapPressure = 101.3;
        double coolantTemp = 90.0;
        double oilTemp = 95.0;
        double intakeTemp = 25.0;
        double exhaustTemp = 260.0;
        double boostPressure = 0.0;
    };

    void sample(const Engine::State& engineState, double throttlePosition);
    const Readings& readings() const;

private:
    Readings current_;
};

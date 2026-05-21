#pragma once

class FuelSystem;
class AirIntake;
class Turbocharger;
class CoolingSystem;
class Sensors;
class ECU;

class Engine {
public:
    struct State {
        double rpm = 800.0;
        double torque = 0.0;
        double load = 0.0;
        double boostPressure = 0.0;
        double fuelRate = 0.0;
        double airMassFlow = 0.0;
        double coolantTemp = 90.0;
        double oilTemp = 95.0;
        double intakeAirTemp = 25.0;
        double exhaustTemp = 250.0;
        double fuelTemp = 30.0;
        double mafVoltage = 0.0;
        double mapPressure = 101.3;
        double unburntFuelInExhaust = 0.0;
    };

    Engine();
    void update(double dt, double throttlePosition, const FuelSystem& fuel, const AirIntake& intake,
                const Turbocharger& turbo, const CoolingSystem& cooling, const Sensors& sensors,
                const ECU& ecu);

    const State& state() const;

private:
    State state_;
    double inertia_;
};

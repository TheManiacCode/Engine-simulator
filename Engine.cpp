#include "Engine.h"
#include "FuelSystem.h"
#include "AirIntake.h"
#include "Turbocharger.h"
#include "CoolingSystem.h"
#include "Sensors.h"
#include "ECU.h"
#include <algorithm>
#include <cmath>

Engine::Engine()
    : inertia_(0.12)
{
}

void Engine::update(double dt, double throttlePosition, const FuelSystem& fuel, const AirIntake& intake,
                    const Turbocharger& turbo, const CoolingSystem& cooling, const Sensors& sensors,
                    const ECU& ecu)
{
    (void)sensors;
    (void)ecu;
    double targetRpm = std::clamp(800.0 + throttlePosition * 5200.0, 800.0, 6000.0);
    double rpmError = targetRpm - state_.rpm;
    double rpmAdjustment = rpmError * 0.35;

    double torqueBase = 70.0 + 130.0 * std::tanh(fuel.fuelRate() * 0.25);
    double boostFactor = 1.0 + std::clamp(turbo.boostPressure(), 0.0, 2.0) * 0.18;
    double availableTorque = std::clamp(torqueBase * boostFactor, 0.0, 420.0);

    double friction = 30.0 + state_.rpm * 0.015;
    double netTorque = std::max(0.0, availableTorque - friction);

    double rpmDelta = (netTorque - 20.0) * dt * 0.45;
    state_.rpm += rpmDelta + rpmAdjustment * dt;
    state_.rpm = std::clamp(state_.rpm, 700.0, 6000.0);

    state_.torque = availableTorque;
    state_.load = std::clamp(netTorque / 420.0, 0.0, 1.0);
    state_.boostPressure = turbo.boostPressure();
    state_.fuelRate = fuel.fuelRate();
    state_.airMassFlow = intake.airMassFlow();
    state_.coolantTemp = cooling.coolantTemp();
    state_.oilTemp = cooling.oilTemp();
    state_.intakeAirTemp = intake.intakeTemperature();
    state_.exhaustTemp = cooling.exhaustTemp();
    state_.fuelTemp = fuel.temperature();
    state_.mafVoltage = intake.mafVoltage();
    state_.mapPressure = std::clamp(101.3 + state_.boostPressure * 95.0, 60.0, 340.0);
}

const Engine::State& Engine::state() const
{
    return state_;
}

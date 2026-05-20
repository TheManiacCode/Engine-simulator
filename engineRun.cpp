#include "Engine.h"
#include "Turbocharger.h"
#include "FuelSystem.h"
#include "AirIntake.h"
#include "CoolingSystem.h"
#include "Sensors.h"
#include "ECU.h"
#include "CANBus.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <algorithm>

int main(int argc, char* argv[])
{
    std::string canInterface;
    if (argc > 1) {
        canInterface = argv[1];
    }

    CANBus can;
    if (!canInterface.empty()) {
        if (!can.initialize(canInterface)) {
            std::cerr << "Warning: CAN initialization failed, running simulation without CAN.\n";
        }
    }

    Engine engine;
    Turbocharger turbo;
    FuelSystem fuel;
    AirIntake intake;
    CoolingSystem cooling;
    Sensors sensors;
    ECU ecu;

    double throttlePosition = 0.0;
    double timeSeconds = 0.0;
    const double dt = 0.05;
    std::cout << "Diesel ECU simulator starting. Use throttle 0-100 or CAN command on "
              << (canInterface.empty() ? "none" : canInterface) << ".\n";

    auto lastPrompt = std::chrono::steady_clock::now();
    while (true) {
        bool gotCanCommand = false;
        if (can.isInitialized()) {
            can.processInput(throttlePosition, gotCanCommand);
        }

        if (!gotCanCommand) {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - lastPrompt).count() >= 2) {
                std::cout << "Enter throttle percent [0-100], 'q' to quit: ";
                std::cout.flush();
                std::string line;
                if (std::getline(std::cin, line)) {
                    if (!line.empty() && (line[0] == 'q' || line[0] == 'Q')) {
                        break;
                    }
                    try {
                        double value = std::stod(line);
                        throttlePosition = std::clamp(value / 100.0, 0.0, 1.0);
                    } catch (...) {
                        std::cerr << "Invalid throttle input.\n";
                    }
                }
                lastPrompt = now;
            }
        }

        sensors.sample(engine.state(), throttlePosition);
        ecu.update(dt, sensors.readings());
        intake.update(dt, sensors, turbo);
        fuel.update(dt, ecu, sensors);
        turbo.update(dt, engine.state().rpm, cooling.exhaustTemp());
        cooling.update(dt, engine.state().load, 20.0, turbo.temperature());
        engine.update(dt, throttlePosition, fuel, intake, turbo, cooling, sensors, ecu);

        if (can.isInitialized()) {
            can.sendEngineState(engine, sensors);
        }

        if (static_cast<int>(timeSeconds / dt) % 10 == 0) {
            const auto& state = engine.state();
            std::cout << std::fixed << std::setprecision(1)
                      << "t=" << timeSeconds << "s "
                      << "RPM=" << state.rpm << " "
                      << "Throttle=" << throttlePosition * 100.0 << "% "
                      << "Boost=" << state.boostPressure << "bar "
                      << "Fuel=" << state.fuelRate << "L/h "
                      << "Coolant=" << state.coolantTemp << "C "
                      << "Oil=" << state.oilTemp << "C "
                      << "Intake=" << state.intakeAirTemp << "C "
                      << "Exhaust=" << state.exhaustTemp << "C\n";
        }

        std::this_thread::sleep_for(std::chrono::duration<double>(dt));
        timeSeconds += dt;
    }

    std::cout << "Simulation ended.\n";
    return 0;
}

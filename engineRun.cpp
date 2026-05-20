#include "Engine.h"
#include "Turbocharger.h"
#include "FuelSystem.h"
#include "AirIntake.h"
#include "CoolingSystem.h"
#include "Sensors.h"
#include "ECU.h"
#include "CANBus.h"
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <algorithm>
#include <sys/select.h>
#include <unistd.h>

static bool stdinReady()
{
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);
    struct timeval tv{0, 0};
    return select(STDIN_FILENO + 1, &rfds, nullptr, nullptr, &tv) > 0;
}

static int calculateFiringCylinder(double rpm, double elapsedSeconds)
{
    if (rpm < 50.0) {
        return 0;
    }

    double revolutions = rpm / 60.0 * elapsedSeconds;
    double cycleDegrees = std::fmod(revolutions * 360.0, 720.0);
    int phase = static_cast<int>(std::floor(cycleDegrees / 180.0)) % 4;
    const int firingOrder[4] = {1, 3, 4, 2};
    return firingOrder[(phase + 4) % 4];
}

static std::string inline4FiringArt(int activeCylinder)
{
    std::ostringstream art;
    art << "Firing: ";
    for (int cylinder = 1; cylinder <= 4; ++cylinder) {
        if (cylinder == activeCylinder) {
            art << "[>" << cylinder << "<] ";
        } else {
            art << "[ " << cylinder << " ] ";
        }
    }
    if (activeCylinder == 0) {
        art.str("Firing: idle");
    }
    return art.str();
}

int main(int argc, char* argv[])
{
    std::string canInterface;
    double simTotal = 0.0;
    double cmdThrottlePercent = -1.0;
    double cmdThrottleDuration = 0.0;

    if (argc == 4) {
        simTotal = std::stod(argv[1]);
        cmdThrottlePercent = std::stod(argv[2]);
        cmdThrottleDuration = std::stod(argv[3]);
    } else if (argc == 5) {
        canInterface = argv[1];
        simTotal = std::stod(argv[2]);
        cmdThrottlePercent = std::stod(argv[3]);
        cmdThrottleDuration = std::stod(argv[4]);
    } else if (argc > 1) {
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
    double scheduledThrottle = -1.0;
    if (cmdThrottlePercent >= 0.0) {
        scheduledThrottle = std::clamp(cmdThrottlePercent / 100.0, 0.0, 1.0);
    }
    bool interactive = (cmdThrottlePercent < 0.0);
    double timeSeconds = 0.0;
    const double dt = 0.05;
    std::cout << "Diesel ECU simulator starting. Use throttle 0-100 or CAN command on "
              << (canInterface.empty() ? "none" : canInterface) << ".\n";
    std::cout << "Fuel tank capacity: 60.0 L.\n";
    if (interactive) {
        std::cout << "Enter one of the following:\n";
        std::cout << "  3 values: totalTime throttlePercent duration  (scheduled run)\n";
        std::cout << "  1 value : throttlePercent  (start interactive with that throttle)\n";
        std::cout << "  blank   : continuous interactive mode\n> ";
        std::cout.flush();
        std::string startupLine;
        if (std::getline(std::cin, startupLine) && !startupLine.empty()) {
            std::istringstream iss(startupLine);
            double a, b, c;
            if (iss >> a >> b >> c) {
                simTotal = a;
                cmdThrottlePercent = b;
                cmdThrottleDuration = c;
                scheduledThrottle = std::clamp(cmdThrottlePercent / 100.0, 0.0, 1.0);
                interactive = false;
                std::cout << "Scheduled run: " << simTotal << "s, throttle=" << cmdThrottlePercent
                          << "% for " << cmdThrottleDuration << "s\n";
            } else {
                iss.clear();
                iss.str(startupLine);
                if (iss >> a && !(iss >> b)) {
                    throttlePosition = std::clamp(a / 100.0, 0.0, 1.0);
                    std::cout << "Interactive mode enabled with initial throttle=" << a << "%\n";
                } else {
                    std::cout << "Continuous interactive throttle mode enabled.\n";
                }
            }
        } else {
            std::cout << "Continuous interactive throttle mode enabled.\n";
        }
    } else {
        std::cout << "Scheduled run: " << simTotal << "s, throttle=" << cmdThrottlePercent << "% for "
                  << cmdThrottleDuration << "s\n";
    }

    auto lastPrompt = std::chrono::steady_clock::now();
    while (interactive ? true : (timeSeconds < simTotal)) {
        bool gotCanCommand = false;
        if (can.isInitialized()) {
            can.processInput(throttlePosition, gotCanCommand);
        }

        if (!gotCanCommand) {
            if (interactive) {
                std::string line;
                if (stdinReady() && std::getline(std::cin, line)) {
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
            } else {
                if (scheduledThrottle >= 0.0 && timeSeconds <= cmdThrottleDuration) {
                    throttlePosition = scheduledThrottle;
                } else if (scheduledThrottle >= 0.0 && cmdThrottleDuration > 0.0) {
                    throttlePosition = 0.0;
                }
            }
        }

        sensors.sample(engine.state(), throttlePosition);
        ecu.update(dt, sensors.readings(), engine.state().rpm);
        intake.update(dt, sensors, turbo);
        fuel.update(dt, ecu, sensors);
        turbo.update(dt, engine.state().rpm, cooling.exhaustTemp());
        cooling.update(dt, engine.state().load, 20.0, turbo.temperature());
        engine.update(dt, throttlePosition, fuel, intake, turbo, cooling, sensors, ecu);

        if (can.isInitialized()) {
            can.sendEngineState(engine, sensors);
        }

        if (static_cast<int>(timeSeconds / dt) % 20 == 0) {
            const auto& state = engine.state();
            int currentCylinder = calculateFiringCylinder(state.rpm, timeSeconds);
            std::cout << std::fixed << std::setprecision(1)
                      << "t=" << timeSeconds << "s "
                      << "RPM=" << state.rpm << " "
                      << "Throttle=" << throttlePosition * 100.0 << "% "
                      << "Boost=" << state.boostPressure << "bar "
                      << "Fuel=" << state.fuelRate << "L/h "
                      << "Remaining=" << fuel.fuelRemaining() << "L "
                      << "Coolant=" << state.coolantTemp << "C "
                      << "Oil=" << state.oilTemp << "C "
                      << "Intake=" << state.intakeAirTemp << "C "
                      << "Exhaust=" << state.exhaustTemp << "C\n";
            std::cout << inline4FiringArt(currentCylinder) << "\n";
        }

        std::this_thread::sleep_for(std::chrono::duration<double>(dt));
        timeSeconds += dt;
    }

    std::cout << "Simulation ended.\n";
    return 0;
}

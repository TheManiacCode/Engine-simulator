#pragma once

#include <cstdint>
#include <string>

class Engine;
class Sensors;
class ECU;

class CANBus {
public:
    CANBus();
    ~CANBus();

    bool initialize(const std::string& interfaceName);
    void sendEngineState(const Engine& engine, const Sensors& sensors) const;
    void processInput(double& throttlePosition, bool& gotCommand);
    bool isInitialized() const;

private:
    int socketFd_;
    bool canEnabled_;
    std::string interfaceName_;
    bool readCommand(double& throttlePosition);
    static uint16_t packUint16(double value, double scale, double offset);
};

#include "CANBus.h"
#include "Engine.h"
#include "Sensors.h"
#include "ECU.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <iostream>

CANBus::CANBus()
    : socketFd_(-1), canEnabled_(false)
{
}

CANBus::~CANBus()
{
    if (socketFd_ >= 0) {
        close(socketFd_);
    }
}

bool CANBus::initialize(const std::string& interfaceName)
{
    interfaceName_ = interfaceName;
    socketFd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socketFd_ < 0) {
        std::cerr << "CAN socket init failed: " << strerror(errno) << "\n";
        canEnabled_ = false;
        return false;
    }

    struct ifreq ifr;
    std::strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    if (ioctl(socketFd_, SIOCGIFINDEX, &ifr) < 0) {
        std::cerr << "CAN ifindex failed: " << strerror(errno) << "\n";
        close(socketFd_);
        canEnabled_ = false;
        return false;
    }

    struct sockaddr_can addr{};
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(socketFd_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "CAN bind failed: " << strerror(errno) << "\n";
        close(socketFd_);
        canEnabled_ = false;
        return false;
    }

    int flags = fcntl(socketFd_, F_GETFL, 0);
    fcntl(socketFd_, F_SETFL, flags | O_NONBLOCK);
    canEnabled_ = true;
    return true;
}

bool CANBus::isInitialized() const
{
    return canEnabled_;
}

uint16_t CANBus::packUint16(double value, double scale, double offset)
{
    int integer = static_cast<int>(std::round((value + offset) * scale));
    return static_cast<uint16_t>(std::clamp(integer, 0, 0xFFFF));
}

void CANBus::sendEngineState(const Engine& engine, const Sensors& sensors) const
{
    if (!canEnabled_) {
        return;
    }

    const auto& state = engine.state();
    const auto& reading = sensors.readings();
    struct can_frame frame{};

    frame.can_id = 0x100;
    frame.can_dlc = 8;
    frame.data[0] = static_cast<uint8_t>(state.rpm / 25.0);
    frame.data[1] = static_cast<uint8_t>(reading.throttlePosition * 250.0);
    frame.data[2] = static_cast<uint8_t>(std::clamp(state.boostPressure * 25.0, 0.0, 255.0));
    frame.data[3] = static_cast<uint8_t>(std::clamp(state.load * 250.0, 0.0, 255.0));
    frame.data[4] = 0;
    frame.data[5] = 0;
    frame.data[6] = 0;
    frame.data[7] = 0;
    write(socketFd_, &frame, sizeof(frame));

    frame.can_id = 0x101;
    frame.data[0] = static_cast<uint8_t>(std::clamp((reading.coolantTemp + 40.0) * 1.2, 0.0, 255.0));
    frame.data[1] = static_cast<uint8_t>(std::clamp((reading.oilTemp + 40.0) * 1.2, 0.0, 255.0));
    frame.data[2] = static_cast<uint8_t>(std::clamp((reading.intakeTemp + 40.0) * 1.2, 0.0, 255.0));
    frame.data[3] = static_cast<uint8_t>(std::clamp((reading.exhaustTemp - 100.0) * 0.4, 0.0, 255.0));
    frame.data[4] = 0;
    frame.data[5] = 0;
    frame.data[6] = 0;
    frame.data[7] = 0;
    write(socketFd_, &frame, sizeof(frame));

    frame.can_id = 0x102;
    frame.data[0] = static_cast<uint8_t>(std::clamp(state.fuelRate * 20.0, 0.0, 255.0));
    frame.data[1] = static_cast<uint8_t>(std::clamp(engine.state().fuelRate * 8.0, 0.0, 255.0));
    frame.data[2] = static_cast<uint8_t>(std::clamp(sensors.readings().boostPressure * 50.0, 0.0, 255.0));
    frame.data[3] = 0;
    frame.data[4] = 0;
    frame.data[5] = 0;
    frame.data[6] = 0;
    frame.data[7] = 0;
    write(socketFd_, &frame, sizeof(frame));
}

void CANBus::processInput(double& throttlePosition, bool& gotCommand)
{
    gotCommand = false;
    if (!canEnabled_) {
        return;
    }

    double receivedThrottle = 0.0;
    if (readCommand(receivedThrottle)) {
        throttlePosition = std::clamp(receivedThrottle, 0.0, 1.0);
        gotCommand = true;
    }
}

bool CANBus::readCommand(double& throttlePosition)
{
    struct can_frame frame{};
    int nbytes = read(socketFd_, &frame, sizeof(frame));
    if (nbytes < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return false;
        }
        return false;
    }

    if (frame.can_id != 0x200 || frame.can_dlc < 1) {
        return false;
    }

    throttlePosition = static_cast<double>(frame.data[0]) / 250.0;
    return true;
}

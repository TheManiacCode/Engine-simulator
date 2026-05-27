#pragma once

enum class CoolantFluidType {
    Standard,
    HighPerformance,
    Racing,
    ExtremePerformance,
    LiquidNitrogen,
};

class CoolingSystem {
public:
    CoolingSystem(CoolantFluidType fluidType = CoolantFluidType::HighPerformance);
    void update(double dt, double engineLoad, double ambientTemp, double turboTemp);

    double coolantTemp() const;
    double oilTemp() const;
    double exhaustTemp() const;
    CoolantFluidType coolantFluidType() const;
    void setCoolantFluidType(CoolantFluidType type);
    double coolantPressure() const;

private:
    double coolantFlow_;
    double coolantTemp_;
    double oilTemp_;
    double exhaustTemp_;
    CoolantFluidType coolantType_;
};
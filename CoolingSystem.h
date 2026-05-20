#pragma once

class CoolingSystem {
public:
    CoolingSystem();
    void update(double dt, double engineLoad, double ambientTemp, double turboTemp);

    double coolantTemp() const;
    double oilTemp() const;
    double exhaustTemp() const;

private:
    double coolantTemp_;
    double oilTemp_;
    double exhaustTemp_;
};

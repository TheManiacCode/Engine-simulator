#pragma once

class Turbocharger {
public:
    Turbocharger();
    void update(double dt, double engineRpm, double exhaustTemp);

    double boostPressure() const;
    double temperature() const;
    double health() const;

private:
    double spoolSpeed_;
    double boostPressure_;
    double temperature_;
    double health_;
};

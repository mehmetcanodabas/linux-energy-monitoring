#pragma once

#include <string>

#include "Measurement.h"

class AlarmManager {
public:
    // Configurable alarm thresholds
    struct Thresholds {
        double minVoltage = 220.0;
        double maxVoltage = 235.0;
        double maxCurrent = 7.0;
        double maxPower = 1600.0;
    };

    explicit AlarmManager(Thresholds thresholds)
        : thresholds_(thresholds) {}

    // Check whether a measurement exceeds any threshold
    std::string check(const Measurement& measurement) const {
        std::string alarm;

        if (measurement.voltage < thresholds_.minVoltage) {
            alarm += "LOW VOLTAGE; ";
        }

        if (measurement.voltage > thresholds_.maxVoltage) {
            alarm += "HIGH VOLTAGE; ";
        }

        if (measurement.current > thresholds_.maxCurrent) {
            alarm += "HIGH CURRENT; ";
        }

        if (measurement.power > thresholds_.maxPower) {
            alarm += "HIGH POWER; ";
        }

        return alarm.empty() ? "NORMAL" : alarm;
    }

private:
    Thresholds thresholds_;
};

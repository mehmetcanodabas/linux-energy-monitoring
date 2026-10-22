#pragma once

struct Measurement {
    double voltage;
    double current;
    double power;
};

inline Measurement createMeasurement(double voltage, double current) {
    return {
        voltage,
        current,
        voltage * current
    };
}

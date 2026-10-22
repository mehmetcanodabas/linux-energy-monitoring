
#pragma once

#include <vector>
#include "Measurement.h"

class SensorSimulator {
public:
    // Generate predefined electrical measurements
    std::vector<Measurement> generateMeasurements() const {
        return {
            createMeasurement(230.0, 5.2),
            createMeasurement(228.5, 5.5),
            createMeasurement(231.2, 5.1),
            createMeasurement(225.0, 6.0),
            createMeasurement(235.0, 7.5),
            createMeasurement(240.0, 8.2)
        };
    }
};

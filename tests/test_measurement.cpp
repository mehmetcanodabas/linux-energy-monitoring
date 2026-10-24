#include <cassert>
#include <cmath>
#include <iostream>

#include "Measurement.h"

int main() {
    // Test power calculation
    Measurement measurement = createMeasurement(230.0, 5.2);

    assert(measurement.voltage == 230.0);
    assert(measurement.current == 5.2);
    assert(std::abs(measurement.power - 1196.0) < 0.0001);

    // Test zero current
    Measurement zeroCurrent = createMeasurement(230.0, 0.0);

    assert(std::abs(zeroCurrent.power) < 0.0001);

    std::cout << "Measurement tests passed!\n";

    return 0;
}

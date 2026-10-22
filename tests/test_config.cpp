#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "ConfigReader.h"

int main() {
    const std::string filename = "test_thresholds.conf";

    // Create a temporary configuration file
    {
        std::ofstream file(filename);
        assert(file.is_open());

        file << "min_voltage=210.0\n";
        file << "max_voltage=240.0\n";
        file << "max_current=8.0\n";
        file << "max_power=1800.0\n";
    }

    // Verify configuration values
    const auto thresholds = ConfigReader::load(filename);

    assert(std::abs(thresholds.minVoltage - 210.0) < 0.0001);
    assert(std::abs(thresholds.maxVoltage - 240.0) < 0.0001);
    assert(std::abs(thresholds.maxCurrent - 8.0) < 0.0001);
    assert(std::abs(thresholds.maxPower - 1800.0) < 0.0001);

    // Remove the temporary file
    std::filesystem::remove(filename);

    std::cout << "Configuration tests passed!\n";

    return 0;
}

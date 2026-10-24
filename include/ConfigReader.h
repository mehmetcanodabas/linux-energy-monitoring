#pragma once

#include <fstream>
#include <stdexcept>
#include <string>

#include "AlarmManager.h"

class ConfigReader {
public:
    static AlarmManager::Thresholds load(const std::string& filename) {
        // Open the configuration file
        std::ifstream file(filename);

        if (!file.is_open()) {
            throw std::runtime_error(
                "Cannot open configuration file: " + filename
            );
        }

        AlarmManager::Thresholds thresholds;
        bool minVoltageFound = false;
        bool maxVoltageFound = false;
        bool maxCurrentFound = false;
        bool maxPowerFound = false;

        std::string line;

        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }

            const std::size_t separator = line.find('=');

            if (separator == std::string::npos) {
                throw std::runtime_error("Invalid configuration line: " + line);
            }

            const std::string key = line.substr(0, separator);
            const std::string value = line.substr(separator + 1);

            double number;
            try {
                std::size_t processed = 0;
                number = std::stod(value, &processed);

                if (processed != value.size()) {
                    throw std::invalid_argument("Unexpected characters");
                }
            } catch (const std::exception&) {
                throw std::runtime_error("Invalid value for: " + key);
            }

            if (key == "min_voltage") {
                thresholds.minVoltage = number;
                minVoltageFound = true;
            } else if (key == "max_voltage") {
                thresholds.maxVoltage = number;
                maxVoltageFound = true;
            } else if (key == "max_current") {
                thresholds.maxCurrent = number;
                maxCurrentFound = true;
            } else if (key == "max_power") {
                thresholds.maxPower = number;
                maxPowerFound = true;
            } else {
                throw std::runtime_error("Unknown configuration key: " + key);
            }
        }

        if (!minVoltageFound || !maxVoltageFound ||
            !maxCurrentFound || !maxPowerFound) {
            throw std::runtime_error("Missing alarm threshold in configuration");
        }

        if (thresholds.minVoltage > thresholds.maxVoltage ||
            thresholds.maxCurrent < 0 || thresholds.maxPower < 0) {
            throw std::runtime_error("Invalid alarm threshold range");
        }

        return thresholds;
    }
};

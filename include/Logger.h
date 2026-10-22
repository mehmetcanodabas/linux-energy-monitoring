
#pragma once

#include <fstream>
#include <stdexcept>
#include <string>

#include "Measurement.h"

class Logger {
public:
    explicit Logger(const std::string& filename)
        : file_(filename, std::ios::app) {
        if (!file_.is_open()) {
            throw std::runtime_error(
                "Cannot open log file: " + filename
            );
        }
    }

    // Save measurement values and alarm status
    void log(const Measurement& measurement, const std::string& status) {
        file_ << "Voltage=" << measurement.voltage << " V, "
              << "Current=" << measurement.current << " A, "
              << "Power=" << measurement.power << " W, "
              << "Status=" << status << '\n';

        if (!file_) {
            throw std::runtime_error("Failed to write log entry");
        }
    }

private:
    std::ofstream file_;
};

#include <exception>
#include <filesystem>
#include <iostream>
#include <vector>

#include "Measurement.h"
#include "SensorSimulator.h"
#include "AlarmManager.h"
#include "ConfigReader.h"
#include "Logger.h"

int main() {
    try {
        // Load alarm thresholds
        AlarmManager::Thresholds thresholds =
            ConfigReader::load("config/thresholds.conf");

        AlarmManager alarmManager(thresholds);

        // Create the log directory if necessary
        std::filesystem::create_directories("logs");

        Logger logger("logs/measurements.log");

        // Generate simulated sensor measurements
        SensorSimulator simulator;
        std::vector<Measurement> measurements =
            simulator.generateMeasurements();

        std::cout << "Linux Energy Monitoring System\n";
        std::cout << "------------------------------\n";

        // Process measurements, check alarms and save logs
        for (std::size_t i = 0; i < measurements.size(); ++i) {
            const Measurement& measurement = measurements[i];
            const std::string status = alarmManager.check(measurement);

            std::cout << "\nMeasurement " << i + 1 << "\n";
            std::cout << "Voltage: " << measurement.voltage << " V\n";
            std::cout << "Current: " << measurement.current << " A\n";
            std::cout << "Power:   " << measurement.power << " W\n";
            std::cout << "Status:  " << status << "\n";

            logger.log(measurement, status);
        }

        std::cout << "\nMeasurements saved to logs/measurements.log\n";

    } catch (const std::exception& error) {
        std::cerr << "ERROR: " << error.what() << "\n";
        return 1;
    }

    return 0;
}

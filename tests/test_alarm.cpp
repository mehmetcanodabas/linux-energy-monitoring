#include <cassert>
#include <iostream>
#include <string>

#include "AlarmManager.h"
#include "Measurement.h"

int main() {
    // Configure alarm thresholds
    AlarmManager::Thresholds thresholds;
    thresholds.minVoltage = 220.0;
    thresholds.maxVoltage = 235.0;
    thresholds.maxCurrent = 8.0;
    thresholds.maxPower = 1600.0;

    AlarmManager alarmManager(thresholds);

    // Test normal measurements
    Measurement normal = createMeasurement(230.0, 5.2);
    assert(alarmManager.check(normal) == "NORMAL");

    // Test low voltage alarm
    Measurement lowVoltage = createMeasurement(210.0, 2.0);
    assert(alarmManager.check(lowVoltage) == "LOW VOLTAGE; ");

    // Test high voltage alarm
    Measurement highVoltage = createMeasurement(240.0, 2.0);
    assert(alarmManager.check(highVoltage) == "HIGH VOLTAGE; ");

    // Test high current and high power alarms
    Measurement highCurrent = createMeasurement(230.0, 9.0);
    assert(alarmManager.check(highCurrent) ==
           "HIGH CURRENT; HIGH POWER; ");

    // Test voltage threshold boundary
    Measurement boundary = createMeasurement(235.0, 2.0);
    assert(alarmManager.check(boundary) == "NORMAL");

    std::cout << "Alarm tests passed!\n";

    return 0;
}

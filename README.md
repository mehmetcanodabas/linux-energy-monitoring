## linux-energy-monitoring

Implementation of a Linux-Based Energy Monitoring and Alert System Using C++17 and CMake

The project processes simulated electrical measurements, including voltage, current, and calculated DC power, using a modular C++ application.

Configurable alarm thresholds are loaded from an external configuration file to detect abnormal operating conditions. Measurement values and alarm statuses are displayed in the terminal and recorded in a log file.

The application was developed and tested in an Ubuntu Linux environment.

### Key Results

- Successfully processed 6 simulated voltage and current measurements with automatic power calculation.
- Implemented configurable voltage, current, and power alarm thresholds.
- Detected high-power, high-current, and high-voltage conditions.
- Verified that alarm thresholds can be modified without recompiling the application.
- Implemented file-based logging for measurements and alarm statuses.
- Successfully built the application using C++17 and CMake on Ubuntu Linux.
- Passed 3 automated tests using CTest, covering power calculation, alarm detection, and configuration loading.

### Result Snapshots

[Energy Monitoring](docs/results/energy%20monitoring.png): Simulated electrical measurements showing calculated power values and detected alarm conditions.

[Unit Tests](docs/results/unit%20tests.png): Successful execution of three automated C++ tests using CTest.

**Keywords:** C++17, Linux, CMake, Energy Monitoring, Electrical Measurements, Threshold-Based Alarms, Configuration Management, File Logging, Unit Testing, CTest.

**Development Environment:** Ubuntu Linux (WSL2), GCC, CMake.

**Data Source:** Simulated electrical measurements.

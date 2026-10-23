## linux-energy-monitoring

Implementation of a Linux-Based Energy Monitoring and Alert System Using C++17, TCP/IP, and CMake

The project processes simulated electrical measurements, including voltage, current, and calculated DC power, using a modular C++ application.

Configurable alarm thresholds are loaded from an external configuration file to detect abnormal operating conditions. Measurement values and alarm statuses are displayed in the terminal and recorded in a log file.

A lightweight HTTP server, implemented using the Linux socket API, provides TCP/IP communication through two endpoints: GET `/measurements` for retrieving electrical measurements and alarm statuses, and POST `/thresholds` for remotely updating alarm thresholds.

The application was developed and tested in an Ubuntu Linux (WSL2) environment.

### Key Results

- Successfully processed 6 simulated voltage and current measurements with automatic power calculation.
- Implemented configurable voltage, current, and power alarm thresholds.
- Detected high-power, high-current, and high-voltage conditions.
- Implemented a C++ HTTP server using Linux TCP sockets.
- Successfully retrieved measurement data and alarm statuses using HTTP GET.
- Remotely updated alarm thresholds using HTTP POST and verified changes in alarm detection.
- Implemented file-based logging for measurements and alarm statuses.
- Successfully built the application using C++17 and CMake on Ubuntu Linux.
- Passed 3 automated tests using CTest, covering power calculation, alarm detection, and configuration loading.

### Result Snapshots

[Energy Monitoring](docs/results/energy%20monitoring.png): Electrical measurements retrieved through HTTP GET, showing calculated power values and alarm statuses.

[Unit Tests](docs/results/unit%20tests.png): Successful execution of three automated C++ tests using CTest.

[HTTP Communication](docs/results/http%20communication.png): Remote alarm threshold update using HTTP POST, followed by HTTP GET to verify the updated alarm conditions.

**Keywords:** C++17, Linux, TCP/IP, Socket Programming, HTTP GET/POST, CMake, Energy Monitoring, Electrical Measurements, Threshold-Based Alarms, Configuration Management, File Logging, Unit Testing, CTest.

**Development Environment:** Ubuntu Linux (WSL2), GCC, CMake.

**Data Source:** Simulated electrical measurements.
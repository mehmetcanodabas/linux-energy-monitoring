
cmake_minimum_required(VERSION 3.16)

project(LinuxEnergyMonitoring VERSION 1.0 LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build the energy monitoring application
add_executable(energy_monitor
    src/main.cpp
)

target_include_directories(energy_monitor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(energy_monitor PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# Enable automated testing
enable_testing()

# Build the measurement test
add_executable(test_measurement
    tests/test_measurement.cpp
)

target_include_directories(test_measurement PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(test_measurement PRIVATE
    -UNDEBUG
    -Wall
    -Wextra
    -Wpedantic
)

add_test(
    NAME MeasurementTest
    COMMAND test_measurement
)

# Build the alarm test
add_executable(test_alarm
    tests/test_alarm.cpp
)

target_include_directories(test_alarm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(test_alarm PRIVATE
    -UNDEBUG
    -Wall
    -Wextra
    -Wpedantic
)

add_test(
    NAME AlarmTest
    COMMAND test_alarm
)

# Build the configuration test
add_executable(test_config
    tests/test_config.cpp
)

target_include_directories(test_config PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(test_config PRIVATE
    -UNDEBUG
    -Wall
    -Wextra
    -Wpedantic
)

add_test(
    NAME ConfigurationTest
    COMMAND test_config
)

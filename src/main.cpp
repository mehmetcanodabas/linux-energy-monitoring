#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cmath>
#include <csignal>
#include <cstring>
#include <exception>
#include <filesystem>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "AlarmManager.h"
#include "ConfigReader.h"
#include "Logger.h"
#include "Measurement.h"
#include "SensorSimulator.h"

namespace {

constexpr int SERVER_PORT = 8080;
constexpr std::size_t MAX_REQUEST_SIZE = 8192;

std::string httpResponse(
    int statusCode,
    const std::string& statusText,
    const std::string& body
) {
    std::ostringstream response;

    response << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n"
             << "Content-Type: application/json\r\n"
             << "Content-Length: " << body.size() << "\r\n"
             << "Connection: close\r\n"
             << "\r\n"
             << body;

    return response.str();
}

std::string escapeJson(const std::string& text) {
    std::string result;

    for (char character : text) {
        if (character == '"') {
            result += "\\\"";
        } else if (character == '\\') {
            result += "\\\\";
        } else if (character == '\n') {
            result += "\\n";
        } else {
            result += character;
        }
    }

    return result;
}

std::string createMeasurementsJson(
    const std::vector<Measurement>& measurements,
    const AlarmManager& alarmManager
) {
    std::ostringstream json;
    json << "{\"measurements\":[";

    for (std::size_t i = 0; i < measurements.size(); ++i) {
        if (i > 0) {
            json << ",";
        }

        const Measurement& m = measurements[i];

        json << "{"
             << "\"voltage\":" << m.voltage << ","
             << "\"current\":" << m.current << ","
             << "\"power\":" << m.power << ","
             << "\"status\":\""
             << escapeJson(alarmManager.check(m)) << "\""
             << "}";
    }

    json << "]}";
    return json.str();
}

bool updateThresholds(
    const std::string& json,
    AlarmManager::Thresholds& thresholds
) {
    // Accept a flat JSON object containing numeric threshold values.
    static const std::regex fieldPattern(
        R"REGEX(\s*"([a-z_]+)"\s*:\s*(-?(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+)(?:[eE][+-]?[0-9]+)?)\s*)REGEX"
    );

    AlarmManager::Thresholds updated = thresholds;
    std::size_t position = json.find_first_not_of(" \t\r\n");

    if (position == std::string::npos || json[position] != '{') {
        return false;
    }

    ++position;
    bool foundField = false;

    while (true) {
        position = json.find_first_not_of(" \t\r\n", position);

        if (position == std::string::npos) {
            return false;
        }

        if (json[position] == '}') {
            ++position;
            break;
        }

        std::smatch match;
        const std::string remaining = json.substr(position);

        if (!std::regex_search(
                remaining,
                match,
                fieldPattern,
                std::regex_constants::match_continuous)) {
            return false;
        }

        const std::string key = match[1].str();

        double value;
        try {
            value = std::stod(match[2].str());
        } catch (const std::exception&) {
            return false;
        }

        if (!std::isfinite(value)) {
            return false;
        }

        if (key == "min_voltage") {
            updated.minVoltage = value;
        } else if (key == "max_voltage") {
            updated.maxVoltage = value;
        } else if (key == "max_current") {
            updated.maxCurrent = value;
        } else if (key == "max_power") {
            updated.maxPower = value;
        } else {
            return false;
        }

        foundField = true;
        position += match.length();
        position = json.find_first_not_of(" \t\r\n", position);

        if (position == std::string::npos) {
            return false;
        }

        if (json[position] == ',') {
            ++position;
            continue;
        }

        if (json[position] == '}') {
            ++position;
            break;
        }

        return false;
    }

    if (!foundField ||
        json.find_first_not_of(" \t\r\n", position) != std::string::npos ||
        updated.minVoltage > updated.maxVoltage ||
        updated.maxCurrent < 0 ||
        updated.maxPower < 0) {
        return false;
    }

    thresholds = updated;
    return true;
}

std::string handleRequest(
    const std::string& request,
    const std::vector<Measurement>& measurements,
    AlarmManager::Thresholds& thresholds
) {
    const std::size_t headerEnd = request.find("\r\n\r\n");

    if (headerEnd == std::string::npos) {
        return httpResponse(400, "Bad Request",
                            "{\"error\":\"Invalid HTTP request\"}");
    }

    const std::string headers = request.substr(0, headerEnd);
    const std::string body = request.substr(headerEnd + 4);

    std::istringstream stream(headers);
    std::string method;
    std::string path;
    std::string version;

    stream >> method >> path >> version;

    if (version != "HTTP/1.1") {
        return httpResponse(400, "Bad Request",
                            "{\"error\":\"Invalid HTTP version\"}");
    }

    if (method == "GET" && path == "/measurements") {
        AlarmManager alarmManager(thresholds);

        return httpResponse(
            200,
            "OK",
            createMeasurementsJson(measurements, alarmManager)
        );
    }

    if (method == "POST" && path == "/thresholds") {
        if (!updateThresholds(body, thresholds)) {
            return httpResponse(
                400,
                "Bad Request",
                "{\"error\":\"Invalid threshold configuration\"}"
            );
        }

        return httpResponse(
            200,
            "OK",
            "{\"message\":\"Thresholds updated successfully\"}"
        );
    }

    return httpResponse(
        404,
        "Not Found",
        "{\"error\":\"Endpoint not found\"}"
    );
}

void sendResponse(int clientSocket, const std::string& response) {
    std::size_t sent = 0;

    while (sent < response.size()) {
        const ssize_t bytes = send(
            clientSocket,
            response.data() + sent,
            response.size() - sent,
            0
        );

        if (bytes <= 0) {
            return;
        }

        sent += static_cast<std::size_t>(bytes);
    }
}

void runServer(
    const std::vector<Measurement>& measurements,
    AlarmManager::Thresholds& thresholds
) {
    const int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0) {
        throw std::runtime_error("Cannot create TCP socket");
    }

    int reuseAddress = 1;
    setsockopt(
        serverSocket,
        SOL_SOCKET,
        SO_REUSEADDR,
        &reuseAddress,
        sizeof(reuseAddress)
    );

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(SERVER_PORT);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(
            serverSocket,
            reinterpret_cast<sockaddr*>(&address),
            sizeof(address)) < 0) {
        close(serverSocket);
        throw std::runtime_error("Cannot bind TCP port 8080");
    }

    if (listen(serverSocket, 5) < 0) {
        close(serverSocket);
        throw std::runtime_error("Cannot listen on TCP socket");
    }

    std::cout << "\nHTTP server running on http://127.0.0.1:8080\n";
    std::cout << "GET  /measurements\n";
    std::cout << "POST /thresholds\n";
    std::cout << "Press Ctrl+C to stop.\n";

    while (true) {
        const int clientSocket = accept(
            serverSocket, nullptr, nullptr
        );

        if (clientSocket < 0) {
            continue;
        }

        // Avoid waiting indefinitely for incomplete client requests.
        timeval timeout{};
        timeout.tv_sec = 5;

        setsockopt(
            clientSocket,
            SOL_SOCKET,
            SO_RCVTIMEO,
            &timeout,
            sizeof(timeout)
        );

        std::string request;
        char buffer[2048];
        std::size_t expectedSize = 0;
        bool headersComplete = false;

        while (request.size() < MAX_REQUEST_SIZE) {
            const ssize_t received = recv(
                clientSocket, buffer, sizeof(buffer), 0
            );

            if (received <= 0) {
                break;
            }

            request.append(
                buffer,
                static_cast<std::size_t>(received)
            );

            const std::size_t headerEnd = request.find("\r\n\r\n");

            if (!headersComplete &&
                headerEnd != std::string::npos) {
                headersComplete = true;
                expectedSize = headerEnd + 4;

                std::istringstream headerStream(
                    request.substr(0, headerEnd)
                );

                std::string line;

                while (std::getline(headerStream, line)) {
                    if (line.rfind("Content-Length:", 0) == 0 ||
                        line.rfind("content-length:", 0) == 0) {
                        try {
                            expectedSize += std::stoul(
                                line.substr(15)
                            );
                        } catch (const std::exception&) {
                            expectedSize = MAX_REQUEST_SIZE + 1;
                        }
                    }
                }
            }

            if (headersComplete && request.size() >= expectedSize) {
                break;
            }
        }

        std::string response;

        if (request.size() >= MAX_REQUEST_SIZE ||
            (headersComplete && expectedSize > MAX_REQUEST_SIZE)) {
            response = httpResponse(
                413,
                "Payload Too Large",
                "{\"error\":\"Request too large\"}"
            );
        } else if (!headersComplete ||
                   request.size() < expectedSize) {
            response = httpResponse(
                400,
                "Bad Request",
                "{\"error\":\"Incomplete HTTP request\"}"
            );
        } else {
            response = handleRequest(
                request, measurements, thresholds
            );
        }

        sendResponse(clientSocket, response);
        close(clientSocket);
    }
}

} // namespace

int main() {
    try {
        // Load alarm settings from configuration file.
        AlarmManager::Thresholds thresholds =
            ConfigReader::load("config/thresholds.conf");

        std::filesystem::create_directories("logs");
        Logger logger("logs/measurements.log");

        SensorSimulator simulator;
        const std::vector<Measurement> measurements =
            simulator.generateMeasurements();

        AlarmManager alarmManager(thresholds);

        std::cout << "Linux Energy Monitoring System\n";
        std::cout << "------------------------------\n";

        for (std::size_t i = 0; i < measurements.size(); ++i) {
            const Measurement& m = measurements[i];
            const std::string status = alarmManager.check(m);

            std::cout << "\nMeasurement " << i + 1 << "\n";
            std::cout << "Voltage: " << m.voltage << " V\n";
            std::cout << "Current: " << m.current << " A\n";
            std::cout << "Power:   " << m.power << " W\n";
            std::cout << "Status:  " << status << "\n";

            logger.log(m, status);
        }

        // Prevent termination when a TCP client disconnects.
        std::signal(SIGPIPE, SIG_IGN);

        runServer(measurements, thresholds);

    } catch (const std::exception& error) {
        std::cerr << "ERROR: " << error.what() << "\n";
        return 1;
    }

    return 0;
}

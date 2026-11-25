#ifndef DISASTER_CORE_EXCEPTIONS_HPP
#define DISASTER_CORE_EXCEPTIONS_HPP

#include <exception>
#include <string>

namespace DisasterCore {
namespace Alerts {

// Unit IV: Custom exception classes derived from std::exception
class SensorFailureException : public std::exception {
private:
    std::string nodeId_;
    std::string message_;

public:
    SensorFailureException(const std::string& nodeId, const std::string& reason)
        : nodeId_(nodeId),
          message_("[SensorFailureException] Node '" + nodeId + "' failed: " + reason) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }

    const std::string& getNodeId() const noexcept { return nodeId_; }
};

class TelemetryChecksumMismatch : public std::exception {
private:
    unsigned int expected_;
    unsigned int actual_;
    std::string message_;

public:
    TelemetryChecksumMismatch(unsigned int expected, unsigned int actual)
        : expected_(expected),
          actual_(actual),
          message_("[TelemetryChecksumMismatch] Expected checksum " +
                   std::to_string(expected) + ", got " + std::to_string(actual)) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }

    unsigned int getExpected() const noexcept { return expected_; }
    unsigned int getActual() const noexcept { return actual_; }
};

class ThresholdOverflowException : public std::exception {
private:
    double observedValue_;
    double maxAllowed_;
    std::string message_;

public:
    ThresholdOverflowException(double observed, double maxAllowed)
        : observedValue_(observed),
          maxAllowed_(maxAllowed),
          message_("[ThresholdOverflowException] Observed value " +
                   std::to_string(observed) + " exceeds physical limit " +
                   std::to_string(maxAllowed)) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }

    double getObservedValue() const noexcept { return observedValue_; }
    double getMaxAllowed() const noexcept { return maxAllowed_; }
};

} // namespace Alerts
} // namespace DisasterCore

#endif // DISASTER_CORE_EXCEPTIONS_HPP

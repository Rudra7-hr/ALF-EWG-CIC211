#ifndef DISASTER_CORE_TELEMETRY_BUFFER_HPP
#define DISASTER_CORE_TELEMETRY_BUFFER_HPP

#include <cstddef>
#include <iostream>

namespace DisasterCore {
namespace Sensors {

// Unit II: Dynamic sensor memory buffer with parameterized, dynamic, deep copy constructors & explicit destructor
class TelemetryBuffer {
private:
    double* samples_;
    std::size_t capacity_;
    std::size_t count_;

public:
    // Parameterized constructor
    explicit TelemetryBuffer(std::size_t capacity = 8);

    // Dynamic constructor (allocates and initializes buffer from raw array)
    TelemetryBuffer(const double* initialSamples, std::size_t count);

    // Deep copy constructor
    TelemetryBuffer(const TelemetryBuffer& other);

    // Copy assignment operator (Rule of Three)
    TelemetryBuffer& operator=(const TelemetryBuffer& other);

    // Explicit destructor
    ~TelemetryBuffer();

    void pushSample(double value);
    double getSample(std::size_t index) const;
    double average() const;
    std::size_t size() const noexcept { return count_; }
    std::size_t capacity() const noexcept { return capacity_; }
};

} // namespace Sensors
} // namespace DisasterCore

#endif // DISASTER_CORE_TELEMETRY_BUFFER_HPP

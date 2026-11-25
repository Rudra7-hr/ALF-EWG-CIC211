#include "TelemetryBuffer.hpp"
#include <stdexcept>

namespace DisasterCore {
namespace Sensors {

// Parameterized constructor
TelemetryBuffer::TelemetryBuffer(std::size_t capacity)
    : samples_(nullptr),
      capacity_(capacity > 0 ? capacity : 1),
      count_(0) {
    samples_ = new double[capacity_]{};
}

// Dynamic constructor
TelemetryBuffer::TelemetryBuffer(const double* initialSamples, std::size_t count)
    : samples_(nullptr),
      capacity_(count > 0 ? count : 1),
      count_(count) {
    samples_ = new double[capacity_]{};
    for (std::size_t i = 0; i < count_; ++i) {
        samples_[i] = initialSamples[i];
    }
}

// Deep copy constructor
TelemetryBuffer::TelemetryBuffer(const TelemetryBuffer& other)
    : samples_(new double[other.capacity_]{}),
      capacity_(other.capacity_),
      count_(other.count_) {
    for (std::size_t i = 0; i < count_; ++i) {
        samples_[i] = other.samples_[i];
    }
}

// Copy assignment operator
TelemetryBuffer& TelemetryBuffer::operator=(const TelemetryBuffer& other) {
    if (this == &other) {
        return *this;
    }
    double* newBuf = new double[other.capacity_]{};
    for (std::size_t i = 0; i < other.count_; ++i) {
        newBuf[i] = other.samples_[i];
    }
    delete[] samples_;
    samples_ = newBuf;
    capacity_ = other.capacity_;
    count_ = other.count_;
    return *this;
}

// Explicit destructor
TelemetryBuffer::~TelemetryBuffer() {
    delete[] samples_;
    samples_ = nullptr;
}

void TelemetryBuffer::pushSample(double value) {
    if (count_ >= capacity_) {
        std::size_t newCap = capacity_ * 2;
        double* resized = new double[newCap]{};
        for (std::size_t i = 0; i < count_; ++i) {
            resized[i] = samples_[i];
        }
        delete[] samples_;
        samples_ = resized;
        capacity_ = newCap;
    }
    samples_[count_++] = value;
}

double TelemetryBuffer::getSample(std::size_t index) const {
    if (index >= count_) {
        throw std::out_of_range("TelemetryBuffer index out of range");
    }
    return samples_[index];
}

double TelemetryBuffer::average() const {
    if (count_ == 0) {
        return 0.0;
    }
    double sum = 0.0;
    for (std::size_t i = 0; i < count_; ++i) {
        sum += samples_[i];
    }
    return sum / static_cast<double>(count_);
}

} // namespace Sensors
} // namespace DisasterCore

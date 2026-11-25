#include "Entities.hpp"
#include <iomanip>

namespace DisasterCore {
namespace Alerts {

EarlyWarningSiren::EarlyWarningSiren(const std::string& id,
                                     const std::string& sector,
                                     double db)
    : sirenId_(id),
      sectorName_(sector),
      active_(false),
      decibelOutput_(db) {}

void EarlyWarningSiren::trigger(AlertLevel level) {
    if (level >= AlertLevel::WATCH) {
        active_ = true;
        std::cout << "  [SIREN " << sirenId_ << " | " << sectorName_
                  << "] ACTIVATED at " << decibelOutput_ << " dB ("
                  << alertLevelToString(level) << ")\n";
    } else {
        std::cout << "  [SIREN " << sirenId_ << " | " << sectorName_
                  << "] Standby (" << alertLevelToString(level) << ")\n";
    }
}

void EarlyWarningSiren::silence() {
    active_ = false;
}

} // namespace Alerts

namespace Sensors {

// Static member initialization
int SensorStation::activeStationCount_ = 0;
Alerts::AlertLevel SensorStation::globalEmergencyLevel_ = Alerts::AlertLevel::NORMAL;

SensorStation::SensorStation(const std::string& id,
                             const std::string& region,
                             double hazard,
                             std::size_t bufferCap)
    : stationId_(id),
      regionName_(region),
      hazardIndex_(hazard),
      rawCalibrationOffset_(0.037),
      buffer_(bufferCap) {
    ++activeStationCount_;
    if (hazard >= 80.0) {
        globalEmergencyLevel_ = Alerts::AlertLevel::CRITICAL_EVACUATION;
    } else if (hazard >= 60.0 && globalEmergencyLevel_ < Alerts::AlertLevel::WATCH) {
        globalEmergencyLevel_ = Alerts::AlertLevel::WATCH;
    } else if (hazard >= 35.0 && globalEmergencyLevel_ < Alerts::AlertLevel::ADVISORY) {
        globalEmergencyLevel_ = Alerts::AlertLevel::ADVISORY;
    }
}

SensorStation::SensorStation(const SensorStation& other)
    : stationId_(other.stationId_),
      regionName_(other.regionName_),
      hazardIndex_(other.hazardIndex_),
      rawCalibrationOffset_(other.rawCalibrationOffset_),
      buffer_(other.buffer_) {
    ++activeStationCount_;
}

SensorStation::~SensorStation() {
    --activeStationCount_;
}

void SensorStation::recordReading(double sample, double updatedHazard) {
    buffer_.pushSample(sample);
    hazardIndex_ = updatedHazard;
    if (hazardIndex_ >= 80.0) {
        globalEmergencyLevel_ = Alerts::AlertLevel::CRITICAL_EVACUATION;
    } else if (hazardIndex_ >= 60.0 && globalEmergencyLevel_ < Alerts::AlertLevel::WATCH) {
        globalEmergencyLevel_ = Alerts::AlertLevel::WATCH;
    } else if (hazardIndex_ >= 35.0 && globalEmergencyLevel_ < Alerts::AlertLevel::ADVISORY) {
        globalEmergencyLevel_ = Alerts::AlertLevel::ADVISORY;
    }
}

// Binary + aggregates two stations into a regional cluster vector
std::vector<SensorStation> SensorStation::operator+(const SensorStation& other) const {
    std::vector<SensorStation> cluster;
    cluster.reserve(2);
    cluster.push_back(*this);
    cluster.push_back(other);
    return cluster;
}

// Relational > prioritizes higher hazard values
bool SensorStation::operator>(const SensorStation& other) const noexcept {
    return hazardIndex_ > other.hazardIndex_;
}

// Stream insertion <<
std::ostream& operator<<(std::ostream& os, const SensorStation& st) {
    os << std::left << std::setw(10) << st.stationId_
       << " | " << std::setw(20) << st.regionName_
       << " | Hazard: " << std::fixed << std::setprecision(2) << std::setw(6) << st.hazardIndex_
       << " | AvgSample: " << std::setw(6) << st.buffer_.average();
    return os;
}

// Stream extraction >>
std::istream& operator>>(std::istream& is, SensorStation& st) {
    is >> st.stationId_ >> st.regionName_ >> st.hazardIndex_;
    return is;
}

// Friend class inspecting private raw fields directly
void TelemetryAuditor::auditStation(const SensorStation& st, std::ostream& os) {
    os << "[AUDIT] Station=" << st.stationId_
       << ", PrivateCalibrationOffset=" << st.rawCalibrationOffset_
       << ", BufferCount=" << st.buffer_.size() << "/" << st.buffer_.capacity()
       << ", RawHazard=" << st.hazardIndex_ << "\n";
}

} // namespace Sensors
} // namespace DisasterCore

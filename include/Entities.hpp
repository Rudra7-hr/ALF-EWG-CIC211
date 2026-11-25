#ifndef DISASTER_CORE_ENTITIES_HPP
#define DISASTER_CORE_ENTITIES_HPP

#include "TelemetryBuffer.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace DisasterCore {
namespace Alerts {

enum class AlertLevel {
    NORMAL = 0,
    ADVISORY = 1,
    WATCH = 2,
    CRITICAL_EVACUATION = 3
};

inline std::string alertLevelToString(AlertLevel lvl) {
    switch (lvl) {
        case AlertLevel::NORMAL: return "NORMAL";
        case AlertLevel::ADVISORY: return "ADVISORY";
        case AlertLevel::WATCH: return "WATCH";
        case AlertLevel::CRITICAL_EVACUATION: return "CRITICAL_EVACUATION";
    }
    return "UNKNOWN";
}

class EarlyWarningSiren {
private:
    std::string sirenId_;
    std::string sectorName_;
    bool active_;
    double decibelOutput_;

public:
    EarlyWarningSiren(const std::string& id = "SRN-00",
                      const std::string& sector = "Default-Sector",
                      double db = 115.0);

    void trigger(AlertLevel level);
    void silence();

    bool isActive() const noexcept { return active_; }
    const std::string& getSirenId() const noexcept { return sirenId_; }
    const std::string& getSectorName() const noexcept { return sectorName_; }
    double getDecibelOutput() const noexcept { return decibelOutput_; }
};

} // namespace Alerts

namespace Sensors {

// Forward declaration of friend class
class TelemetryAuditor;

// Unit II: SensorStation with strict data abstraction, const correctness, static members, and operator overloading
class SensorStation {
private:
    std::string stationId_;
    std::string regionName_;
    double hazardIndex_;
    double rawCalibrationOffset_;
    TelemetryBuffer buffer_;

    // Static data members tracking network-wide active stations & global emergency alert level
    static int activeStationCount_;
    static Alerts::AlertLevel globalEmergencyLevel_;

public:
    SensorStation(const std::string& id = "STN-000",
                  const std::string& region = "Himalayan-Zone-A",
                  double hazard = 0.0,
                  std::size_t bufferCap = 8);
    SensorStation(const SensorStation& other);
    ~SensorStation();

    void recordReading(double sample, double updatedHazard);

    // Const-correct getters
    const std::string& getStationId() const noexcept { return stationId_; }
    const std::string& getRegionName() const noexcept { return regionName_; }
    double getHazardIndex() const noexcept { return hazardIndex_; }
    const TelemetryBuffer& getBuffer() const noexcept { return buffer_; }

    // Static member functions
    static int getActiveStationCount() noexcept { return activeStationCount_; }
    static Alerts::AlertLevel getGlobalEmergencyLevel() noexcept { return globalEmergencyLevel_; }
    static void resetGlobalEmergencyLevel() noexcept { globalEmergencyLevel_ = Alerts::AlertLevel::NORMAL; }

    // Operator Overloading (Unit II)
    // 1. Binary + to aggregate multiple sensor clusters into regional zone vectors
    std::vector<SensorStation> operator+(const SensorStation& other) const;

    // 2. Relational operator > to prioritize higher hazard values
    bool operator>(const SensorStation& other) const noexcept;

    // 3. Stream insertion (<<) and extraction (>>) for console & file streams
    friend std::ostream& operator<<(std::ostream& os, const SensorStation& st);
    friend std::istream& operator>>(std::istream& is, SensorStation& st);

    // Unit I: Friend class to inspect raw private sensor metrics without exposing getters
    friend class TelemetryAuditor;
};

// Friend class inspecting raw private metrics of SensorStation
class TelemetryAuditor {
public:
    static void auditStation(const SensorStation& st, std::ostream& os);
};

} // namespace Sensors
} // namespace DisasterCore

#endif // DISASTER_CORE_ENTITIES_HPP

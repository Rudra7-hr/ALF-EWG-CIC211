#ifndef DISASTER_CORE_PERSISTENCE_AND_STL_HPP
#define DISASTER_CORE_PERSISTENCE_AND_STL_HPP

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace DisasterCore {
namespace Alerts {

// Fixed-size POD-like struct for safe binary serialization via std::fstream
struct IncidentRecord {
    int incidentId;
    char stationId[16];
    char zoneName[24];
    double hazardScore;
    int severityLevel; // 0=NORMAL, 1=ADVISORY, 2=WATCH, 3=CRITICAL
    bool committed;
};

// Unit IV: Custom stream manipulator for tabular alert dispatch formatting
inline std::ostream& alertHeaderFormat(std::ostream& os) {
    os << std::left << std::setw(6) << "ID"
       << std::setw(14) << "STATION"
       << std::setw(22) << "ZONE"
       << std::right << std::setw(10) << "HAZARD"
       << "  " << std::left << std::setw(14) << "SEVERITY"
       << "STATUS";
    return os;
}

inline std::ostream& hazardScoreFormat(std::ostream& os) {
    os << std::fixed << std::setprecision(2) << std::right << std::setw(10);
    return os;
}

// Unit IV: Custom functor for priority-based evacuation routing (descending hazard, then severity)
struct EvacuationPriorityComparator {
    bool operator()(const IncidentRecord& a, const IncidentRecord& b) const noexcept {
        if (a.hazardScore != b.hazardScore) {
            return a.hazardScore > b.hazardScore;
        }
        return a.severityLevel > b.severityLevel;
    }
};

// Unit IV: Persistent binary logger with transactional rollback using seekp() / seekg()
class IncidentBinaryLogger {
private:
    std::string filePath_;

public:
    explicit IncidentBinaryLogger(const std::string& filePath);

    void clearLog();
    std::streampos appendIncident(const IncidentRecord& record);

    // Transactional write: writes record, validates checksum/threshold, and rolls back via seekp()/seekg() on failure
    bool appendIncidentTransactional(const IncidentRecord& record, bool simulateFailure = false);

    std::vector<IncidentRecord> loadAllIncidents() const;
};

} // namespace Alerts
} // namespace DisasterCore

#endif // DISASTER_CORE_PERSISTENCE_AND_STL_HPP

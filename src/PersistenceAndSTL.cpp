#include "PersistenceAndSTL.hpp"
#include "Exceptions.hpp"

namespace DisasterCore {
namespace Alerts {

IncidentBinaryLogger::IncidentBinaryLogger(const std::string& filePath)
    : filePath_(filePath) {}

void IncidentBinaryLogger::clearLog() {
    std::ofstream ofs(filePath_, std::ios::binary | std::ios::trunc);
    ofs.close();
}

std::streampos IncidentBinaryLogger::appendIncident(const IncidentRecord& record) {
    std::ofstream ofs(filePath_, std::ios::binary | std::ios::app);
    if (!ofs) {
        throw SensorFailureException("DISK-LOGGER", "Unable to open binary log for writing");
    }
    std::streampos pos = ofs.tellp();
    ofs.write(reinterpret_cast<const char*>(&record), sizeof(IncidentRecord));
    ofs.flush();
    return pos;
}

bool IncidentBinaryLogger::appendIncidentTransactional(const IncidentRecord& record, bool simulateFailure) {
    // Ensure file exists
    {
        std::ofstream ensure(filePath_, std::ios::binary | std::ios::app);
    }

    std::fstream fs(filePath_, std::ios::binary | std::ios::in | std::ios::out);
    if (!fs) {
        throw SensorFailureException("DISK-LOGGER", "Unable to open binary log in read/write mode");
    }

    // Move to end and record checkpoint position using seekp/tellp
    fs.seekp(0, std::ios::end);
    std::streampos checkpointPos = fs.tellp();

    // Write tentative uncommitted record
    IncidentRecord tentative = record;
    tentative.committed = false;
    fs.write(reinterpret_cast<const char*>(&tentative), sizeof(IncidentRecord));
    fs.flush();

    // Verify written bytes via seekg()
    fs.seekg(checkpointPos);
    IncidentRecord verify{};
    fs.read(reinterpret_cast<char*>(&verify), sizeof(IncidentRecord));

    if (simulateFailure || verify.hazardScore < 0.0 || verify.hazardScore > 100.0) {
        // Transactional rollback: rewind with seekp() and overwrite with neutral/rolled-back marker
        fs.clear();
        fs.seekp(checkpointPos);
        IncidentRecord rolledBack{};
        rolledBack.incidentId = -1; // Marked invalid/rolled back
        std::strncpy(rolledBack.stationId, "ROLLED_BACK", sizeof(rolledBack.stationId) - 1);
        std::strncpy(rolledBack.zoneName, "TX_ABORTED", sizeof(rolledBack.zoneName) - 1);
        rolledBack.hazardScore = 0.0;
        rolledBack.severityLevel = 0;
        rolledBack.committed = false;
        fs.write(reinterpret_cast<const char*>(&rolledBack), sizeof(IncidentRecord));
        fs.flush();
        return false;
    }

    // Commit record by seeking back with seekp() and setting committed = true
    fs.clear();
    fs.seekp(checkpointPos);
    tentative.committed = true;
    fs.write(reinterpret_cast<const char*>(&tentative), sizeof(IncidentRecord));
    fs.flush();
    return true;
}

std::vector<IncidentRecord> IncidentBinaryLogger::loadAllIncidents() const {
    std::vector<IncidentRecord> records;
    std::ifstream ifs(filePath_, std::ios::binary);
    if (!ifs) {
        return records;
    }

    ifs.seekg(0, std::ios::end);
    std::streamoff totalBytes = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::size_t count = static_cast<std::size_t>(totalBytes) / sizeof(IncidentRecord);
    for (std::size_t i = 0; i < count; ++i) {
        IncidentRecord rec{};
        if (ifs.read(reinterpret_cast<char*>(&rec), sizeof(IncidentRecord))) {
            if (rec.committed && rec.incidentId > 0) {
                records.push_back(rec);
            }
        }
    }
    return records;
}

} // namespace Alerts
} // namespace DisasterCore

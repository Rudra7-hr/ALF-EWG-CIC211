#include "Entities.hpp"
#include "Exceptions.hpp"
#include "MathUtils.hpp"
#include "PersistenceAndSTL.hpp"
#include "SensorHierarchy.hpp"
#include "TelemetryBuffer.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

using namespace DisasterCore::Sensors;
using namespace DisasterCore::Alerts;

namespace {

IncidentRecord makeIncident(int id, const std::string& stId, const std::string& zone,
                            double hazard, int severity) {
    IncidentRecord rec{};
    rec.incidentId = id;
    std::strncpy(rec.stationId, stId.c_str(), sizeof(rec.stationId) - 1);
    std::strncpy(rec.zoneName, zone.c_str(), sizeof(rec.zoneName) - 1);
    rec.hazardScore = hazard;
    rec.severityLevel = severity;
    rec.committed = true;
    return rec;
}

} // namespace

int main() {
    std::cout << "======================================================================\n";
    std::cout << "  ALF-EWG: Autonomous Landslide & Flash-Flood Early Warning Grid\n";
    std::cout << "  CIC-211 Object-Oriented Programming Using C++ (Option 1)\n";
    std::cout << "======================================================================\n\n";

    // ------------------------------------------------------------------
    // UNIT I: Namespaces, Inline Risk Math, Overloading, Call/Return by Ref, Friend
    // ------------------------------------------------------------------
    std::cout << "[UNIT I] Core Foundations & Call Semantics\n";
    double rainHazard = computeHazardIndex(92.0, 38.0);               // Overload 1
    double seismicHazard = computeHazardIndex(78.0, 14.5, 0.35);      // Overload 2
    double floodHazard = computeHazardIndex(6.8, 4.2, 5.5, true);     // Overload 3

    std::cout << "  Overloaded computeHazardIndex (Rainfall+Slope)   : " << rainHazard << "\n";
    std::cout << "  Overloaded computeHazardIndex (Soil+Vibration)   : " << seismicHazard << "\n";
    std::cout << "  Overloaded computeHazardIndex (River+Velocity)   : " << floodHazard << "\n";

    GeoCoordinate coord{30.7333, 79.0667, 3583.0}; // Kedarnath Sector
    transformGeoCoordinates(coord, 0.0012, -0.0008, 12.5);
    std::cout << "  Transformed GeoCoordinate (by reference)         : ("
              << coord.latitude << ", " << coord.longitude << ", "
              << coord.elevationMeters << "m)\n";

    TelemetryAccumulator acc{0.0, 0.0, 0};
    accumulateTelemetryState(accumulateTelemetryState(acc, 18.5, 9.2), 24.0, 15.6);
    std::cout << "  Chained TelemetryAccumulator (return by ref)     : "
              << acc.cumulativeRainfallMm << " mm, Peak " << acc.peakVibrationHz
              << " Hz (" << acc.sampleCount << " samples)\n\n";

    // ------------------------------------------------------------------
    // UNIT II: Encapsulation, Dynamic Memory, Static Members, Operator Overloading
    // ------------------------------------------------------------------
    std::cout << "[UNIT II] Encapsulation, Dynamic Memory & Operator Overloading\n";
    double initialSamples[] = {42.1, 55.4, 61.8};
    TelemetryBuffer dynBuf(initialSamples, 3);
    TelemetryBuffer copiedBuf(dynBuf); // Deep copy constructor
    copiedBuf.pushSample(88.9);
    std::cout << "  Original Buffer Avg: " << dynBuf.average()
              << " | Deep-Copied Buffer Avg: " << copiedBuf.average() << "\n";

    SensorStation st1("STN-KDR-01", "Kedarnath-Ridge", rainHazard);
    SensorStation st2("STN-RDR-02", "Rudraprayag-Gorge", floodHazard);
    st1.recordReading(rainHazard, rainHazard);
    st2.recordReading(floodHazard, floodHazard);

    std::cout << "  Active Stations (static): " << SensorStation::getActiveStationCount()
              << " | Global Alert Level (static): "
              << alertLevelToString(SensorStation::getGlobalEmergencyLevel()) << "\n";

    // Friend class TelemetryAuditor inspecting raw private metrics
    std::cout << "  ";
    TelemetryAuditor::auditStation(st1, std::cout);

    // Binary + operator to aggregate sensor clusters
    std::vector<SensorStation> regionalCluster = st1 + st2;
    std::cout << "  Clustered Stations via operator+ (size=" << regionalCluster.size() << "):\n";
    for (const auto& s : regionalCluster) {
        std::cout << "    " << s << "\n"; // Overloaded <<
    }
    std::cout << "  Priority Comparison (st2 > st1): " << (st2 > st1 ? "YES (Flood > Rain)" : "NO") << "\n";

    // Stream extraction >> demo
    std::istringstream iss("STN-TMP-99 Tehri-Dam 71.5");
    SensorStation parsedStation;
    iss >> parsedStation;
    std::cout << "  Parsed via operator>>: " << parsedStation << "\n\n";

    // ------------------------------------------------------------------
    // UNIT III: Abstract Base Class, Diamond Resolution, Containership, Polymorphism
    // ------------------------------------------------------------------
    std::cout << "[UNIT III] Inheritance, Containership & Runtime Polymorphism\n";
    std::vector<BaseSensorNode*> sweepGrid;
    sweepGrid.push_back(new RiverSonarNode("NODE-SNR-01", "Alaknanda-Basin", 6.8, 4.2, 5.5));
    sweepGrid.push_back(new SeismicGeophoneNode("NODE-GEO-02", "Joshimath-Slope", 78.0, 14.5, 0.35));
    auto* hybrid = new AutonomousHybridProbe("NODE-HYB-03", "Badrinath-Pass",
                                             "DTD-HYB-900", "v3.1.0",
                                             92.0, 38.0, 0xA1B2, 0xA1B2);
    sweepGrid.push_back(hybrid);

    std::cout << "  Diamond Resolution -> Serial: " << hybrid->getHardwareSerial()
              << ", FW: " << hybrid->getFirmwareVersion()
              << ", SolarEff: " << hybrid->getPanelEfficiency() << "%\n";

    EarlyWarningSiren regionalSiren("SRN-HIM-01", "Upper-Garhwal", 122.0);

    for (BaseSensorNode* node : sweepGrid) {
        double score = node->readTelemetry(); // Dynamic dispatch via vtable/_vptr
        bool triggered = node->evaluateThreshold();
        std::cout << "  [" << node->getNodeType() << "] " << node->getNodeId()
                  << " (" << node->getLocation() << ") -> Hazard=" << score
                  << " | Battery=" << node->getPowerSubsystem().getBatteryVoltage() << "V"
                  << " | Threshold=" << (triggered ? "BREACHED" : "SAFE") << "\n";
        if (triggered) {
            regionalSiren.trigger(AlertLevel::CRITICAL_EVACUATION);
        }
    }
    std::cout << "\n";

    // ------------------------------------------------------------------
    // UNIT IV: Custom Exceptions, Binary fstream Rollback, STL & Manipulators
    // ------------------------------------------------------------------
    std::cout << "[UNIT IV] Exceptions, Binary Persistence (seekp/seekg) & STL Dispatch\n";

    // 1. Exception Handling Demo
    AutonomousHybridProbe faultyProbe("NODE-ERR-99", "Chamoli-Fault",
                                      "DTD-ERR-001", "v3.1.0",
                                      50.0, 30.0, 0x1234, 0x9999);
    try {
        faultyProbe.readTelemetry();
    } catch (const TelemetryChecksumMismatch& ex) {
        std::cout << "  Caught Exception: " << ex.what() << "\n";
    }

    RiverSonarNode overflowSonar("NODE-OVR-88", "Teesta-Gorge", 99.0, 3.0, 5.0);
    try {
        overflowSonar.readTelemetry();
    } catch (const ThresholdOverflowException& ex) {
        std::cout << "  Caught Exception: " << ex.what() << "\n";
    }

    // 2. Binary std::fstream with transactional rollback
    IncidentBinaryLogger logger("incidents.bin");
    logger.clearLog();
    logger.appendIncidentTransactional(makeIncident(101, "NODE-SNR-01", "Alaknanda-Basin", floodHazard, 3));
    logger.appendIncidentTransactional(makeIncident(102, "NODE-GEO-02", "Joshimath-Slope", seismicHazard, 2));
    logger.appendIncidentTransactional(makeIncident(103, "NODE-HYB-03", "Badrinath-Pass", rainHazard, 3));

    bool txOk = logger.appendIncidentTransactional(
        makeIncident(104, "NODE-BAD-04", "Corrupted-Zone", 85.0, 3), true);
    std::cout << "  Simulated Failed Transaction Rollback (seekp/seekg): "
              << (txOk ? "COMMITTED" : "ROLLED BACK SAFELY") << "\n\n";

    // 3. STL containers (vector, map), sort with custom functor, for_each, custom manipulators
    std::vector<IncidentRecord> incidents = logger.loadAllIncidents();
    std::sort(incidents.begin(), incidents.end(), EvacuationPriorityComparator());

    std::map<std::string, int> zoneIncidentCount;
    for (const auto& rec : incidents) {
        zoneIncidentCount[rec.zoneName]++;
    }

    std::cout << "  Priority Evacuation Dispatch Table (Sorted via Functor):\n";
    std::cout << "  " << alertHeaderFormat << "\n";
    std::cout << "  ----------------------------------------------------------------------\n";
    std::for_each(incidents.begin(), incidents.end(), [](const IncidentRecord& rec) {
        std::cout << "  " << std::left << std::setw(6) << rec.incidentId
                  << std::setw(14) << rec.stationId
                  << std::setw(22) << rec.zoneName
                  << hazardScoreFormat << rec.hazardScore
                  << "  " << std::left << std::setw(14)
                  << (rec.severityLevel >= 3 ? "CRITICAL" : "WATCH")
                  << (rec.committed ? "COMMITTED" : "PENDING") << "\n";
    });

    std::cout << "\n  Zone Incident Summary (std::map):\n";
    for (const auto& kv : zoneIncidentCount) {
        std::cout << "    - " << kv.first << ": " << kv.second << " incident(s)\n";
    }

    // Clean up polymorphic heap allocations
    for (BaseSensorNode* node : sweepGrid) {
        delete node;
    }
    sweepGrid.clear();

    std::cout << "\n[SYSTEM] Clean shutdown complete. Active stations remaining: "
              << SensorStation::getActiveStationCount() << "\n";
    return 0;
}

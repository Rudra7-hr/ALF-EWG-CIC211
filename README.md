# Autonomous Landslide & Flash-Flood Early Warning Grid (ALF-EWG)
**Name:** RUDRA  
**Enrollment No.:** 02996402725  
**Class:** C-20  
**Course:** Object-Oriented Programming Using C++ (CIC-211) — Maharaja Agrasen Institute of Technology  
**Option:** Option 1 — Autonomous Landslide & Flash-Flood Early Warning Grid (ALF-EWG)  
**Standard:** Modular C++17 (`-std=c++17 -Wall -Wextra -pedantic`)

## Project Structure

```text
ALF-EWG/
├── include/
│   ├── Exceptions.hpp          # Unit IV: Custom std::exception subclasses
│   ├── MathUtils.hpp           # Unit I: Namespaces, inline math, overloaded functions, references
│   ├── TelemetryBuffer.hpp     # Unit II: Dynamic memory buffer (Rule of Three)
│   ├── Entities.hpp            # Unit I & II: SensorStation, EarlyWarningSiren, statics, friend, operators
│   ├── SensorHierarchy.hpp     # Unit III: BaseSensorNode, PowerSubsystem containership, diamond resolution
│   └── PersistenceAndSTL.hpp   # Unit IV: Binary fstream logger (seekp/seekg), functors, manipulators
├── src/
│   ├── TelemetryBuffer.cpp
│   ├── Entities.cpp
│   ├── SensorHierarchy.cpp
│   ├── PersistenceAndSTL.cpp
│   └── main.cpp
├── Makefile
└── README.md
```

## Syllabus Mapping (CIC-211)

- **Unit I (Core Foundations & Call Semantics):**
  - Namespaces `DisasterCore::Sensors` and `DisasterCore::Alerts`
  - `inline` risk-indexing functions (`normalizeRiskScore`, `computeSlopeInstabilityFactor`)
  - Overloaded `computeHazardIndex()` for rainfall+slope, soil+vibration+acceleration, and river+velocity
  - Call-by-reference (`transformGeoCoordinates`) and return-by-reference (`accumulateTelemetryState`)
  - `friend class TelemetryAuditor` inspecting private `SensorStation` metrics
- **Unit II (Encapsulation, Dynamic Memory & Operator Overloading):**
  - `SensorStation`, `EarlyWarningSiren`, and `TelemetryBuffer` with `const` correctness
  - `static` data members & functions (`activeStationCount_`, `globalEmergencyLevel_`)
  - `TelemetryBuffer` with parameterized, dynamic, deep copy constructors, copy assignment, and explicit destructor
  - Overloaded `+` (cluster aggregation), `>` (hazard priority), `<<` and `>>` (stream I/O)
- **Unit III (Inheritance, Containership & Polymorphism):**
  - Abstract `BaseSensorNode` with pure virtual `readTelemetry()` and `evaluateThreshold()`
  - Diamond problem resolved via `virtual public DisasterTelemetryDevice` in `SolarPoweredNode` and `WirelessLoRaTransceiver`, joined in `AutonomousHybridProbe`
  - Containership (`PowerSubsystem` inside `BaseSensorNode`)
  - Runtime dynamic dispatch using `std::vector<BaseSensorNode*>`
- **Unit IV (Streams, Persistence, Exception Safety & STL):**
  - `SensorFailureException`, `TelemetryChecksumMismatch`, `ThresholdOverflowException`
  - Binary `std::fstream` logging (`incidents.bin`) with `seekp()`/`seekg()` transactional rollback
  - `std::vector`, `std::map`, `std::sort`, `std::for_each`, `EvacuationPriorityComparator` functor, and custom stream manipulators (`alertHeaderFormat`, `hazardScoreFormat`)

## Build & Run

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude src/*.cpp -o alf_ewg
./alf_ewg
```

Or using `make`:

```bash
make
./alf_ewg
```

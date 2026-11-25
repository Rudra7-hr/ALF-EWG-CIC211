#ifndef DISASTER_CORE_SENSOR_HIERARCHY_HPP
#define DISASTER_CORE_SENSOR_HIERARCHY_HPP

#include <iostream>
#include <string>
#include <vector>

namespace DisasterCore {
namespace Sensors {

// Unit III: Containership (Aggregation/Composition) - independent PowerSubsystem composed inside BaseSensorNode
class PowerSubsystem {
private:
    double batteryVoltage_;
    double solarChargeWatts_;
    bool lowPowerMode_;

public:
    PowerSubsystem(double voltage = 12.6, double solarWatts = 18.5);

    void updatePower(double solarWatts, double drawWatts);
    double getBatteryVoltage() const noexcept { return batteryVoltage_; }
    double getSolarChargeWatts() const noexcept { return solarChargeWatts_; }
    bool isLowPower() const noexcept { return lowPowerMode_; }
};

// Unit III: Abstract base class with pure virtual functions readTelemetry() and evaluateThreshold()
class BaseSensorNode {
protected:
    std::string nodeId_;
    std::string location_;
    PowerSubsystem powerUnit_; // Containership (has-a relationship)
    double lastHazardScore_;

public:
    BaseSensorNode(const std::string& id, const std::string& location,
                   double voltage = 12.6, double solarWatts = 20.0);
    virtual ~BaseSensorNode() = default;

    // Pure virtual functions
    virtual double readTelemetry() = 0;
    virtual bool evaluateThreshold() const = 0;

    virtual std::string getNodeType() const = 0;

    const std::string& getNodeId() const noexcept { return nodeId_; }
    const std::string& getLocation() const noexcept { return location_; }
    double getLastHazardScore() const noexcept { return lastHazardScore_; }
    const PowerSubsystem& getPowerSubsystem() const noexcept { return powerUnit_; }
};

// Concrete derived node 1: River-Level Sonar Gauge
class RiverSonarNode : public BaseSensorNode {
private:
    double riverLevelMeters_;
    double flowVelocityMs_;
    double dangerMarkMeters_;

public:
    RiverSonarNode(const std::string& id, const std::string& location,
                   double levelM, double velocityMs, double dangerM);

    double readTelemetry() override;
    bool evaluateThreshold() const override;
    std::string getNodeType() const override { return "RiverSonarNode"; }
};

// Concrete derived node 2: Seismic Vibration Geophone
class SeismicGeophoneNode : public BaseSensorNode {
private:
    double soilSaturationPct_;
    double vibrationHz_;
    double groundAccelG_;

public:
    SeismicGeophoneNode(const std::string& id, const std::string& location,
                        double saturationPct, double vibHz, double accelG);

    double readTelemetry() override;
    bool evaluateThreshold() const override;
    std::string getNodeType() const override { return "SeismicGeophoneNode"; }
};

// Unit III: Diamond Problem Resolution using Virtual Base Classes
// Root: DisasterTelemetryDevice
class DisasterTelemetryDevice {
protected:
    std::string hardwareSerial_;
    std::string firmwareVersion_;

public:
    DisasterTelemetryDevice(const std::string& serial = "DTD-ROOT-001",
                            const std::string& fw = "v2.4.1");
    virtual ~DisasterTelemetryDevice() = default;

    const std::string& getHardwareSerial() const noexcept { return hardwareSerial_; }
    const std::string& getFirmwareVersion() const noexcept { return firmwareVersion_; }
};

// Intermediate derivation 1 (virtually inherited)
class SolarPoweredNode : virtual public DisasterTelemetryDevice {
protected:
    double panelEfficiencyPct_;

public:
    SolarPoweredNode(const std::string& serial = "DTD-SOLAR",
                     const std::string& fw = "v2.4.1",
                     double efficiency = 21.8);
    double getPanelEfficiency() const noexcept { return panelEfficiencyPct_; }
};

// Intermediate derivation 2 (virtually inherited)
class WirelessLoRaTransceiver : virtual public DisasterTelemetryDevice {
protected:
    double frequencyMHz_;
    int signalRssiDbm_;

public:
    WirelessLoRaTransceiver(const std::string& serial = "DTD-LORA",
                            const std::string& fw = "v2.4.1",
                            double freqMHz = 868.0,
                            int rssiDbm = -74);
    double getFrequencyMHz() const noexcept { return frequencyMHz_; }
    int getSignalRssiDbm() const noexcept { return signalRssiDbm_; }
};

// Derived leaf: AutonomousHybridProbe (resolves diamond ambiguity + implements BaseSensorNode)
class AutonomousHybridProbe : public SolarPoweredNode,
                              public WirelessLoRaTransceiver,
                              public BaseSensorNode {
private:
    double rainfallRateMmHr_;
    double slopeAngleDeg_;
    unsigned int expectedChecksum_;
    unsigned int actualChecksum_;

public:
    AutonomousHybridProbe(const std::string& id,
                          const std::string& location,
                          const std::string& serial,
                          const std::string& fw,
                          double rainfallMmHr,
                          double slopeDeg,
                          unsigned int expectedChecksum,
                          unsigned int actualChecksum);

    double readTelemetry() override;
    bool evaluateThreshold() const override;
    std::string getNodeType() const override { return "AutonomousHybridProbe"; }
};

} // namespace Sensors
} // namespace DisasterCore

#endif // DISASTER_CORE_SENSOR_HIERARCHY_HPP

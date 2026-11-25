#include "SensorHierarchy.hpp"
#include "Exceptions.hpp"
#include "MathUtils.hpp"

namespace DisasterCore {
namespace Sensors {

PowerSubsystem::PowerSubsystem(double voltage, double solarWatts)
    : batteryVoltage_(voltage),
      solarChargeWatts_(solarWatts),
      lowPowerMode_(voltage < 11.2) {}

void PowerSubsystem::updatePower(double solarWatts, double drawWatts) {
    solarChargeWatts_ = solarWatts;
    batteryVoltage_ += (solarWatts - drawWatts) * 0.02;
    if (batteryVoltage_ > 14.4) batteryVoltage_ = 14.4;
    if (batteryVoltage_ < 9.5) batteryVoltage_ = 9.5;
    lowPowerMode_ = (batteryVoltage_ < 11.2);
}

BaseSensorNode::BaseSensorNode(const std::string& id,
                               const std::string& location,
                               double voltage,
                               double solarWatts)
    : nodeId_(id),
      location_(location),
      powerUnit_(voltage, solarWatts),
      lastHazardScore_(0.0) {}

RiverSonarNode::RiverSonarNode(const std::string& id,
                               const std::string& location,
                               double levelM,
                               double velocityMs,
                               double dangerM)
    : BaseSensorNode(id, location, 12.8, 22.0),
      riverLevelMeters_(levelM),
      flowVelocityMs_(velocityMs),
      dangerMarkMeters_(dangerM) {}

double RiverSonarNode::readTelemetry() {
    if (riverLevelMeters_ < 0.0 || riverLevelMeters_ > 45.0) {
        throw Alerts::ThresholdOverflowException(riverLevelMeters_, 45.0);
    }
    lastHazardScore_ = computeHazardIndex(riverLevelMeters_, flowVelocityMs_, dangerMarkMeters_, true);
    return lastHazardScore_;
}

bool RiverSonarNode::evaluateThreshold() const {
    return lastHazardScore_ >= 65.0;
}

SeismicGeophoneNode::SeismicGeophoneNode(const std::string& id,
                                         const std::string& location,
                                         double saturationPct,
                                         double vibHz,
                                         double accelG)
    : BaseSensorNode(id, location, 12.4, 16.0),
      soilSaturationPct_(saturationPct),
      vibrationHz_(vibHz),
      groundAccelG_(accelG) {}

double SeismicGeophoneNode::readTelemetry() {
    if (soilSaturationPct_ > 100.0) {
        throw Alerts::ThresholdOverflowException(soilSaturationPct_, 100.0);
    }
    if (vibrationHz_ < 0.0) {
        throw Alerts::SensorFailureException(nodeId_, "Negative seismic frequency reading");
    }
    lastHazardScore_ = computeHazardIndex(soilSaturationPct_, vibrationHz_, groundAccelG_);
    return lastHazardScore_;
}

bool SeismicGeophoneNode::evaluateThreshold() const {
    return lastHazardScore_ >= 60.0;
}

DisasterTelemetryDevice::DisasterTelemetryDevice(const std::string& serial, const std::string& fw)
    : hardwareSerial_(serial),
      firmwareVersion_(fw) {}

SolarPoweredNode::SolarPoweredNode(const std::string& serial, const std::string& fw, double efficiency)
    : DisasterTelemetryDevice(serial, fw),
      panelEfficiencyPct_(efficiency) {}

WirelessLoRaTransceiver::WirelessLoRaTransceiver(const std::string& serial,
                                                 const std::string& fw,
                                                 double freqMHz,
                                                 int rssiDbm)
    : DisasterTelemetryDevice(serial, fw),
      frequencyMHz_(freqMHz),
      signalRssiDbm_(rssiDbm) {}

AutonomousHybridProbe::AutonomousHybridProbe(const std::string& id,
                                             const std::string& location,
                                             const std::string& serial,
                                             const std::string& fw,
                                             double rainfallMmHr,
                                             double slopeDeg,
                                             unsigned int expectedChecksum,
                                             unsigned int actualChecksum)
    : DisasterTelemetryDevice(serial, fw),
      SolarPoweredNode(serial, fw, 22.5),
      WirelessLoRaTransceiver(serial, fw, 868.0, -71),
      BaseSensorNode(id, location, 13.1, 25.0),
      rainfallRateMmHr_(rainfallMmHr),
      slopeAngleDeg_(slopeDeg),
      expectedChecksum_(expectedChecksum),
      actualChecksum_(actualChecksum) {}

double AutonomousHybridProbe::readTelemetry() {
    if (expectedChecksum_ != actualChecksum_) {
        throw Alerts::TelemetryChecksumMismatch(expectedChecksum_, actualChecksum_);
    }
    lastHazardScore_ = computeHazardIndex(rainfallRateMmHr_, slopeAngleDeg_);
    return lastHazardScore_;
}

bool AutonomousHybridProbe::evaluateThreshold() const {
    return lastHazardScore_ >= 55.0;
}

} // namespace Sensors
} // namespace DisasterCore

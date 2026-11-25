#ifndef DISASTER_CORE_MATH_UTILS_HPP
#define DISASTER_CORE_MATH_UTILS_HPP

#include <cmath>
#include <string>

namespace DisasterCore {
namespace Sensors {

struct GeoCoordinate {
    double latitude;
    double longitude;
    double elevationMeters;
};

struct TelemetryAccumulator {
    double cumulativeRainfallMm;
    double peakVibrationHz;
    int sampleCount;
};

// Unit I: Inline mathematical risk-indexing algorithms for real-time calculations
inline double normalizeRiskScore(double rawScore, double maxScale = 100.0) {
    if (rawScore < 0.0) return 0.0;
    return (rawScore > maxScale) ? maxScale : rawScore;
}

inline double computeSlopeInstabilityFactor(double slopeAngleDeg) {
    constexpr double PI = 3.14159265358979323846;
    double rad = slopeAngleDeg * PI / 180.0;
    return std::sin(rad) * 1.45;
}

// Unit I: Function overloading for computeHazardIndex()
// Overload 1: Rainfall rate (mm/hr) + Slope angle (degrees) -> Landslide rainfall risk
inline double computeHazardIndex(double rainfallRateMmHr, double slopeAngleDeg) {
    double instability = computeSlopeInstabilityFactor(slopeAngleDeg);
    return normalizeRiskScore(rainfallRateMmHr * 0.65 * (1.0 + instability));
}

// Overload 2: Soil saturation (%) + Seismic vibration frequency (Hz) + Peak ground accel (g)
inline double computeHazardIndex(double soilSaturationPct, double vibrationHz, double groundAccelG) {
    double raw = (soilSaturationPct * 0.45) + (vibrationHz * 1.25) + (groundAccelG * 40.0);
    return normalizeRiskScore(raw);
}

// Overload 3: River level (m) + Flow velocity (m/s) + Danger mark (m) -> Flash-flood risk
inline double computeHazardIndex(double riverLevelM, double flowVelocityMs, double dangerMarkM, bool isFlashFloodMode) {
    (void)isFlashFloodMode;
    double ratio = (dangerMarkM > 0.0) ? (riverLevelM / dangerMarkM) : 1.0;
    double raw = (ratio * 55.0) + (flowVelocityMs * 6.5);
    return normalizeRiskScore(raw);
}

// Unit I: Call by reference for high-frequency geocoordinate transformations (e.g., local terrain datum shift)
inline void transformGeoCoordinates(GeoCoordinate& coord, double latOffset, double lonOffset, double elevDeltaM) {
    coord.latitude += latOffset;
    coord.longitude += lonOffset;
    coord.elevationMeters += elevDeltaM;
}

// Unit I: Call by reference AND return by reference for high-frequency telemetry state accumulation
inline TelemetryAccumulator& accumulateTelemetryState(TelemetryAccumulator& acc,
                                                      double rainfallSampleMm,
                                                      double vibrationHz) {
    acc.cumulativeRainfallMm += rainfallSampleMm;
    if (vibrationHz > acc.peakVibrationHz) {
        acc.peakVibrationHz = vibrationHz;
    }
    ++acc.sampleCount;
    return acc;
}

} // namespace Sensors
} // namespace DisasterCore

#endif // DISASTER_CORE_MATH_UTILS_HPP

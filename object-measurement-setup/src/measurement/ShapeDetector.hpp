#pragma once
#include <opencv2/core.hpp>
#include <string>
#include <vector>
#include "DetectedShape.hpp"
#include "../core/AppConfig.hpp"

// ---------------------------------------------------------------------------
// ShapeDetector — abstract interface for shape classifiers.
//
// To add a new shape:
//   1. Create detectors/MyShape{.hpp,.cpp} inheriting from ShapeDetector.
//   2. Implement detect() and name().
//   3. Register with engine->registerDetector(std::make_unique<MyShape>())
//      in main.cpp — nothing else needs to change.
// ---------------------------------------------------------------------------
class ShapeDetector {
public:
    virtual ~ShapeDetector() = default;

    // Returns true if this detector claims the contour and populates `out`.
    // Returns false if the contour does not match this shape type.
    virtual bool detect(
        const std::vector<cv::Point>& approxPoly,
        const std::vector<cv::Point>& rawContour,
        const CalibrationState&       cal,
        const AppConfig&              config,
        DetectedShape&                out) const = 0;

    // Human-readable name for logging and UI labels.
    virtual std::string name() const = 0;
};

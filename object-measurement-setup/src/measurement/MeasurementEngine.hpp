#pragma once
#include <opencv2/core.hpp>
#include <memory>
#include <vector>
#include "DetectedShape.hpp"
#include "ShapeDetector.hpp"
#include "../core/AppConfig.hpp"

// ---------------------------------------------------------------------------
// MeasurementEngine — finds contours in a preprocessed frame, filters by
// minimum area, and dispatches each contour through registered detectors.
// The first detector whose detect() returns true claims the contour.
//
// Registration order = priority order.
// ---------------------------------------------------------------------------
class MeasurementEngine {
public:
    explicit MeasurementEngine(const AppConfig& config);

    // Register a detector. Called at startup before the main loop.
    void registerDetector(std::unique_ptr<ShapeDetector> detector);

    // Run all detectors over every qualifying contour in `processedFrame`
    // (expects the eroded/dilated output from Preprocessor).
    // Returns all detected shapes; does not draw anything.
    std::vector<DetectedShape> detect(const cv::Mat&          processedFrame,
                                       const CalibrationState& cal) const;

private:
    AppConfig config_;
    std::vector<std::unique_ptr<ShapeDetector>> detectors_;
};

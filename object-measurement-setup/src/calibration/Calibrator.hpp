#pragma once
#include <opencv2/core.hpp>
#include <string>
#include "../core/AppConfig.hpp"

// ---------------------------------------------------------------------------
// CalibrationResult — returned by Calibrator::run().
// ---------------------------------------------------------------------------
struct CalibrationResult {
    bool             success = false;
    CalibrationState state;           // populated only when success == true
    std::string      message;         // status text for the sidebar overlay
};

// ---------------------------------------------------------------------------
// Calibrator — detects the reference coin in a preprocessed frame and
// computes the pixel-to-mm conversion ratio.
// ---------------------------------------------------------------------------
class Calibrator {
public:
    explicit Calibrator(const AppConfig& config);

    // Runs coin detection on `processedFrame` (eroded/dilated output).
    // Draws the detection overlay (green circle + label) onto `displayFrame`.
    CalibrationResult run(const cv::Mat& processedFrame,
                          cv::Mat&       displayFrame) const;

private:
    AppConfig config_;
};

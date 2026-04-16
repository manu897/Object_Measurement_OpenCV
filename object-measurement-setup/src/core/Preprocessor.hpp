#pragma once
#include <opencv2/core.hpp>
#include "AppConfig.hpp"

// ---------------------------------------------------------------------------
// PreprocessedFrame — holds every stage of the image pipeline so callers
// can choose whichever output they need (e.g. canny for display, eroded for
// contour detection).
// ---------------------------------------------------------------------------
struct PreprocessedFrame {
    cv::Mat gray;
    cv::Mat blurred;
    cv::Mat canny;
    cv::Mat dilated;
    cv::Mat eroded;
};

// ---------------------------------------------------------------------------
// Preprocessor — stateless image pipeline.
// Same inputs always produce the same outputs; no global state.
// ---------------------------------------------------------------------------
class Preprocessor {
public:
    explicit Preprocessor(const AppConfig& config);

    PreprocessedFrame process(const cv::Mat&        frame,
                              const ThresholdParams& thresholds) const;

private:
    cv::Mat   kernelDilate_;
    cv::Mat   kernelErode_;
    AppConfig config_;
};

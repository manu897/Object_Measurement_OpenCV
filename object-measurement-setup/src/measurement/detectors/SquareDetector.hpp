#pragma once
#include "../ShapeDetector.hpp"

class SquareDetector : public ShapeDetector {
public:
    bool detect(const std::vector<cv::Point>& approxPoly,
                const std::vector<cv::Point>& rawContour,
                const CalibrationState&       cal,
                const AppConfig&              config,
                DetectedShape&                out) const override;

    std::string name() const override { return "Square"; }
};

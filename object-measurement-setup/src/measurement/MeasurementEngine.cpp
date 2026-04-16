#include "MeasurementEngine.hpp"
#include <opencv2/imgproc.hpp>
#include <utility>

MeasurementEngine::MeasurementEngine(const AppConfig& config)
    : config_(config)
{}

void MeasurementEngine::registerDetector(std::unique_ptr<ShapeDetector> detector)
{
    detectors_.push_back(std::move(detector));
}

std::vector<DetectedShape> MeasurementEngine::detect(
    const cv::Mat&          processedFrame,
    const CalibrationState& cal) const
{
    std::vector<DetectedShape> results;

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i>              hierarchy;
    cv::findContours(processedFrame, contours, hierarchy,
                     cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& contour : contours) {
        if (cv::contourArea(contour) <= config_.minContourArea) continue;

        std::vector<cv::Point> approx;
        double peri = cv::arcLength(contour, true);
        cv::approxPolyDP(contour, approx, 0.02 * peri, true);

        for (const auto& detector : detectors_) {
            DetectedShape shape;
            if (detector->detect(approx, contour, cal, config_, shape)) {
                results.push_back(shape);
                break;   // first match wins
            }
        }
    }

    return results;
}

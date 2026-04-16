#include "Calibrator.hpp"
#include <opencv2/imgproc.hpp>
#include <cstdio>

Calibrator::Calibrator(const AppConfig& config)
    : config_(config)
{}

CalibrationResult Calibrator::run(const cv::Mat& processedFrame,
                                   cv::Mat&       displayFrame) const
{
    CalibrationResult result;

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i>              hierarchy;
    cv::findContours(processedFrame, contours, hierarchy,
                     cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    result.state.shapeCount = static_cast<int>(contours.size());

    for (size_t i = 0; i < contours.size(); ++i) {
        double area = cv::contourArea(contours[i]);
        if (area <= config_.minContourArea) continue;

        std::vector<cv::Point> approx;
        double peri = cv::arcLength(contours[i], true);
        cv::approxPolyDP(contours[i], approx, 0.02 * peri, true);

        // Coin is circular — identified by high vertex count after approx
        if (static_cast<int>(approx.size()) > config_.circleVertexMin) {
            cv::Point2f center;
            float       radius = 0.f;
            cv::minEnclosingCircle(approx, center, radius);

            if (radius <= 0.f) continue;

            double diaPx     = 2.0 * radius;
            double pixelToMM = config_.coinRealDiamMM / diaPx;
            double diaCm     = diaPx * pixelToMM * config_.mmToCm;

            // Draw detection onto display frame
            cv::circle(displayFrame,
                       center,
                       static_cast<int>(radius),
                       cv::Scalar(0, 255, 0),
                       static_cast<int>(config_.fontThickness));

            char buf[64];
            std::snprintf(buf, sizeof(buf), "Coin Dia: %.2fcm", diaCm);
            cv::putText(displayFrame, buf,
                        cv::Point(approx[0].x, approx[0].y),
                        cv::FONT_ITALIC,
                        config_.fontScale,
                        cv::Scalar(0, 0, 255),
                        config_.fontThickness);

            result.success       = true;
            result.state.valid   = true;
            result.state.pixelToMM = pixelToMM;
            result.state.coinDiaPx = diaPx;

            char msgBuf[128];
            std::snprintf(msgBuf, sizeof(msgBuf),
                          "Calibrated: %.4f mm/px  (coin = %.2fcm)",
                          pixelToMM, diaCm);
            result.message = msgBuf;
            break;   // use first detected coin
        }
    }

    if (!result.success) {
        result.message = "Coin not detected — place 2p coin in frame";
    }

    return result;
}

#include "SquareDetector.hpp"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <cstdio>

bool SquareDetector::detect(const std::vector<cv::Point>& approxPoly,
                              const std::vector<cv::Point>& rawContour,
                              const CalibrationState&       cal,
                              const AppConfig&              config,
                              DetectedShape&                out) const
{
    if (approxPoly.size() != 4) return false;

    cv::RotatedRect rr = cv::minAreaRect(rawContour);
    cv::Point2f     pts[4];
    rr.points(pts);

    double w = cv::norm(pts[0] - pts[1]);
    double h = cv::norm(pts[1] - pts[2]);
    if (h <= 0.0) return false;

    double aspRatio = w / h;
    if (aspRatio < config.squareAspLow || aspRatio > config.squareAspHigh)
        return false;

    double area = cv::contourArea(rawContour);
    double side = std::sqrt(area) * (cal.pixelToMM + config.calErrorAdj) * config.mmToCm;

    char buf[64];
    std::snprintf(buf, sizeof(buf), "Square Side: %.2fcm", side);

    out.type       = ShapeType::SQUARE;
    out.label      = buf;
    out.labelOrigin = cv::Point(static_cast<int>(pts[0].x),
                                static_cast<int>(pts[0].y));
    for (int i = 0; i < 4; ++i) out.rectPoints[i] = pts[i];
    out.hasRect    = true;

    return true;
}

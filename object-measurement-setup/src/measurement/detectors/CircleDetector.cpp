#include "CircleDetector.hpp"
#include <opencv2/imgproc.hpp>
#include <cstdio>

bool CircleDetector::detect(const std::vector<cv::Point>& approxPoly,
                             const std::vector<cv::Point>& /*rawContour*/,
                             const CalibrationState&       cal,
                             const AppConfig&              config,
                             DetectedShape&                out) const
{
    if (static_cast<int>(approxPoly.size()) <= config.circleVertexMin)
        return false;

    cv::Point2f center;
    float       radius = 0.f;
    cv::minEnclosingCircle(approxPoly, center, radius);

    if (radius <= 0.f) return false;

    double diaCm = (2.0 * radius) * (cal.pixelToMM + config.calErrorAdj) * config.mmToCm;

    char buf[64];
    std::snprintf(buf, sizeof(buf), "Circle Dia: %.2fcm", diaCm);

    out.type         = ShapeType::CIRCLE;
    out.label        = buf;
    out.labelOrigin  = cv::Point(approxPoly[0].x, approxPoly[0].y);
    out.circleCenter = center;
    out.circleRadius = radius;

    return true;
}

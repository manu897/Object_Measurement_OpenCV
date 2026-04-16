#include "Overlay.hpp"
#include <opencv2/imgproc.hpp>

void Overlay::drawCrosshair(cv::Mat&          frame,
                              cv::Point         center,
                              int               size,
                              const cv::Scalar& color,
                              int               thickness)
{
    cv::line(frame,
             cv::Point(center.x - size, center.y),
             cv::Point(center.x + size, center.y),
             color, thickness);
    cv::line(frame,
             cv::Point(center.x, center.y - size),
             cv::Point(center.x, center.y + size),
             color, thickness);
}

void Overlay::drawStatusLines(cv::Mat&                        frame,
                               const std::vector<std::string>& lines,
                               cv::Point                       origin,
                               double                          scale,
                               const cv::Scalar&               color,
                               int                             thickness)
{
    int lineHeight = static_cast<int>(20 * scale / 0.4);
    for (size_t i = 0; i < lines.size(); ++i) {
        cv::Point pos(origin.x, origin.y + static_cast<int>(i) * lineHeight);
        cv::putText(frame, lines[i], pos,
                    cv::FONT_ITALIC, scale, color, thickness);
    }
}

void Overlay::drawShapes(cv::Mat&                          frame,
                          const std::vector<DetectedShape>& shapes,
                          const AppConfig&                  config)
{
    for (const auto& s : shapes) {
        if (s.type == ShapeType::CIRCLE) {
            cv::circle(frame,
                       s.circleCenter,
                       static_cast<int>(s.circleRadius),
                       cv::Scalar(0, 255, 0),
                       config.fontThickness);
        } else if (s.hasRect) {
            // Draw rotated rectangle from its 4 corner points
            for (int i = 0; i < 4; ++i) {
                cv::line(frame,
                         s.rectPoints[i],
                         s.rectPoints[(i + 1) % 4],
                         cv::Scalar(0, 255, 0),
                         config.fontThickness);
            }
        }

        cv::putText(frame, s.label, s.labelOrigin,
                    cv::FONT_ITALIC,
                    config.fontScale,
                    cv::Scalar(0, 0, 255),
                    config.fontThickness);
    }
}

void Overlay::drawStateBadge(cv::Mat&           frame,
                               const std::string& label,
                               const cv::Scalar&  bgColor)
{
    int badgeW = 160, badgeH = 28;
    int x = frame.cols - badgeW - 6;
    int y = 6;

    cv::rectangle(frame,
                  cv::Rect(x, y, badgeW, badgeH),
                  bgColor, cv::FILLED);
    cv::putText(frame, label,
                cv::Point(x + 8, y + 19),
                cv::FONT_HERSHEY_SIMPLEX,
                0.55,
                cv::Scalar(255, 255, 255),
                1);
}

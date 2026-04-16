#pragma once
#include <opencv2/core.hpp>
#include <string>
#include <vector>
#include "../core/AppConfig.hpp"
#include "../measurement/DetectedShape.hpp"

// ---------------------------------------------------------------------------
// Overlay — stateless drawing helpers.
// All functions write onto the provided Mat and return void.
// ---------------------------------------------------------------------------
class Overlay {
public:
    // Centered crosshair (used during WELCOME and CALIBRATING states)
    static void drawCrosshair(cv::Mat&         frame,
                               cv::Point        center,
                               int              size,
                               const cv::Scalar& color,
                               int              thickness = 2);

    // Multi-line status text, each string on its own row
    static void drawStatusLines(cv::Mat&                       frame,
                                 const std::vector<std::string>& lines,
                                 cv::Point                      origin,
                                 double                         scale,
                                 const cv::Scalar&              color,
                                 int                            thickness);

    // Draw detected shapes — circles and rotated rectangles with labels
    static void drawShapes(cv::Mat&                          frame,
                            const std::vector<DetectedShape>& shapes,
                            const AppConfig&                  config);

    // Colored badge in top-right corner showing the current AppState name
    static void drawStateBadge(cv::Mat&           frame,
                                const std::string& label,
                                const cv::Scalar&  bgColor);
};

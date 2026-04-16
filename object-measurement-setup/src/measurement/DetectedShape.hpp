#pragma once
#include <opencv2/core.hpp>
#include <string>

// ---------------------------------------------------------------------------
// ShapeType — identifies the kind of shape a detector classified.
// ---------------------------------------------------------------------------
enum class ShapeType {
    CIRCLE,
    RECTANGLE,
    SQUARE,
    UNKNOWN
};

// ---------------------------------------------------------------------------
// DetectedShape — result produced by a ShapeDetector and consumed by both
// MeasurementEngine (aggregation) and Overlay (drawing).
// ---------------------------------------------------------------------------
struct DetectedShape {
    ShapeType   type        = ShapeType::UNKNOWN;
    std::string label;          // formatted measurement string, e.g. "Circle Dia: 3.20cm"
    cv::Point   labelOrigin;    // pixel position for putText

    // Circle geometry (populated when type == CIRCLE)
    cv::Point2f circleCenter;
    float       circleRadius = 0.f;

    // Quadrilateral geometry (populated when type == RECTANGLE or SQUARE)
    cv::Point2f rectPoints[4];
    bool        hasRect = false;
};

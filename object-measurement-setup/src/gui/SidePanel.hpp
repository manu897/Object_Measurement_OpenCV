#pragma once
#include <opencv2/core.hpp>
#include <string>
#include <vector>
#include "../core/AppConfig.hpp"
#include "../measurement/DetectedShape.hpp"

// ---------------------------------------------------------------------------
// SidePanel — owns the 300px-wide right column of the composite canvas.
//
// Layout (top → bottom, within the 300×480 sidebar):
//   [0  .. 40 ] State badge
//   [40 .. 265] Canny preview (300×225, scaled from live resolution)
//               — only shown in MEASURING state
//   [265.. 345] Threshold sliders T1 and T2 (custom drawn)
//               — only shown in MEASURING state
//   [345.. 435] Action buttons (state-specific)
//   [435.. 480] Stats footer
//
// In WELCOME / CALIBRATING, the canny+slider area shows instruction text.
// ---------------------------------------------------------------------------
class SidePanel {
public:
    explicit SidePanel(const AppConfig& config, ThresholdParams& thresholds);

    // Returns a freshly rendered 300×480 BGR Mat.
    cv::Mat render(AppState                          state,
                   const cv::Mat&                    cannyFrame,
                   const CalibrationState&           cal,
                   const std::vector<DetectedShape>& shapes) const;

    // Mouse coordinates are relative to the side panel's top-left (0,0).
    // Returns NONE unless a button was clicked.
    UserIntent onMouse(int event, int x, int y, AppState state);

private:
    // ---- Layout helpers ----
    struct Button {
        std::string label;
        cv::Rect    bounds;
        UserIntent  intent;
        bool        hovered = false;
    };

    struct Slider {
        std::string label;
        cv::Rect    track;    // full track bar area
        int*        value;
        int         maxVal;
        bool        dragging = false;
    };

    void drawBadge  (cv::Mat& panel, AppState state) const;
    void drawCanny  (cv::Mat& panel, const cv::Mat& cannyFrame) const;
    void drawSliders(cv::Mat& panel) const;
    void drawButtons(cv::Mat& panel, AppState state) const;
    void drawStats  (cv::Mat& panel, const CalibrationState& cal,
                     const std::vector<DetectedShape>& shapes) const;

    std::vector<Button> buttonsForState(AppState state) const;
    cv::Rect            buttonRect(int index) const;
    cv::Rect            sliderRect(int index) const;

    AppConfig        config_;
    ThresholdParams& thresholds_;

    // Mutable so render() (logically const) can keep slider drag state
    mutable std::vector<Slider> sliders_;
    mutable std::vector<Button> buttons_;
};

#include "SidePanel.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cstdio>
#include <algorithm>

// ---- Layout constants (within the 300×480 side panel) -------------------
static const int kPanelW    = 300;
static const int kBadgeH    = 40;
static const int kCannyH    = 169;   // 300 * (480/640) ≈ 225; capped at 169 to leave room
static const int kSliderY   = kBadgeH + kCannyH;  // 209
static const int kSliderH   = 76;
static const int kButtonY   = kSliderY + kSliderH; // 285
static const int kButtonH   = 48;
static const int kButtonGap = 4;
static const int kStatsY    = 390;
static const int kPadX      = 12;

SidePanel::SidePanel(const AppConfig& config, ThresholdParams& thresholds)
    : config_(config)
    , thresholds_(thresholds)
{
    // Initialize sliders
    sliders_.resize(2);
    sliders_[0] = {"T1", sliderRect(0), &thresholds_.thresh1, 255, false};
    sliders_[1] = {"T2", sliderRect(1), &thresholds_.thresh2, 255, false};
}

// ---------------------------------------------------------------------------

cv::Rect SidePanel::sliderRect(int index) const
{
    int y = kSliderY + 8 + index * 36;
    return cv::Rect(kPadX, y, kPanelW - 2 * kPadX, 16);
}

cv::Rect SidePanel::buttonRect(int index) const
{
    int y = kButtonY + index * (kButtonH + kButtonGap);
    return cv::Rect(kPadX, y, kPanelW - 2 * kPadX, kButtonH);
}

std::vector<SidePanel::Button> SidePanel::buttonsForState(AppState state) const
{
    std::vector<Button> btns;
    if (state == AppState::WELCOME) {
        btns.push_back({"Start Calibration  (Y)", buttonRect(0), UserIntent::CONFIRM});
        btns.push_back({"Quit  (Q)",              buttonRect(1), UserIntent::QUIT});
    } else if (state == AppState::CALIBRATING) {
        btns.push_back({"Accept  (Y)",      buttonRect(0), UserIntent::CONFIRM});
        btns.push_back({"Retry   (N)",      buttonRect(1), UserIntent::RETRY});
        btns.push_back({"Quit    (Q)",      buttonRect(2), UserIntent::QUIT});
    } else if (state == AppState::MEASURING) {
        btns.push_back({"Recalibrate  (N)", buttonRect(0), UserIntent::RETRY});
        btns.push_back({"Quit  (Q)",        buttonRect(1), UserIntent::QUIT});
    }
    return btns;
}

// ---------------------------------------------------------------------------
// render
// ---------------------------------------------------------------------------

cv::Mat SidePanel::render(AppState                          state,
                           const cv::Mat&                    cannyFrame,
                           const CalibrationState&           cal,
                           const std::vector<DetectedShape>& shapes) const
{
    cv::Mat panel(config_.liveHeight, kPanelW, CV_8UC3, cv::Scalar(28, 28, 28));

    drawBadge(panel, state);

    if (state == AppState::MEASURING) {
        drawCanny  (panel, cannyFrame);
        drawSliders(panel);
    } else {
        // Show instruction text in the area where canny would appear
        std::vector<std::string> lines;
        if (state == AppState::WELCOME) {
            lines = {
                "Place a 2-pound coin",
                "in the centre of",
                "the camera frame,",
                "then press",
                "\"Start Calibration\"."
            };
        } else if (state == AppState::CALIBRATING) {
            lines = {
                "Coin detected =",
                "green circle shown.",
                "",
                "Press Accept when",
                "calibration looks",
                "correct, or Retry",
                "to scan again."
            };
        }
        int lineY = kBadgeH + 20;
        for (const auto& l : lines) {
            cv::putText(panel, l,
                        cv::Point(kPadX, lineY),
                        cv::FONT_HERSHEY_SIMPLEX,
                        0.46, cv::Scalar(180, 180, 180), 1);
            lineY += 22;
        }
    }

    drawButtons(panel, state);
    drawStats  (panel, cal, shapes);

    // Rebuild button list for onMouse (kept in sync)
    buttons_ = buttonsForState(state);
    // Rebuild sliders too (track rects are fixed but pointers must stay valid)
    sliders_[0] = {"T1", sliderRect(0), &thresholds_.thresh1, 255, sliders_[0].dragging};
    sliders_[1] = {"T2", sliderRect(1), &thresholds_.thresh2, 255, sliders_[1].dragging};

    return panel;
}

void SidePanel::drawBadge(cv::Mat& panel, AppState state) const
{
    cv::Scalar bg;
    std::string label;
    switch (state) {
        case AppState::WELCOME:      bg = cv::Scalar(150, 80,  0);   label = "SETUP";       break;
        case AppState::CALIBRATING:  bg = cv::Scalar(0,   150, 180); label = "CALIBRATING"; break;
        case AppState::MEASURING:    bg = cv::Scalar(0,   140, 0);   label = "MEASURING";   break;
        default:                     bg = cv::Scalar(60,  60,  60);   label = "...";         break;
    }
    cv::rectangle(panel, cv::Rect(0, 0, kPanelW, kBadgeH), bg, cv::FILLED);
    cv::putText(panel, label,
                cv::Point(kPadX, 26),
                cv::FONT_HERSHEY_SIMPLEX, 0.65,
                cv::Scalar(255, 255, 255), 2);
}

void SidePanel::drawCanny(cv::Mat& panel, const cv::Mat& cannyFrame) const
{
    if (cannyFrame.empty()) return;

    cv::Mat preview;
    cv::resize(cannyFrame, preview, cv::Size(kPanelW, kCannyH));

    // Convert single-channel canny to BGR for display in the BGR panel
    if (preview.channels() == 1) {
        cv::cvtColor(preview, preview, cv::COLOR_GRAY2BGR);
    }

    preview.copyTo(panel(cv::Rect(0, kBadgeH, kPanelW, kCannyH)));

    // Label
    cv::putText(panel, "Edge preview",
                cv::Point(kPadX, kBadgeH + kCannyH - 6),
                cv::FONT_HERSHEY_SIMPLEX, 0.38,
                cv::Scalar(140, 140, 140), 1);
}

void SidePanel::drawSliders(cv::Mat& panel) const
{
    // Section label
    cv::putText(panel, "Canny Thresholds",
                cv::Point(kPadX, kSliderY + 6),
                cv::FONT_HERSHEY_SIMPLEX, 0.42,
                cv::Scalar(160, 160, 160), 1);

    for (const auto& s : sliders_) {
        cv::Rect track = s.track;
        int filled = static_cast<int>(
            static_cast<double>(*s.value) / s.maxVal * track.width);

        // Track background
        cv::rectangle(panel, track, cv::Scalar(60, 60, 60), cv::FILLED);
        // Filled portion
        if (filled > 0) {
            cv::rectangle(panel,
                          cv::Rect(track.x, track.y, filled, track.height),
                          cv::Scalar(70, 130, 180), cv::FILLED);
        }
        // Track border
        cv::rectangle(panel, track, cv::Scalar(100, 100, 100), 1);

        // Label + value
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%s: %d", s.label.c_str(), *s.value);
        cv::putText(panel, buf,
                    cv::Point(track.x, track.y - 3),
                    cv::FONT_HERSHEY_SIMPLEX, 0.38,
                    cv::Scalar(200, 200, 200), 1);
    }
}

void SidePanel::drawButtons(cv::Mat& panel, AppState state) const
{
    auto btns = buttonsForState(state);
    for (size_t i = 0; i < btns.size(); ++i) {
        const auto& b  = btns[i];
        cv::Scalar  bg = (b.intent == UserIntent::QUIT)
                             ? cv::Scalar(40, 40, 120)
                             : (b.intent == UserIntent::CONFIRM)
                                   ? cv::Scalar(0, 100, 0)
                                   : cv::Scalar(70, 70, 70);

        cv::rectangle(panel, b.bounds, bg, cv::FILLED);
        cv::rectangle(panel, b.bounds, cv::Scalar(90, 90, 90), 1);
        cv::putText(panel, b.label,
                    cv::Point(b.bounds.x + 10, b.bounds.y + 30),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5,
                    cv::Scalar(230, 230, 230), 1);
    }
}

void SidePanel::drawStats(cv::Mat&                          panel,
                           const CalibrationState&           cal,
                           const std::vector<DetectedShape>& shapes) const
{
    int y = kStatsY;
    cv::line(panel,
             cv::Point(kPadX, y - 4),
             cv::Point(kPanelW - kPadX, y - 4),
             cv::Scalar(60, 60, 60), 1);

    auto text = [&](const std::string& s) {
        cv::putText(panel, s,
                    cv::Point(kPadX, y),
                    cv::FONT_HERSHEY_SIMPLEX, 0.4,
                    cv::Scalar(150, 150, 150), 1);
        y += 18;
    };

    if (cal.valid) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "Cal: %.4f mm/px", cal.pixelToMM);
        text(buf);
    } else {
        text("Cal: not set");
    }

    char shapesBuf[32];
    std::snprintf(shapesBuf, sizeof(shapesBuf),
                  "Shapes: %d", static_cast<int>(shapes.size()));
    text(shapesBuf);

    char camBuf[32];
    std::snprintf(camBuf, sizeof(camBuf), "Camera: %d", config_.cameraIndex);
    text(camBuf);
}

// ---------------------------------------------------------------------------
// onMouse
// ---------------------------------------------------------------------------

UserIntent SidePanel::onMouse(int event, int x, int y, AppState state)
{
    // ---- Slider interaction (only in MEASURING) ----
    if (state == AppState::MEASURING) {
        for (auto& s : sliders_) {
            if (event == cv::EVENT_LBUTTONDOWN && s.track.contains(cv::Point(x, y))) {
                s.dragging = true;
            }
            if (event == cv::EVENT_LBUTTONUP) {
                s.dragging = false;
            }
            if (s.dragging && event == cv::EVENT_MOUSEMOVE) {
                int rel    = x - s.track.x;
                rel        = std::max(0, std::min(rel, s.track.width));
                *s.value   = static_cast<int>(
                    static_cast<double>(rel) / s.track.width * s.maxVal);
            }
        }
    }

    // ---- Button clicks ----
    if (event == cv::EVENT_LBUTTONDOWN) {
        auto btns = buttonsForState(state);
        for (const auto& b : btns) {
            if (b.bounds.contains(cv::Point(x, y))) {
                return b.intent;
            }
        }
    }

    return UserIntent::NONE;
}

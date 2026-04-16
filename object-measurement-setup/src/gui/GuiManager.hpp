#pragma once
#include <opencv2/core.hpp>
#include <mutex>
#include <vector>
#include "../core/AppConfig.hpp"
#include "../measurement/DetectedShape.hpp"
#include "SidePanel.hpp"
#include "CameraSelector.hpp"

// ---------------------------------------------------------------------------
// GuiManager — owns the single composite window and all user-input routing.
//
// One cv::Mat canvas_ (940×520) is reused every frame:
//   [0 ..640)  x  [0 ..480)  → live feed
//   [640..940) x  [0 ..480)  → SidePanel
//   [0 ..940)  x  [480..520) → status bar
//
// A single setMouseCallback routes events to CameraSelector or SidePanel
// depending on the current AppState.  pollEvents() merges keyboard input
// (waitKey) with mouse clicks into one UserIntent per frame.
// ---------------------------------------------------------------------------
class GuiManager {
public:
    GuiManager(const AppConfig& config, ThresholdParams& thresholds);
    ~GuiManager();

    // Main render path — composites live feed + side panel + status bar
    // and calls imshow once.
    void render(AppState                          state,
                const cv::Mat&                    liveFrame,
                const cv::Mat&                    cannyFrame,
                const std::vector<DetectedShape>& shapes,
                const CalibrationState&           cal);

    // Camera selection render path (replaces the whole canvas)
    void renderCameraSelect(const std::vector<int>& cameras);

    // Non-blocking (waitKey(30)); merges keyboard + mouse → UserIntent.
    UserIntent pollEvents(AppState currentState);

    // Returns the camera index chosen on the selection screen (-1 = not yet)
    int getSelectedCamera() const;

private:
    static void mouseCallback(int event, int x, int y, int flags, void* userdata);
    void        onMouse(int event, int x, int y);

    void compositeStatusBar(const CalibrationState& cal,
                             AppState                state) const;

    AppConfig        config_;
    ThresholdParams& thresholds_;
    SidePanel        sidePanel_;
    CameraSelector   cameraSelector_;

    cv::Mat          canvas_;
    AppState         lastState_ = AppState::CAMERA_SELECT;

    // Pending mouse intent (written by callback thread, read + cleared by pollEvents)
    UserIntent       pendingIntent_ = UserIntent::NONE;
    std::mutex       intentMutex_;

    // Cached camera list so the mouse callback can route clicks correctly
    std::vector<int> availableCameras_;

    static const char* kWindowName;
};

#pragma once
#include <opencv2/core.hpp>
#include <vector>

// ---------------------------------------------------------------------------
// CameraSelector — detects available cameras and renders a selection screen.
//
// detectAvailable() probes VideoCapture(0..maxIndex-1) once at startup.
// render() draws the picker onto the full canvas.
// onMouse() handles clicks; getSelectedIndex() returns the confirmed choice.
// ---------------------------------------------------------------------------
class CameraSelector {
public:
    CameraSelector();

    // Probe camera indices [0, maxIndex).  Returns indices that open OK.
    static std::vector<int> detectAvailable(int maxIndex = 6);

    // Render camera picker onto `canvas` (full 940×520 canvas).
    void render(cv::Mat&                  canvas,
                const std::vector<int>&   available,
                int                       highlighted) const;

    // Mouse callback — coordinates in the full canvas space.
    void onMouse(int event, int x, int y,
                 const std::vector<int>& available);

    // Returns -1 until the user confirms a selection; then returns the index.
    int  getSelectedIndex() const { return confirmed_ ? selectedIndex_ : -1; }

    // Index currently highlighted (before confirmation)
    int  getHighlightedIndex() const { return highlightedIndex_; }

    void reset() { confirmed_ = false; selectedIndex_ = -1; highlightedIndex_ = 0; }

private:
    int  highlightedIndex_ = 0;
    int  selectedIndex_    = -1;
    bool confirmed_        = false;

    // Compute button rect for camera entry `i` in the available list
    cv::Rect cameraButtonRect(int i) const;
    cv::Rect continueButtonRect() const;
};

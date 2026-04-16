#include "CameraSelector.hpp"
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cstdio>

// Layout constants (relative to the 940×520 canvas)
static const int kCanvasW  = 940;
static const int kBtnW     = 480;
static const int kBtnH     = 44;
static const int kBtnX     = (kCanvasW - kBtnW) / 2;
static const int kFirstY   = 160;
static const int kBtnGap   = 54;
static const int kContBtnY = 420;

CameraSelector::CameraSelector() {}

std::vector<int> CameraSelector::detectAvailable(int maxIndex)
{
    std::vector<int> result;
    for (int i = 0; i < maxIndex; ++i) {
        cv::VideoCapture cap(i);
        if (cap.isOpened()) {
            result.push_back(i);
            cap.release();
        }
    }
    if (result.empty()) result.push_back(0);   // fallback
    return result;
}

cv::Rect CameraSelector::cameraButtonRect(int i) const
{
    return cv::Rect(kBtnX, kFirstY + i * kBtnGap, kBtnW, kBtnH);
}

cv::Rect CameraSelector::continueButtonRect() const
{
    return cv::Rect(kBtnX, kContBtnY, kBtnW, kBtnH);
}

void CameraSelector::render(cv::Mat&                canvas,
                              const std::vector<int>& available,
                              int                     highlighted) const
{
    // Dark background
    canvas.setTo(cv::Scalar(30, 30, 30));

    // Title
    cv::putText(canvas,
                "Object Measurement  —  Select Camera",
                cv::Point(kBtnX, 80),
                cv::FONT_HERSHEY_SIMPLEX, 0.9,
                cv::Scalar(220, 220, 220), 2);

    cv::putText(canvas,
                "Detected cameras:",
                cv::Point(kBtnX, 130),
                cv::FONT_HERSHEY_SIMPLEX, 0.55,
                cv::Scalar(160, 160, 160), 1);

    // Camera buttons
    for (int i = 0; i < static_cast<int>(available.size()); ++i) {
        cv::Rect  btn = cameraButtonRect(i);
        bool      sel = (i == highlighted);
        cv::Scalar bg = sel ? cv::Scalar(70, 130, 180) : cv::Scalar(55, 55, 55);
        cv::Scalar fg = cv::Scalar(230, 230, 230);

        cv::rectangle(canvas, btn, bg, cv::FILLED);
        cv::rectangle(canvas, btn, cv::Scalar(100, 100, 100), 1);

        char label[64];
        std::snprintf(label, sizeof(label), "%s  Camera %d",
                      sel ? ">" : " ", available[i]);
        cv::putText(canvas, label,
                    cv::Point(btn.x + 20, btn.y + 28),
                    cv::FONT_HERSHEY_SIMPLEX, 0.6, fg, 1);
    }

    // Continue button
    cv::Rect contBtn = continueButtonRect();
    cv::rectangle(canvas, contBtn, cv::Scalar(34, 139, 34), cv::FILLED);
    cv::rectangle(canvas, contBtn, cv::Scalar(100, 100, 100), 1);
    cv::putText(canvas, "  Continue  ->",
                cv::Point(contBtn.x + contBtn.width / 2 - 70,
                          contBtn.y + 28),
                cv::FONT_HERSHEY_SIMPLEX, 0.65,
                cv::Scalar(255, 255, 255), 2);
}

void CameraSelector::onMouse(int event, int x, int y,
                               const std::vector<int>& available)
{
    if (event != cv::EVENT_LBUTTONDOWN) return;

    // Check camera buttons
    for (int i = 0; i < static_cast<int>(available.size()); ++i) {
        if (cameraButtonRect(i).contains(cv::Point(x, y))) {
            highlightedIndex_ = i;
            return;
        }
    }

    // Check Continue button
    if (continueButtonRect().contains(cv::Point(x, y))) {
        selectedIndex_ = available[highlightedIndex_];
        confirmed_     = true;
    }
}

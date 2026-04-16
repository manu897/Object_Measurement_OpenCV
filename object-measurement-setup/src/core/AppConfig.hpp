#pragma once
#include <opencv2/core.hpp>

// ---------------------------------------------------------------------------
// AppConfig — compile-time defaults and tunable parameters.
// All other modules receive a const AppConfig& and must not modify it.
// ---------------------------------------------------------------------------
struct AppConfig {
    // Camera
    int cameraIndex = 0;

    // Canvas dimensions
    int canvasWidth     = 940;   // liveWidth + sidebarWidth
    int canvasHeight    = 520;   // liveHeight + statusBarHeight
    int liveWidth       = 640;
    int liveHeight      = 480;
    int sidebarWidth    = 300;
    int statusBarHeight = 40;

    // Calibration reference object (2-pound coin)
    double coinRealDiamMM = 28.4;
    double mmToCm         = 0.1;
    double calErrorAdj    = 0.002;   // systematic error correction factor

    // Contour filtering
    int    minContourArea  = 400;
    int    circleVertexMin = 7;       // approxPoly vertex count >= this → circle

    // Canny defaults
    int defaultThresh1 = 25;
    int defaultThresh2 = 75;

    // Shape aspect-ratio bounds for square vs rectangle
    double squareAspLow  = 0.95;
    double squareAspHigh = 1.05;

    // Morphological kernels
    cv::Size morphKernelSize = {3, 3};

    // Gaussian blur
    cv::Size gaussianKernel = {7, 7};
    double   gaussianSigma  = 3.0;

    // Rendering
    float fontScale     = 0.4f;
    int   fontThickness = 2;
};

// ---------------------------------------------------------------------------
// ThresholdParams — mutable Canny thresholds shared between AppController
// (which passes them to Preprocessor) and GuiManager (which exposes sliders).
// ---------------------------------------------------------------------------
struct ThresholdParams {
    int thresh1 = 25;
    int thresh2 = 75;
};

// ---------------------------------------------------------------------------
// CalibrationState — result of a successful calibration pass.
// ---------------------------------------------------------------------------
struct CalibrationState {
    bool   valid      = false;
    double pixelToMM  = 0.0;   // mm per pixel (the 'cal' value in original code)
    double coinDiaPx  = 0.0;   // measured diameter of coin in pixels
    int    shapeCount = 0;     // number of contours detected during calibration
};

// ---------------------------------------------------------------------------
// AppState — drives the top-level state machine in AppController.
// ---------------------------------------------------------------------------
enum class AppState {
    CAMERA_SELECT,
    WELCOME,
    CALIBRATING,
    MEASURING,
    EXITING
};

// ---------------------------------------------------------------------------
// UserIntent — unified intent signal produced by GuiManager (keyboard or
// mouse click) and consumed by AppController to drive state transitions.
// ---------------------------------------------------------------------------
enum class UserIntent {
    NONE,
    CONFIRM,         // Accept / Yes
    RETRY,           // Retry / Recalibrate
    QUIT,
    SELECT_CAMERA    // Camera index chosen in camera-select screen
};

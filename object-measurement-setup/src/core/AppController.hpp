#pragma once
#include <opencv2/videoio.hpp>
#include <memory>
#include <vector>
#include "AppConfig.hpp"
#include "Preprocessor.hpp"
#include "../calibration/Calibrator.hpp"
#include "../measurement/MeasurementEngine.hpp"
#include "../gui/GuiManager.hpp"

// ---------------------------------------------------------------------------
// AppController — owns the state machine and the main event loop.
//
// Responsibilities:
//   - Open VideoCapture once at construction; never re-open it
//   - Drive the CAMERA_SELECT → WELCOME → CALIBRATING → MEASURING → EXITING
//     state transitions in response to UserIntent signals
//   - Delegate image processing to Preprocessor / Calibrator / MeasurementEngine
//   - Delegate all rendering to GuiManager
// ---------------------------------------------------------------------------
class AppController {
public:
    explicit AppController(const AppConfig& config);

    // Blocks until the user quits.
    void run();

private:
    // Per-state tick functions — called once per frame
    void tickCameraSelect();
    void tickWelcome();
    void tickCalibrating();
    void tickMeasuring();

    void transitionTo(AppState next);
    void openCamera(int index);

    AppConfig         config_;
    ThresholdParams   thresholds_;
    CalibrationState  calState_;
    AppState          state_;

    cv::VideoCapture  capture_;

    std::unique_ptr<Preprocessor>      preprocessor_;
    std::unique_ptr<Calibrator>        calibrator_;
    std::unique_ptr<MeasurementEngine> engine_;
    std::unique_ptr<GuiManager>        gui_;

    std::vector<int>  availableCameras_;

    // Last calibration display frame (kept so Calibration can re-show it)
    cv::Mat lastCalFrame_;
    CalibrationResult lastCalResult_;
};

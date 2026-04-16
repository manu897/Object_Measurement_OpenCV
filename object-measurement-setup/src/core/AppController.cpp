#include "AppController.hpp"
#include "../measurement/detectors/CircleDetector.hpp"
#include "../measurement/detectors/RectangleDetector.hpp"
#include "../measurement/detectors/SquareDetector.hpp"
#include <iostream>

AppController::AppController(const AppConfig& config)
    : config_(config)
    , thresholds_{config.defaultThresh1, config.defaultThresh2}
    , state_(AppState::CAMERA_SELECT)
{
    preprocessor_ = std::make_unique<Preprocessor>(config_);
    calibrator_   = std::make_unique<Calibrator>(config_);

    engine_ = std::make_unique<MeasurementEngine>(config_);
    // Registration order = priority order.
    // Square must be checked before Rectangle (both have 4 vertices).
    engine_->registerDetector(std::make_unique<CircleDetector>());
    engine_->registerDetector(std::make_unique<SquareDetector>());
    engine_->registerDetector(std::make_unique<RectangleDetector>());

    gui_ = std::make_unique<GuiManager>(config_, thresholds_);

    // Detect available cameras once at startup
    std::cout << "Scanning for cameras..." << std::endl;
    availableCameras_ = CameraSelector::detectAvailable();
    std::cout << "Found " << availableCameras_.size() << " camera(s)." << std::endl;
}

// ---------------------------------------------------------------------------

void AppController::openCamera(int index)
{
    if (capture_.isOpened()) capture_.release();
    capture_.open(index);
    if (!capture_.isOpened()) {
        std::cerr << "Warning: could not open camera " << index << std::endl;
    }
}

void AppController::transitionTo(AppState next)
{
    state_ = next;
}

// ---------------------------------------------------------------------------
// run — main event loop
// ---------------------------------------------------------------------------

void AppController::run()
{
    while (state_ != AppState::EXITING) {
        switch (state_) {
            case AppState::CAMERA_SELECT: tickCameraSelect(); break;
            case AppState::WELCOME:       tickWelcome();      break;
            case AppState::CALIBRATING:   tickCalibrating();  break;
            case AppState::MEASURING:     tickMeasuring();    break;
            default: break;
        }
    }
}

// ---------------------------------------------------------------------------
// tickCameraSelect
// ---------------------------------------------------------------------------

void AppController::tickCameraSelect()
{
    gui_->renderCameraSelect(availableCameras_);

    UserIntent intent = gui_->pollEvents(AppState::CAMERA_SELECT);
    if (intent == UserIntent::QUIT) {
        transitionTo(AppState::EXITING);
        return;
    }

    int chosen = gui_->getSelectedCamera();
    if (chosen >= 0) {
        config_.cameraIndex = chosen;
        openCamera(chosen);
        transitionTo(AppState::WELCOME);
    }
}

// ---------------------------------------------------------------------------
// tickWelcome
// ---------------------------------------------------------------------------

void AppController::tickWelcome()
{
    cv::Mat frame;
    capture_.read(frame);

    cv::Mat emptyCanny;
    gui_->render(AppState::WELCOME,
                 frame,
                 emptyCanny,
                 {},
                 calState_);

    UserIntent intent = gui_->pollEvents(AppState::WELCOME);
    if (intent == UserIntent::CONFIRM) {
        transitionTo(AppState::CALIBRATING);
    } else if (intent == UserIntent::QUIT) {
        transitionTo(AppState::EXITING);
    }
}

// ---------------------------------------------------------------------------
// tickCalibrating
// ---------------------------------------------------------------------------

void AppController::tickCalibrating()
{
    cv::Mat frame;
    capture_.read(frame);

    if (frame.empty()) {
        gui_->pollEvents(AppState::CALIBRATING);
        return;
    }

    // Keep a copy for calibration (calibrator draws on it)
    lastCalFrame_ = frame.clone();

    PreprocessedFrame proc = preprocessor_->process(frame, thresholds_);

    lastCalResult_ = calibrator_->run(proc.eroded, lastCalFrame_);
    if (lastCalResult_.success) {
        calState_ = lastCalResult_.state;
    }

    gui_->render(AppState::CALIBRATING,
                 lastCalFrame_,
                 proc.canny,
                 {},
                 calState_);

    UserIntent intent = gui_->pollEvents(AppState::CALIBRATING);
    if (intent == UserIntent::CONFIRM && calState_.valid) {
        std::cout << "Calibration accepted: "
                  << calState_.pixelToMM << " mm/px" << std::endl;
        transitionTo(AppState::MEASURING);
    } else if (intent == UserIntent::RETRY) {
        // Reset calibration state and stay in CALIBRATING
        calState_ = CalibrationState{};
    } else if (intent == UserIntent::QUIT) {
        transitionTo(AppState::EXITING);
    }
}

// ---------------------------------------------------------------------------
// tickMeasuring
// ---------------------------------------------------------------------------

void AppController::tickMeasuring()
{
    cv::Mat frame;
    capture_.read(frame);

    if (frame.empty()) {
        gui_->pollEvents(AppState::MEASURING);
        return;
    }

    PreprocessedFrame proc = preprocessor_->process(frame, thresholds_);

    std::vector<DetectedShape> shapes = engine_->detect(proc.eroded, calState_);
    calState_.shapeCount = static_cast<int>(shapes.size());

    // Draw shapes onto the live frame before passing to GuiManager
    cv::Mat display = frame.clone();

    gui_->render(AppState::MEASURING,
                 display,
                 proc.canny,
                 shapes,
                 calState_);

    UserIntent intent = gui_->pollEvents(AppState::MEASURING);
    if (intent == UserIntent::RETRY) {
        // Go back to calibration
        calState_ = CalibrationState{};
        transitionTo(AppState::CALIBRATING);
    } else if (intent == UserIntent::QUIT) {
        transitionTo(AppState::EXITING);
    }
}

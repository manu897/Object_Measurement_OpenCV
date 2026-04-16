#include "GuiManager.hpp"
#include "Overlay.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

const char* GuiManager::kWindowName = "Object Measurement";

GuiManager::GuiManager(const AppConfig& config, ThresholdParams& thresholds)
    : config_(config)
    , thresholds_(thresholds)
    , sidePanel_(config, thresholds)
{
    int W = config_.liveWidth + config_.sidebarWidth;
    int H = config_.liveHeight + config_.statusBarHeight;
    canvas_.create(H, W, CV_8UC3);
    canvas_.setTo(cv::Scalar(20, 20, 20));

    cv::namedWindow(kWindowName, cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback(kWindowName, GuiManager::mouseCallback, this);
}

GuiManager::~GuiManager()
{
    cv::destroyAllWindows();
}

// ---------------------------------------------------------------------------
// mouseCallback (static) — routes to instance method
// ---------------------------------------------------------------------------
void GuiManager::mouseCallback(int event, int x, int y, int /*flags*/, void* userdata)
{
    static_cast<GuiManager*>(userdata)->onMouse(event, x, y);
}

void GuiManager::onMouse(int event, int x, int y)
{
    UserIntent intent = UserIntent::NONE;

    if (lastState_ == AppState::CAMERA_SELECT) {
        cameraSelector_.onMouse(event, x, y, availableCameras_);
        // confirmed camera is checked via getSelectedCamera()
    } else {
        // Clicks in the side panel area
        if (x >= config_.liveWidth) {
            int relX = x - config_.liveWidth;
            intent = sidePanel_.onMouse(event, relX, y, lastState_);
        }
    }

    if (intent != UserIntent::NONE) {
        std::lock_guard<std::mutex> lock(intentMutex_);
        pendingIntent_ = intent;
    }
}

// ---------------------------------------------------------------------------
// render — main composite frame
// ---------------------------------------------------------------------------
void GuiManager::render(AppState                          state,
                         const cv::Mat&                    liveFrame,
                         const cv::Mat&                    cannyFrame,
                         const std::vector<DetectedShape>& shapes,
                         const CalibrationState&           cal)
{
    lastState_ = state;
    canvas_.setTo(cv::Scalar(20, 20, 20));

    // ---- Live feed (left region 640×480) ----
    if (!liveFrame.empty()) {
        cv::Mat resized;
        cv::resize(liveFrame, resized, cv::Size(config_.liveWidth, config_.liveHeight));
        resized.copyTo(canvas_(cv::Rect(0, 0, config_.liveWidth, config_.liveHeight)));
    }

    // ---- Overlay on live feed ----
    cv::Mat liveROI = canvas_(cv::Rect(0, 0, config_.liveWidth, config_.liveHeight));

    // Crosshair during WELCOME and CALIBRATING
    if (state == AppState::WELCOME || state == AppState::CALIBRATING) {
        Overlay::drawCrosshair(liveROI,
                                cv::Point(config_.liveWidth  / 2,
                                          config_.liveHeight / 2),
                                20,
                                cv::Scalar(0, 255, 0), 2);
    }

    // Shape labels and outlines
    if (state == AppState::MEASURING) {
        Overlay::drawShapes(liveROI, shapes, config_);
    }

    // State badge (top-right of live feed)
    {
        std::string  badgeLabel;
        cv::Scalar   badgeBg;
        switch (state) {
            case AppState::WELCOME:
                badgeLabel = "SETUP";
                badgeBg    = cv::Scalar(150, 80, 0);
                break;
            case AppState::CALIBRATING:
                badgeLabel = "CALIBRATING";
                badgeBg    = cv::Scalar(0, 140, 180);
                break;
            case AppState::MEASURING:
                badgeLabel = "MEASURING";
                badgeBg    = cv::Scalar(0, 140, 0);
                break;
            default:
                badgeLabel = "";
                break;
        }
        if (!badgeLabel.empty())
            Overlay::drawStateBadge(liveROI, badgeLabel, badgeBg);
    }

    // ---- Side panel (right region 300×480) ----
    cv::Mat side = sidePanel_.render(state, cannyFrame, cal, shapes);
    side.copyTo(canvas_(cv::Rect(config_.liveWidth, 0,
                                  config_.sidebarWidth, config_.liveHeight)));

    // ---- Status bar (bottom full-width strip) ----
    compositeStatusBar(cal, state);

    cv::imshow(kWindowName, canvas_);
}

void GuiManager::compositeStatusBar(const CalibrationState& cal,
                                     AppState                state) const
{
    int barY = config_.liveHeight;
    cv::Rect barRect(0, barY,
                     config_.liveWidth + config_.sidebarWidth,
                     config_.statusBarHeight);
    cv::rectangle(canvas_, barRect, cv::Scalar(15, 15, 15), cv::FILLED);

    char buf[160] = {};
    if (state == AppState::MEASURING && cal.valid) {
        std::snprintf(buf, sizeof(buf),
                      "Camera %d  |  Cal: %.4f mm/px  |  "
                      "Thresholds  T1:%d  T2:%d  |  "
                      "Click sliders to adjust edges",
                      config_.cameraIndex,
                      cal.pixelToMM,
                      thresholds_.thresh1,
                      thresholds_.thresh2);
    } else if (state == AppState::CALIBRATING) {
        std::snprintf(buf, sizeof(buf),
                      "Camera %d  |  Place 2-pound coin in centre  |  "
                      "Reference diameter: %.1f mm",
                      config_.cameraIndex,
                      config_.coinRealDiamMM);
    } else {
        std::snprintf(buf, sizeof(buf),
                      "Object Measurement  |  Camera %d  |  OpenCV",
                      config_.cameraIndex);
    }

    cv::putText(canvas_, buf,
                cv::Point(10, barY + 26),
                cv::FONT_HERSHEY_SIMPLEX, 0.42,
                cv::Scalar(140, 140, 140), 1);
}

// ---------------------------------------------------------------------------
// renderCameraSelect
// ---------------------------------------------------------------------------
void GuiManager::renderCameraSelect(const std::vector<int>& cameras)
{
    lastState_ = AppState::CAMERA_SELECT;
    availableCameras_ = cameras;
    // Pass current highlighted index to the renderer
    cameraSelector_.render(canvas_, cameras,
                            cameraSelector_.getHighlightedIndex());

    // Re-bind mouse callback with the available list captured in a closure.
    // Since setMouseCallback only accepts a void* userdata, we route through
    // onMouse() which re-reads the camera list from the selector state.
    cv::imshow(kWindowName, canvas_);
}

int GuiManager::getSelectedCamera() const
{
    return cameraSelector_.getSelectedIndex();
}

// ---------------------------------------------------------------------------
// pollEvents — merges keyboard + mouse, returns one intent per call
// ---------------------------------------------------------------------------
UserIntent GuiManager::pollEvents(AppState currentState)
{
    lastState_ = currentState;

    // Check keyboard
    int key = cv::waitKey(30);
    UserIntent kbIntent = UserIntent::NONE;
    if (key == 'y' || key == 'Y') kbIntent = UserIntent::CONFIRM;
    else if (key == 'n' || key == 'N') kbIntent = UserIntent::RETRY;
    else if (key == 'q' || key == 'Q' || key == 'x' || key == 27) kbIntent = UserIntent::QUIT;

    if (kbIntent != UserIntent::NONE) return kbIntent;

    // Check pending mouse intent
    std::lock_guard<std::mutex> lock(intentMutex_);
    UserIntent mouseIntent = pendingIntent_;
    pendingIntent_         = UserIntent::NONE;
    return mouseIntent;
}

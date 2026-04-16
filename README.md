# Object Measurement Project

A real-time object measurement application built with OpenCV. It uses a webcam feed to detect and measure physical objects (circles, squares, and rectangles) in centimetres, calibrated against a **2-pound coin (28.4 mm diameter)**.

## Project Structure

```
object-measurement-setup/
├── CMakeLists.txt
├── scripts/
│   ├── install_dependencies.sh      # Installs required dependencies (macOS/Linux/Windows)
│   └── setup_project.sh             # Configures and builds the project via CMake
└── src/
    ├── main.cpp                     # Application entry point
    ├── calibration/
    │   ├── Calibrator.cpp           # Coin-based pixel-to-mm calibration
    │   └── Calibrator.hpp
    ├── core/
    │   ├── AppConfig.hpp            # All tunable constants (single source of truth)
    │   ├── AppController.cpp        # State machine / main orchestrator
    │   ├── AppController.hpp
    │   ├── Preprocessor.cpp         # Stateless image pipeline (Gray → Blur → Canny → Dilate → Erode)
    │   └── Preprocessor.hpp
    ├── gui/
    │   ├── CameraSelector.cpp       # Startup camera picker (probes indices 0–5)
    │   ├── CameraSelector.hpp
    │   ├── GuiManager.cpp           # Composite window and input routing
    │   ├── GuiManager.hpp
    │   ├── Overlay.cpp              # Stateless drawing helpers (crosshair, shapes, badges)
    │   ├── Overlay.hpp
    │   ├── SidePanel.cpp            # Right-column UI (sliders, buttons, stats)
    │   └── SidePanel.hpp
    └── measurement/
        ├── DetectedShape.hpp        # Shape data structure
        ├── MeasurementEngine.cpp    # Contour filtering and detector dispatch
        ├── MeasurementEngine.hpp
        ├── ShapeDetector.hpp        # Abstract detector interface
        └── detectors/
            ├── CircleDetector.cpp
            ├── CircleDetector.hpp
            ├── RectangleDetector.cpp
            ├── RectangleDetector.hpp
            ├── SquareDetector.cpp
            └── SquareDetector.hpp
```

## Architecture

The application follows a layered architecture with a state-machine at its core:

```
main.cpp
  └─ AppController            ← 5-state FSM orchestrator
       ├─ Preprocessor        ← Stateless image pipeline
       ├─ Calibrator          ← Coin-based px → mm calibration
       ├─ MeasurementEngine   ← Contour dispatch to shape detectors
       │    ├─ CircleDetector
       │    ├─ SquareDetector
       │    └─ RectangleDetector
       └─ GuiManager          ← Composite window + input routing
            ├─ SidePanel
            ├─ CameraSelector
            └─ Overlay
```

### Application States

| State | Description |
|---|---|
| `CAMERA_SELECT` | Startup screen — user picks from detected cameras |
| `WELCOME` | Live feed shown — user prompted to place coin and confirm |
| `CALIBRATING` | Detects coin, computes pixel-to-mm ratio — user accepts or retries |
| `MEASURING` | Continuous shape detection and measurement overlay on live feed |
| `EXITING` | Clean shutdown |

### Canvas Layout

```
┌──────────────────────────┬───────────────┐
│                          │  State Badge  │
│   Live Camera Feed       │  Canny Preview│
│       640 × 480          │  T1/T2 Sliders│
│  (shapes drawn here)     │  Buttons      │
│                          │  Stats        │
│                          │               │
├──────────────────────────┴───────────────┤
│   Status bar (full width, 40px)          │
└──────────────────────────────────────────┘
              Total: 940 × 520
```

## Requirements

- **OpenCV** (core, imgproc, highgui, videoio)
- **CMake** ≥ 3.10
- **C++14** compatible compiler

## Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/manu897/Object_Measurement_OpenCV.git
   cd Object_Measurement_OpenCV/object-measurement-setup
   ```

2. **Install dependencies**:
   The script auto-detects your OS (macOS via Homebrew, Linux via apt, Windows via Chocolatey).
   ```bash
   chmod +x scripts/install_dependencies.sh
   ./scripts/install_dependencies.sh
   ```

3. **Build the project**:
   ```bash
   chmod +x scripts/setup_project.sh
   ./scripts/setup_project.sh
   ```
   Or manually:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

## Running the Application

```bash
./build/ObjectMeasurement
```

## Usage

1. **Select a camera** from the startup screen.
2. **Place a 2-pound coin** (28.4 mm) in the centre of the camera view.
3. **Press Y or click** to calibrate — the system computes the pixel-to-mm ratio from the coin.
4. **Accept or retry** the calibration result.
5. Once calibrated, objects are **detected and measured in real time** with dimensions displayed as an overlay.
6. Use the **side panel sliders** to adjust Canny edge-detection thresholds live.
7. Press **Q or Esc** to exit.

## Shape Detection

- **Circle**: Contours with > 7 vertices after polygon approximation → `minEnclosingCircle` → diameter in cm
- **Square**: 4-vertex contours with aspect ratio in [0.95, 1.05] → side length in cm
- **Rectangle**: 4-vertex contours with aspect ratio outside square bounds → width and height in cm

New shapes can be added by implementing the `ShapeDetector` interface and registering the detector with the `MeasurementEngine`.

## Contributing

Feel free to contribute to this project by submitting issues or pull requests. Your contributions are welcome!
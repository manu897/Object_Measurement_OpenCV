project(ObjectMeasurement)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

# ---------------------------------------------------------------------------
# Sources — add new shape detectors here
# ---------------------------------------------------------------------------
set(SOURCES
    src/main.cpp

    # Core
    src/core/AppController.cpp
    src/core/Preprocessor.cpp

    # Calibration
    src/calibration/Calibrator.cpp

    # Measurement engine + shape detectors
    src/measurement/MeasurementEngine.cpp
    src/measurement/detectors/CircleDetector.cpp
    src/measurement/detectors/SquareDetector.cpp
    src/measurement/detectors/RectangleDetector.cpp

    # GUI
    src/gui/GuiManager.cpp
    src/gui/SidePanel.cpp
    src/gui/CameraSelector.cpp
    src/gui/Overlay.cpp
)

add_executable(ObjectMeasurement ${SOURCES})

# Allow #include "core/AppConfig.hpp" style includes from all source files
target_include_directories(ObjectMeasurement PRIVATE src)

target_include_directories(ObjectMeasurement PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(ObjectMeasurement ${OpenCV_LIBS})

# Compiler warnings
if(MSVC)
    target_compile_options(ObjectMeasurement PRIVATE /W3)
else()
    target_compile_options(ObjectMeasurement PRIVATE -Wall -Wextra)
endif()

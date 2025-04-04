# Object Measurement with OpenCV

This project demonstrates how to measure objects using OpenCV with a C++ implementation. It uses a reference object for calibration to determine the size of other objects in an image.

## Features

- Measure objects in an image using pixel-to-real-world scaling.
- Supports calibration with a reference object (e.g., a 2-pound coin).
- Adjustable resolution and calibration values for different setups.

## Requirements

- OpenCV library installed on your system.
- A reference object for calibration (default: 2-pound coin).
- A camera or image source with appropriate resolution settings.

## Usage

1. Clone the repository and navigate to the project directory:
    ```bash
    git clone https://github.com/manu897/Object_Measurement_OpenCV.git
    cd Object_Measurement_OpenCV
    ```

2. Build the project using your preferred C++ compiler with OpenCV:
    ```bash
    g++ -o object_measurement main.cpp `pkg-config --cflags --libs opencv4`
    ```

3. Run the program:
    ```bash
    ./object_measurement
    ```

4. Follow the on-screen instructions to calibrate and measure objects.

## Notes

- Ensure the reference object is clearly visible in the image for accurate calibration.
- Adjust the calibration values in the code if using a different reference object.
- Contributions and suggestions for improvements are welcome!

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

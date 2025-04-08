# Object Measurement Project

This project implements an object measurement application using OpenCV. It includes functionalities for preprocessing images, calibrating the system, and measuring dimensions of detected objects.

## Project Structure

```
object-measurement-setup
├── src
│   └── Object_Measurement.cpp      # Main implementation of the object measurement application
├── scripts
│   ├── install_dependencies.sh      # Script to install required dependencies
│   └── setup_project.sh             # Script to set up the project environment
├── CMakeLists.txt                   # CMake configuration file
└── README.md                        # Project documentation
```

## Installation

To set up the project, follow these steps:

1. **Clone the repository**:
   ```
   git clone https://github.com/manu897/Object_Measurement_OpenCV.git
   cd object-measurement-setup
   ```

2. **Run the installation script**:
   This script will install all necessary dependencies, including OpenCV.
   ```
   chmod +x scripts/install_dependencies.sh
   ./scripts/install_dependencies.sh
   ```

3. **Set up the project**:
   This script will create the necessary build directories and configure the project using CMake.
   ```
   chmod +x scripts/setup_project.sh
   ./scripts/setup_project.sh
   ```

## Building the Project

After setting up the project, you can build it using the following command:
```
mkdir build
cd build
cmake ..
make
```

## Running the Application

Once the project is built, you can run the object measurement application:
```
./Object_Measurement
```

## Usage

Follow the on-screen instructions to calibrate the system and measure objects. Make sure to place the reference object (e.g., a coin) in the center of the camera view for accurate calibration.

## Contributing

Feel free to contribute to this project by submitting issues or pull requests. Your contributions are welcome!
#!/bin/bash

# Detect the operating system
OS="$(uname -s)"

if [[ "$OS" == "Linux" ]]; then
    echo "Detected Linux. Installing dependencies..."
    sudo apt update
    sudo apt install -y cmake git python3 python3-pip
    pip3 install opencv-python opencv-contrib-python
    echo "Dependencies installed successfully on Linux."

elif [[ "$OS" == "Darwin" ]]; then
    echo "Detected macOS. Installing dependencies..."
    # Check if Homebrew is installed
    if ! command -v brew &>/dev/null; then
        echo "Homebrew not found. Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi
    brew install cmake git python
    pip3 install opencv-python opencv-contrib-python
    echo "Dependencies installed successfully on macOS."

elif [[ "$OS" == *"MINGW"* || "$OS" == *"CYGWIN"* ]]; then
    echo "Detected Windows. Installing dependencies..."
    # Check if Chocolatey is installed
    if ! command -v choco &>/dev/null; then
        echo "Chocolatey not found. Installing Chocolatey..."
        powershell -NoProfile -ExecutionPolicy Bypass -Command \
            "Set-ExecutionPolicy Bypass -Scope Process -Force; \
            [System.Net.ServicePointManager]::SecurityProtocol = \
            [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; \
            Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))"
    fi
    choco install -y cmake git python
    pip install opencv-python opencv-contrib-python
    echo "Dependencies installed successfully on Windows."

else
    echo "Unsupported operating system: $OS"
    exit 1
fi

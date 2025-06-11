#!/bin/bash
arduino-cli compile --clean --fqbn esp32:esp32:esp32c3 . 
if [ $? -ne 0 ]; then
    echo "Compilation failed. Exiting."
    exit 1
fi
# detect the port automatically
PORT=$(ls /dev/cu.wchusbserial* 2>/dev/null | head -n 1)
if [ -z "$PORT" ]; then
    echo "No device found. Please connect your ESP32-C3 and try again."
    exit 1
fi
arduino-cli upload -p $PORT --fqbn esp32:esp32:esp32c3 .
if [ $? -ne 0 ]; then
    echo "Upload failed. Please check your device connection and try again."
    exit 1
fi
echo "Upload successful!"
# Open the serial monitor
arduino-cli monitor -p $PORT --fqbn esp32:esp32:esp32c3 --config 115200
if [ $? -ne 0 ]; then
    echo "Failed to open serial monitor. Please check your device connection."
    exit 1
fi

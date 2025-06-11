# Hardware features of ESP32-C3 board.

## Overview
This is a compact development board featuring an ESP32-C3 single-core RISC-V processor. It is designed for portable applications, integrating a color LCD, a 6-axis motion sensor (IMU), a TF/SD card slot, and a complete battery charging and management circuit. All major peripherals are mapped to specific GPIO pins which you can control with your code.

## Programming and Debugging
The board is programmed via the USB Type-C port. This port is connected to a CH343P USB-to-Serial chip , which handles communication with the ESP32-C3's primary UART (U0TXD/U0RXD).

Serial TX (from ESP32): GPIO21 (U0TXD) 
Serial RX (to ESP32): GPIO20 (U0RXD) 
The CH343P chip also manages the ESP_EN and IO9 pins, allowing for automatic reset and bootloader mode. This means you can flash new firmware using standard tools like the Arduino IDE or ESP-IDF without needing to manually press any buttons.

## Peripheral GPIO Mapping
### LCD Display (SPI Interface)
The board includes a 240x240 color LCD connected via an SPI interface. Note that the SPI Clock and Data Output pins are shared with the SD Card slot.

SCL (Clock): GPIO9 (connected to ESP32's SPICLK pin) 
SDA (MOSI/Data Out): GPIO10 (connected to ESP32's SPID pin) 
CS (Chip Select): GPIO1 (connected to ESP32's SPICS0 pin) 
DC (Data/Command): GPIO2 
RES (Reset): GPIO11 

### QMI8658 Inertial Measurement Unit (I2C Interface)
A 6-axis QMI8658 (Gyroscope + Accelerometer) is available on an I2C bus.

SCL (Clock): GPIO6 
SDA (Data): GPIO5 

### TF/MicroSD Card Slot (SPI Interface)
The TF card slot allows for expandable storage and communicates over the same SPI bus as the display. It uses a different Chip Select pin to avoid conflicts.

CLK (Clock): GPIO9 (Shared with LCD) 

DI (MOSI/Data In): GPIO10 (Shared with LCD) 

DO (MISO/Data Out): GPIO4 

CS (Chip Select): GPIO18 

### Onboard LED
The ESP32-C3 chip itself contains a user-programmable RGB LED connected to GPIO8. You can control its color and brightness directly from your code.

## Power Management
A programmer can monitor the battery level through one of the ADC (Analog-to-Digital Converter) pins.

Battery Voltage Sensing: GPIO0  is connected to the battery's VBAT line through a voltage divider (R11=100K, R12=47K). To get the actual battery voltage, you must read the analog value from GPIO0 and account for this divider in your code.

## GPIO Summary Table for Programmers

### Programming/UART
| Function      | ESP32-C3 GPIO | Interface | Notes                |
|---------------|--------------|-----------|----------------------|
| UART TX      | GPIO21        | UART      | For serial monitor output. |
| UART RX      | GPIO20        | UART      | For serial input. |

### LCD Display
| Function      | ESP32-C3 GPIO | Interface | Notes                |
|---------------|--------------|-----------|----------------------|
| LCD SCL (Clock) | GPIO4        | SPI       |  |
| LCD SDA (MOSI)  | GPIO7       | SPI       |   |
| LCD CS          | GPIO2        | SPI       |                      |
| LCD DC          | GPIO3        | GPIO      |                      |
| LCD Reset       | GPIO8       | GPIO      |                      |

### QMI8658 IMU			
| Function   | ESP32-C3 GPIO | Interface | Notes |
|------------|---------------|-----------|-------|
| IMU SCL    | GPIO6         | I2C       |       |
| IMU SDA    | GPIO5         | I2C       |       |

### Onboard LED
| Function   | ESP32-C3 GPIO | Interface | Notes                                 |
|------------|---------------|-----------|---------------------------------------|
| RGB LED    | GPIO8         | GPIO      | Integrated into the ESP32-C3 module.  |

### Power
| Function      | ESP32-C3 GPIO | Interface | Notes                                         |
|---------------|--------------|-----------|-----------------------------------------------|
| Battery ADC   | GPIO0        | ADC       | Reads battery voltage via a voltage divider.  |


# Additional Notes
 Use arduino-cli to compile and upload code to the ESP32-C3 board.
 
# font DSEG7_Classic_Mini
it is recommended to use the LVGL font converter to generate the font data from a TTF file. The DSEG7_Classic_Mini font is a 7-segment style font suitable for digital clocks and displays. 
It is under the SIL Open Font License, Version 1.1, which allows for both personal and commercial use with some conditions.
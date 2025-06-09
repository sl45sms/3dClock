/*
Required libraries:
- Adafruit GFX Library v1.12.1
- Adafruit ST7735 and ST7789 Library v1.11.0
*/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <WiFi.h>
#include "time.h"

// Pin definitions
#define TFT_CS   2    // Chip select pin
#define TFT_DC   3    // Data/command pin
#define TFT_RST  8    // Reset pin
#define TFT_SCLK 4    // SPI clock pin (SCL)
#define TFT_MOSI 7    // SPI data pin (SDA/MOSI)

// Create display instance
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// WiFi and NTP configuration
const char* ssid       = "TheDevicesNetwork";
const char* password   = "AaBbCcDd..123!";
const char* ntpServer  = "pool.ntp.org";
const long  gmtOffset  = 0;
const int   daylightOffset = 3600;

void setup() {
  Serial.begin(115200);
  delay(1000); // Allow time for Serial to initialize
  
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  
  // Initialize NTP for Europe/Athens (EET/EEST)
  configTzTime("EET-2EEST,M3.5.0/02:00:00,M10.5.0/03:00:00", ntpServer);

  // Initialize SPI with custom pins: SCLK, MISO (not used), MOSI, SS
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

  // Initialize display with proper timing and reset sequence
  tft.init(240, 240, SPI_MODE3); // Specify SPI_MODE3
  tft.setSPISpeed(20000000UL); // Set SPI frequency to 20MHz
  delay(150); // Extended delay for display stabilization
  
  tft.setRotation(0);
  
  // Mirror horizontally (flip X axis) via MADCTL, preserving RGB color order
  uint8_t madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_RGB; // Combine MX with RGB
  tft.sendCommand(ST77XX_MADCTL, &madctl, 1);
  delay(10); // Delay from original code
  
  tft.fillScreen(ST77XX_BLACK);
  
  Serial.println("Display initialized");
  delay(1000); // Allow time for display to stabilize
}

void loop() {
  struct tm timeInfo;
  if (!getLocalTime(&timeInfo)) {
    Serial.println("Failed to obtain time");
    delay(1000);
    return;
  }
  
  // Prepare current time string
  char timeStr[9];
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeInfo);
    
  // Draw current time
  tft.setTextSize(4);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setCursor(10, 40);
  tft.print(timeStr);
  
  delay(1000);
}

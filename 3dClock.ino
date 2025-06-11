/*
Minimal LVGL v9 example for ESP32 and ST7789 display
Required libraries:
- Adafruit GFX Library
- Adafruit ST7735 and ST7789 Library
- LVGL Library (v9.x)
*/
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <lvgl.h>

// WiFi and NTP includes
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// WiFi credentials (replace with your actual credentials)
const char* ssid     = "TheDevicesNetwork";
const char* password = "AaBbCcDd..123!";

// NTP client setup
WiFiUDP ntpUDP;
// Update time every minute (60000 ms), offset for GMT+0, adjust as needed
#define NTP_OFFSET 7200 // Offset in seconds for Athens (GMT+2, adjust as needed)
NTPClient timeClient(ntpUDP, "pool.ntp.org", NTP_OFFSET, 60000);


// Pin definitions (kept from your original script)
#define TFT_CS   2    // Chip select pin
#define TFT_DC   3    // Data/command pin
#define TFT_RST  8    // Reset pin
#define TFT_SCLK 4    // SPI clock pin (SCL)
#define TFT_MOSI 7    // SPI data pin (SDA/MOSI)

// Display dimensions
#define TFT_WIDTH  240
#define TFT_HEIGHT 240

// Create display instance
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// LVGL display buffer
// A buffer for 1/10th of the screen height is a common size
static lv_color_t buf1[TFT_WIDTH * (TFT_HEIGHT / 10)];
static lv_draw_buf_t draw_buf;

// LVGL display flush callback
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);

    //Serial.printf("Flush: area(x1=%d,y1=%d,x2=%d,y2=%d), w=%lu,h=%lu\n",
    //              area->x1, area->y1, area->x2, area->y2, w, h);

    tft.drawRGBBitmap(area->x1, area->y1, (uint16_t*)px_map, w, h);
    lv_display_flush_ready(disp);
}

void setup() {
    Serial.begin(115200);
    delay(2000); // Wait for serial monitor
    Serial.println("\n=== LVGL v9 ST7789 Clock Test ===\n");

    // Initialize WiFi
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Initialize NTP client
    timeClient.begin();
    Serial.println("NTP client started");

    // Initialize TFT display
    SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
    tft.init(TFT_WIDTH, TFT_HEIGHT);
    tft.setSPISpeed(40000000UL); // Max SPI speed for ST7789 can be higher
    tft.setRotation(0);
    tft.fillScreen(ST77XX_BLACK);
    Serial.println("TFT display initialized");

    // Initialize LVGL
    lv_init();
    Serial.println("LVGL initialized");

    // Initialize LVGL display driver
    // For LVGL v9, the stride is in bytes, not pixels.
    // For LV_COLOR_FORMAT_NATIVE with 16-bit color, bytes_per_pixel is 2.
    uint32_t stride_bytes = TFT_WIDTH * lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);
    lv_draw_buf_init(&draw_buf, TFT_WIDTH, (TFT_HEIGHT / 10), LV_COLOR_FORMAT_NATIVE, stride_bytes, buf1, sizeof(buf1));
    
    lv_display_t *disp = lv_display_create(TFT_WIDTH, TFT_HEIGHT);
    if (disp == NULL) {
        Serial.println("ERROR: lv_display_create failed!");
        while(1);
    }
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_draw_buffers(disp, &draw_buf, NULL); // Single buffer

    Serial.println("LVGL display driver initialized");

    // --- Create a simple LVGL UI ---

    // Set screen background to a color
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x003a57), LV_PART_MAIN | LV_STATE_DEFAULT); // Dark blue background

    // Create a label for time display
    lv_obj_t *label = lv_label_create(scr);
    // lv_label_set_text(label, "Hello LVGL!"); // Will be replaced by time
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT); // White text
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT); // Changed to 14pt font
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0); // Align to center

    Serial.println("LVGL UI created. Waiting for loop...");
}

void loop() {
    static uint32_t last_tick = 0;
    static uint32_t last_time_update = 0;
    uint32_t current_millis = millis();
    
    // Calculate elapsed time for lv_tick_inc
    uint32_t elapsed_time = current_millis - last_tick;
    if (elapsed_time == 0) { // Ensure at least 1ms passes for tick
        elapsed_time = 1; 
    }
    lv_tick_inc(elapsed_time);
    last_tick = current_millis;
    
    lv_timer_handler();

    // Update time every second
    if (current_millis - last_time_update >= 1000) {
        last_time_update = current_millis;
        if (WiFi.status() == WL_CONNECTED) {
            timeClient.update();
            String formattedTime = timeClient.getFormattedTime();
            
            // Example: Add day of the week (optional)
            // int dayOfWeek = timeClient.getDay(); // 0 = Sunday, 1 = Monday, ...
            // String dayStr;
            // switch(dayOfWeek) {
            //    case 0: dayStr = "Sun"; break;
            //    case 1: dayStr = "Mon"; break;
            //    case 2: dayStr = "Tue"; break;
            //    case 3: dayStr = "Wed"; break;
            //    case 4: dayStr = "Thu"; break;
            //    case 5: dayStr = "Fri"; break;
            //    case 6: dayStr = "Sat"; break;
            //    default: dayStr = "";
            // }
            // String displayStr = dayStr + " " + formattedTime;

            lv_obj_t *time_label = lv_obj_get_child(lv_screen_active(), 0); // Assuming label is the first child
            if (time_label) {
                 lv_label_set_text(time_label, formattedTime.c_str());
                 // lv_label_set_text(time_label, displayStr.c_str()); // If using day + time
            }
        } else {
            lv_obj_t *time_label = lv_obj_get_child(lv_screen_active(), 0);
            if (time_label) {
                 lv_label_set_text(time_label, "WiFi...");
            }
        }
    }

    delay(5); // Call LVGL tasks every 5ms
}

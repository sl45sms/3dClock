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
#include "DSEG7_Classic_Mini.c"

// WiFi and NTP includes
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// WiFi credentials (replace with your actual credentials)
const char* ssid     = "YOUR_SSID"; // Replace with your WiFi SSID
const char* password = "YOUR_PASSWORD"; // Replace with your WiFi password

// NTP client setup
WiFiUDP ntpUDP;

// Offset in seconds for Athens (GMT+2, adjust as needed)
#define NTP_OFFSET 7200 // 2 hours offset for Athens (GMT+2) 
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
    //Serial.println("\n=== LVGL v9 ST7789 Clock ===\n");

    // Initialize WiFi
    //Serial.print("Connecting to WiFi: ");
    //Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        //Serial.print(".");
    }
    //Serial.println("\nWiFi connected!");
    //Serial.print("IP address: ");
    //Serial.println(WiFi.localIP());

    // Initialize NTP client
    timeClient.begin();
    //Serial.println("NTP client started");

    // Initialize TFT display
    SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
    tft.init(TFT_WIDTH, TFT_HEIGHT);
    tft.setSPISpeed(40000000UL); // Max SPI speed for ST7789 can be higher

    // Mirror the screen horizontally only
    tft.setRotation(2);
    // For Adafruit_ST7789, horizontal mirroring is not directly supported by setRotation.
    // Mirror horizontally (flip X axis) via MADCTL, preserving RGB color order
    uint8_t madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_RGB; // Mirror X axis
    tft.sendCommand(ST77XX_MADCTL, &madctl, 1);
    //Serial.println("TFT display initialized");

    // Initialize LVGL
    lv_init();
    //Serial.println("LVGL initialized");

    // Initialize LVGL display driver
    // For LVGL v9, the stride is in bytes, not pixels.
    // For LV_COLOR_FORMAT_NATIVE with 16-bit color, bytes_per_pixel = 2.
    uint32_t stride_bytes = TFT_WIDTH * lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);
    lv_draw_buf_init(&draw_buf, TFT_WIDTH, (TFT_HEIGHT / 10), LV_COLOR_FORMAT_NATIVE, stride_bytes, buf1, sizeof(buf1));
    
    lv_display_t *disp = lv_display_create(TFT_WIDTH, TFT_HEIGHT);
    if (disp == NULL) {
        while(1);
    }
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_draw_buffers(disp, &draw_buf, NULL); // Single buffer

    //Serial.println("LVGL display driver initialized");

    // --- Create a simple LVGL UI ---

    // Set screen background to black using LVGL v9 API
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT); // Ensure background is fully opaque
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT); // Black background

    // --- Analog clock (LVGL v9, using objects for hands/face) ---
    // Create a container for the analog clock
    lv_obj_t *analog_cont = lv_obj_create(scr);
    lv_obj_set_size(analog_cont, 120, 120); // width, height
    lv_obj_set_style_bg_opa(analog_cont, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(analog_cont, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(analog_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(analog_cont, LV_ALIGN_TOP_MID, 0, 30); // Top center, 30px from top (increased from 10)

    // Create a label for time display (digital)
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "00:00:00"); // Initial text
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT); // White text
    lv_obj_set_style_text_font(label, &DSEG7_Classic_Mini, LV_PART_MAIN | LV_STATE_DEFAULT); // Use digital font
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 170); // Move label further down below analog clock

    // Store analog_cont in user data for use in loop
    lv_obj_set_user_data(analog_cont, NULL); // Not used, but placeholder for future

    // Force LVGL to redraw the whole screen
    lv_obj_invalidate(scr);

    //Serial.println("LVGL UI created. Waiting for loop...");
}

// --- DST calculation for Athens, Greece ---
// Returns offset in seconds: 7200 (GMT+2) for winter, 10800 (GMT+3) for summer
int getAthensOffset(time_t now) {
    struct tm *tm_info = gmtime(&now);
    int year = tm_info->tm_year + 1900;
    // Calculate last Sunday of March (DST start)
    struct tm startDST = {0};
    startDST.tm_year = year - 1900;
    startDST.tm_mon = 2; // March
    startDST.tm_mday = 31;
    startDST.tm_hour = 1; // 04:00 local, 01:00 UTC
    mktime(&startDST);
    while (startDST.tm_wday != 0) { // 0 = Sunday
        startDST.tm_mday--;
        mktime(&startDST);
    }
    time_t dstStart = mktime(&startDST);
    // Calculate last Sunday of October (DST end)
    struct tm endDST = {0};
    endDST.tm_year = year - 1900;
    endDST.tm_mon = 9; // October
    endDST.tm_mday = 31;
    endDST.tm_hour = 1; // 04:00 local, 01:00 UTC
    mktime(&endDST);
    while (endDST.tm_wday != 0) {
        endDST.tm_mday--;
        mktime(&endDST);
    }
    time_t dstEnd = mktime(&endDST);
    if (now >= dstStart && now < dstEnd) {
        return 10800; // GMT+3 (DST)
    } else {
        return 7200; // GMT+2 (Standard)
    }
}

// --- Analog clock drawing helper using LVGL objects ---
#include <math.h>
void free_line_points(lv_event_t *e) {
    lv_obj_t *line = (lv_obj_t *)lv_event_get_target(e);
    void *points = lv_obj_get_user_data(line);
    if(points) lv_free(points);
}

void draw_analog_clock(lv_obj_t *cont, int hour, int min, int sec) {
    // Remove previous hands/marks and free their points
    while (lv_obj_get_child_cnt(cont) > 0) {
        lv_obj_t *child = lv_obj_get_child(cont, 0);
        lv_obj_del(child); // Only delete, let LV_EVENT_DELETE free the points
    }
    int cx = 60, cy = 60, r = 55;
    // Draw hour marks
    for (int i = 0; i < 12; i++) {
        float angle = (i * 30 - 90) * 3.14159f / 180.0f;
        int x1 = cx + (int)(cosf(angle) * (r - 10));
        int y1 = cy + (int)(sinf(angle) * (r - 10));
        int x2 = cx + (int)(cosf(angle) * (r - 2));
        int y2 = cy + (int)(sinf(angle) * (r - 2));
        lv_obj_t *mark = lv_line_create(cont);
        lv_point_precise_t *pts = (lv_point_precise_t*)lv_malloc(sizeof(lv_point_precise_t) * 2);
        pts[0].x = x1; pts[0].y = y1;
        pts[1].x = x2; pts[1].y = y2;
        lv_line_set_points_mutable(mark, pts, 2);
        lv_obj_set_user_data(mark, pts);
        lv_obj_add_event_cb(mark, free_line_points, LV_EVENT_DELETE, NULL);
        lv_obj_set_style_line_color(mark, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_line_width(mark, 2, LV_PART_MAIN);
    }
    // Draw hour hand
    float hour_angle = ((hour % 12) + min / 60.0f) * 30.0f - 90.0f;
    hour_angle = hour_angle * 3.14159f / 180.0f;
    int hx = cx + (int)(cosf(hour_angle) * (r - 30));
    int hy = cy + (int)(sinf(hour_angle) * (r - 30));
    lv_obj_t *hour_hand = lv_line_create(cont);
    lv_point_precise_t *hour_pts = (lv_point_precise_t*)lv_malloc(sizeof(lv_point_precise_t) * 2);
    hour_pts[0].x = cx; hour_pts[0].y = cy;
    hour_pts[1].x = hx; hour_pts[1].y = hy;
    lv_line_set_points_mutable(hour_hand, hour_pts, 2);
    lv_obj_set_user_data(hour_hand, hour_pts);
    lv_obj_add_event_cb(hour_hand, free_line_points, LV_EVENT_DELETE, NULL);
    lv_obj_set_style_line_color(hour_hand, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_line_width(hour_hand, 5, LV_PART_MAIN);
    // Draw minute hand
    float min_angle = (min * 6.0f - 90.0f) * 3.14159f / 180.0f;
    int mx = cx + (int)(cosf(min_angle) * (r - 18));
    int my = cy + (int)(sinf(min_angle) * (r - 18));
    lv_obj_t *min_hand = lv_line_create(cont);
    lv_point_precise_t *min_pts = (lv_point_precise_t*)lv_malloc(sizeof(lv_point_precise_t) * 2);
    min_pts[0].x = cx; min_pts[0].y = cy;
    min_pts[1].x = mx; min_pts[1].y = my;
    lv_line_set_points_mutable(min_hand, min_pts, 2);
    lv_obj_set_user_data(min_hand, min_pts);
    lv_obj_add_event_cb(min_hand, free_line_points, LV_EVENT_DELETE, NULL);
    lv_obj_set_style_line_color(min_hand, lv_color_hex(0x00FF00), LV_PART_MAIN);
    lv_obj_set_style_line_width(min_hand, 3, LV_PART_MAIN);
    // Draw second hand
    float sec_angle = (sec * 6.0f - 90.0f) * 3.14159f / 180.0f;
    int sx = cx + (int)(cosf(sec_angle) * (r - 10));
    int sy = cy + (int)(sinf(sec_angle) * (r - 10));
    lv_obj_t *sec_hand = lv_line_create(cont);
    lv_point_precise_t *sec_pts = (lv_point_precise_t*)lv_malloc(sizeof(lv_point_precise_t) * 2);
    sec_pts[0].x = cx; sec_pts[0].y = cy;
    sec_pts[1].x = sx; sec_pts[1].y = sy;
    lv_line_set_points_mutable(sec_hand, sec_pts, 2);
    lv_obj_set_user_data(sec_hand, sec_pts);
    lv_obj_add_event_cb(sec_hand, free_line_points, LV_EVENT_DELETE, NULL);
    lv_obj_set_style_line_color(sec_hand, lv_color_hex(0xFF0000), LV_PART_MAIN);
    lv_obj_set_style_line_width(sec_hand, 1, LV_PART_MAIN);
    // Draw center dot (use a small circle)
    lv_obj_t *center = lv_obj_create(cont);
    lv_obj_set_size(center, 8, 8);
    lv_obj_set_style_radius(center, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(center, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(center, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_align(center, LV_ALIGN_CENTER, 0, 0);
}

void loop() {
    static uint32_t last_tick = 0;
    static uint32_t last_time_update = 0;
    static int lastOffset = -1; // Track last set offset
    uint32_t current_millis = millis();
    //Serial.println(current_millis); // Debug: show loop frequency
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
        int hour = 0, min = 0, sec = 0;
        //Serial.println("Updating time...");
        //Serial.println("Before timeClient.update()");
        if (WiFi.status() == WL_CONNECTED) {
            timeClient.update();
            //Serial.println("After timeClient.update()");
            // --- Automatic DST offset update ---
            //Serial.println("Before getAthensOffset");
            time_t rawTime = timeClient.getEpochTime();
            int correctOffset = getAthensOffset(rawTime);
            //Serial.println("After getAthensOffset");
            if (lastOffset != correctOffset) {
                timeClient.setTimeOffset(correctOffset);
                lastOffset = correctOffset;
            }
            String formattedTime = timeClient.getFormattedTime();
            sscanf(formattedTime.c_str(), "%2d:%2d:%2d", &hour, &min, &sec);
            lv_obj_t *time_label = lv_obj_get_child(lv_screen_active(), 1); // label is now 2nd child
            if (time_label) {
                 lv_label_set_text(time_label, formattedTime.c_str());
                 lv_obj_invalidate(time_label);
            }
        } else {
            lv_obj_t *time_label = lv_obj_get_child(lv_screen_active(), 1);
            if (time_label) {
                 lv_label_set_text(time_label, "WiFi...");
                 lv_obj_invalidate(time_label);
            }
        }
        // Draw analog clock
        //Serial.println("Before draw_analog_clock");
        lv_obj_t *analog_cont = lv_obj_get_child(lv_screen_active(), 0); // analog_cont
        if (analog_cont) {
            draw_analog_clock(analog_cont, hour, min, sec);
            lv_obj_invalidate(analog_cont);
        }
        //Serial.println("After draw_analog_clock");
    }
    delay(1); // Call LVGL tasks as often as possible
}

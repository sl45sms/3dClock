/*
 * lv_conf.h - Minimal configuration for LVGL v9.x for ESP32 project
 *
 * You can customize this file as needed for your project.
 * For full options, see the LVGL documentation: https://docs.lvgl.io/latest/en/html/porting/project.html
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_USE_OS 0
#define LV_USE_LOG 0
#define LV_USE_ASSERT_NULL 1
#define LV_USE_ASSERT_MALLOC 1
#define LV_USE_ASSERT_STYLE 1
#define LV_USE_ASSERT_MEM_INTEGRITY 1
#define LV_USE_ASSERT_OBJ 1

#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 0
#define LV_COLOR_SCREEN_TRANSP 0
#define LV_USE_DRAW_SW 1

#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_DEFAULT &lv_font_montserrat_14

#define LV_USE_DISP_ROT_MAX 0
#define LV_USE_GPU 0
#define LV_USE_PERF_MONITOR 0
#define LV_USE_MEM_MONITOR 0
#define LV_USE_LOG 0

#define LV_USE_LABEL 1
#define LV_USE_BTN 0
#define LV_USE_IMG 0
#define LV_USE_LINE 1
#define LV_USE_ARC 0
#define LV_USE_BAR 0
#define LV_USE_SLIDER 0
#define LV_USE_SWITCH 0
#define LV_USE_LED 0
#define LV_USE_MSGBOX 0
#define LV_USE_TEXTAREA 1
#define LV_USE_CANVAS 1
#define LV_USE_TABLE 0
#define LV_USE_TABVIEW 0
#define LV_USE_TILEVIEW 0
#define LV_USE_WIN 0
#define LV_USE_SPINBOX 0
#define LV_USE_SPINNER 0
#define LV_USE_DROPDOWN 0
#define LV_USE_ROLLER 0
#define LV_USE_CHART 0
#define LV_USE_CALENDAR 0
#define LV_USE_CPICKER 0
#define LV_USE_LIST 0
#define LV_USE_METER 0
#define LV_USE_MSGBOX 0
#define LV_USE_SPINBOX 0
#define LV_USE_SPINNER 0
#define LV_USE_TILEVIEW 0
#define LV_USE_WIN 0
#define LV_USE_SCALE 0
#define LV_USE_ST7789 1

#endif // LV_CONF_H

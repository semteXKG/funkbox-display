#pragma once

#include "lvgl.h"
#include "esp_log.h"
#include "stdio.h"
#include "data.h"
#include "math.h"
#include "esp_timer.h"
#include "event_display.h"
#include "command_display.h"

void lvgl_draw_main_ui(lv_disp_t *disp);
void lvgl_update_data();
void lvgl_set_stint_timer(bool enabled, bool running, int64_t target, int64_t elapsed, int32_t adjust);
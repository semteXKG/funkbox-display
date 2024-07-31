#pragma once

#include <lvgl.h>

void lvgl_draw_main_ui(lv_disp_t *disp);
void lvgl_update_data();
void lvgl_set_stint_timer(bool enabled, long target, long elapsed);
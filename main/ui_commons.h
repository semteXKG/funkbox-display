#pragma once

#ifndef ui_commons
#define ui_commons 1

#include "lvgl.h"

lv_color_t lv_color_ok();

lv_color_t lv_color_warn();

lv_color_t lv_color_crit();

lv_color_t lv_desc_bg();

lv_color_t lv_cont_bg();

lv_color_t lv_main_bg();

lv_color_t lv_border_color();


void draw_as_normal(lv_obj_t* box, lv_obj_t* text);

void draw_as_warn(lv_obj_t* box, lv_obj_t* text);

void draw_as_critical(lv_obj_t* box, lv_obj_t* text);

#endif
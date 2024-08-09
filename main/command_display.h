#include "lvgl.h"
#include "esp_log.h"
#include "stdio.h"
#include "data.h"
#include "math.h"
#include "esp_timer.h"
#include "ui_commons.h"

#pragma once

bool lvgl_set_commands(struct mcu_data* data, lv_obj_t* object, lv_obj_t* label);
#include "lvgl.h"
#include "esp_log.h"
#include "stdio.h"
#include "data.h"
#include "math.h"
#include "esp_timer.h"
#include "ui_commons.h"
#include "proto/message.pb-c.h"

bool lvgl_set_pitstop_since(ProtoMcuData* data, lv_obj_t* object, lv_obj_t* label);

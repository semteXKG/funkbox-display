#include "pitstop_display.h"

bool countdown_viewing = false;
bool lvgl_set_pitstop_since(ProtoMcuData* data, lv_obj_t* object, lv_obj_t* label) {
    if (!data->has_pitstop_since) {
        if (countdown_viewing) {
            lv_obj_move_background(object); 
            countdown_viewing = false;
        }
        return false;
    }
    
    lv_obj_set_style_bg_color(object, lv_cont_bg(), LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);

    countdown_viewing = true;
    lv_obj_move_foreground(object);
    
    long now = esp_timer_get_time() / 1000;

    char text[30];
    sprintf(text, "PITSTOP\n%2.1fs", (double)(data->pitstop_since - data->network_time_adjustment + 45000 - now) / 1000);

    lv_label_set_text(label, text);

    return true;
}
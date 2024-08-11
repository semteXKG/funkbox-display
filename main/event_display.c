#include <event_display.h>

#define DISPLAY_TIME_IN_US 3000000

static const char* EVENT_TAG = "event";

struct event* find_next_showing_event(struct mcu_data* data) {
    for (int i = 0; i < 5; i++) {
        if(data->events[i].displayed_since == 0) {
            return &data->events[i];
        }
    }
    return NULL;
}

int current_event_idx = 0;
int64_t current_event_showing_since = 0;
void lvgl_set_events(struct mcu_data* data, lv_obj_t* object, lv_obj_t* label) {
    if (current_event_showing_since != 0) {
        if (esp_timer_get_time() - current_event_showing_since < DISPLAY_TIME_IN_US) {
            return;
        }
        ESP_LOGI(EVENT_TAG, "EVENT DONE, started %"PRId64", now %"PRId64, current_event_showing_since, esp_timer_get_time());

        current_event_showing_since = 0;
        current_event_idx = 0;
        lv_obj_move_background(object);
    }
   
    struct event* non_disp_event = find_next_showing_event(data);
    if(non_disp_event != NULL) {
        ESP_LOGI(EVENT_TAG, "found event with id for display: %d", non_disp_event->id);
        switch (non_disp_event->severity) {
            case POSITIVE:
                lv_obj_set_style_bg_color(object, lv_color_ok(), LV_PART_MAIN);
                lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
            break;
            case NORMAL:
                lv_obj_set_style_bg_color(object, lv_cont_bg(), LV_PART_MAIN);
                lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
            break;
            case WARN:
               lv_obj_set_style_bg_color(object, lv_color_warn(), LV_PART_MAIN);
               lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
            break;
            case CRIT:
                lv_obj_set_style_bg_color(object, lv_color_crit(), LV_PART_MAIN);
                lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
            break;
        }

        int64_t curr_time = esp_timer_get_time();
        non_disp_event->displayed_since = curr_time;
        current_event_showing_since = curr_time;
        
        lv_obj_move_foreground(object);
        if(strlen(non_disp_event->text) > 0) {
            lv_label_set_text_fmt(label, "%s", non_disp_event->text);
        } else {
            lv_label_set_text(label, "");
        }

    }
}
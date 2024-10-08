#include <event_display.h>

#define DISPLAY_TIME_IN_US 3000000
#define ALREADY_SHOWN_NUM 10

static const char* EVENT_TAG = "event";

int already_shown[ALREADY_SHOWN_NUM];
int already_shown_cnt = 0;

ProtoEvent* find_next_showing_event(ProtoMcuData* data) {
    for (int i = 0; i < data->n_events; i++) {
        bool found = false;
        for (int j = 0; j < ALREADY_SHOWN_NUM; j++) {
            if (data->events[i]->id == already_shown[j]) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            return data->events[i];
        }
    }
    return NULL;
}

void create_event_text(ProtoMcuData* data, ProtoEvent* event, char* buffer) {
       if (event->type ==  PROTO__EVENT__TYPE__EVT_TIME_REMAIN) {
            // ignore last timestamp    
            struct time_str ll = convert_millis_to_time(data->stint->target - data->stint->elapsed);
            if (ll.minutes != 0) {
                sprintf(buffer, "%d min\nLEFT", ll.minutes);
            } else {
                sprintf(buffer, "%d sec\nLEFT", ll.seconds);
            }
        } else if (event->type ==  PROTO__EVENT__TYPE__EVT_LAP) {
            ESP_LOGI(EVENT_TAG, "text: %s", event->text);
            int lap_number = atoi(strtok(event->text, ";"));
            int lap_time_ms = atol(strtok(NULL, ";"));
            int lap_diff_ms = atol(strtok(NULL, ";"));

            event->severity = ((lap_diff_ms < 0) ? PROTO__SEVERITY__POSITIVE : PROTO__SEVERITY__CRIT);

            struct time_str ll = convert_millis_to_time(lap_time_ms);
            struct time_str ll_diff = convert_millis_to_time(lap_diff_ms);

            sprintf(buffer, 
                        "Lap %d\n%1d:%02d.%02d\n%s%1d.%02d", 
                        lap_number,
                        ll.minutes, ll.seconds, ll.milliseconds/10,
                        (lap_diff_ms < 0 ? "-":"+"), ll_diff.seconds + (ll_diff.minutes * 60), ll_diff.milliseconds/10);
        } else if (event->type == PROTO__EVENT__TYPE__EVT_STATE_CHANGE) {
            char* target = strtok(event->text,";");
            char* state = strtok(NULL, ";");
            event->severity = PROTO__SEVERITY__NORMAL;
            sprintf(buffer, "%s\n%s", target, state);
        }
}

int current_event_idx = 0;
int64_t current_event_showing_since = 0;
void lvgl_set_events(ProtoMcuData* data, lv_obj_t* object, lv_obj_t* label) {
    if (current_event_showing_since != 0) {
        if (esp_timer_get_time() - current_event_showing_since < DISPLAY_TIME_IN_US) {
            return;
        }
        ESP_LOGI(EVENT_TAG, "EVENT DONE, started %"PRId64", now %"PRId64, current_event_showing_since, esp_timer_get_time());

        current_event_showing_since = 0;
        current_event_idx = 0;
        lv_obj_move_background(object);
    }
    ProtoEvent* non_disp_event = find_next_showing_event(data);
    if(non_disp_event != NULL) {
        ESP_LOGI(EVENT_TAG, "found event with id for display: %"PRId32, non_disp_event->id);
        
        char buffer[100];
        buffer[0] = '\0';
        create_event_text(data, non_disp_event, buffer);

        switch (non_disp_event->severity) {
            case PROTO__SEVERITY__POSITIVE:
                lv_obj_set_style_bg_color(object, lv_color_ok(), LV_PART_MAIN);
                lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
            break;
            case PROTO__SEVERITY__NORMAL:
                lv_obj_set_style_bg_color(object, lv_cont_bg(), LV_PART_MAIN);
                lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
            break;
            case PROTO__SEVERITY__WARN:
               lv_obj_set_style_bg_color(object, lv_color_warn(), LV_PART_MAIN);
               lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
            break;
            case PROTO__SEVERITY__CRIT:
                lv_obj_set_style_bg_color(object, lv_color_crit(), LV_PART_MAIN);
                lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
            break;
            default: 
            break;
        }

        int64_t curr_time = esp_timer_get_time();
        non_disp_event->displayed_since = curr_time;
        current_event_showing_since = curr_time;
        already_shown[already_shown_cnt++%ALREADY_SHOWN_NUM] = non_disp_event->id;

        lv_obj_move_foreground(object);
        
        if(strlen(buffer) > 0) {
            lv_label_set_text_fmt(label, "%s", buffer);
        } else {
            lv_label_set_text(label, "");
        }

    }
}
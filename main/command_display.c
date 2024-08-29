#include <command_display.h>

static const char* EVENT_TAG = "command";

struct command* find_first_active(struct mcu_data* data) {  
    //ESP_LOGI(EVENT_TAG, "IDX: %d", data->incoming_commands_last_idx);
    for (int i = data->incoming_commands_last_idx - 1; 
         i >= data->incoming_commands_last_idx - 5 && i >= 0; 
         i--) {

        if(data->incoming_commands[i%5].handled == 0) {
            return &data->incoming_commands[i%5];
        }
    }
    return NULL;
}

struct command* currently_viewing = NULL;
int blink_timestamp = 0;
bool lvgl_set_commands(struct mcu_data* data, lv_obj_t* object, lv_obj_t* label) {
    if (currently_viewing != NULL && currently_viewing->handled != 0) {
        currently_viewing = NULL;
        lv_obj_move_background(object);
    }
   
    if(currently_viewing != NULL) {
        int aligned = round((double)esp_timer_get_time() / 1000000);
        if(aligned != blink_timestamp) {
            if (aligned % 2 == 0) {
                draw_as_critical(object, label);
            } else {
                draw_as_normal(object, label);
            }
            blink_timestamp = aligned;
        }
        return true;
    }

    struct command* first_active = find_first_active(data);
    if (first_active == NULL) {
        return false;
    }
    
    currently_viewing = first_active;      
    lv_obj_move_foreground(object);
    switch (currently_viewing->type) {
        case COM_PIT:
            lv_label_set_text(label, "PIT\nNOW");
        break;
        case COM_STINT_OVER:
            lv_label_set_text(label, "STINT\nOVER");
        break;
        case COM_TBD:
            lv_label_set_text(label, "TBD\nWTF");
        break;
        default:
        break;
    }
    return true;
}
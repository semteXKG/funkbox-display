#include <command_display.h>

static const char* COMMAND_TAG = "command";

ProtoCommand* find_first_active(ProtoMcuData* data) {  
    //ESP_LOGI(EVENT_TAG, "IDX: %d", data->incoming_commands_last_idx);
    for (int i = 0; i < data->n_incoming_commands; i++) {
        if (!data->incoming_commands[i]->has_handled) {
            return data->incoming_commands[i];
        }
    }
    return NULL;
}

bool currently_viewing = false;
ProtoCommand current_item;
int blink_timestamp = 0;
bool lvgl_set_commands(ProtoMcuData* data, lv_obj_t* object, lv_obj_t* label) {
    if (currently_viewing) {
        for (int i = 0 ; i < data->n_incoming_commands; i++) {
            if(data->incoming_commands[i]->id == current_item.id && data->incoming_commands[i]->has_handled) {
                currently_viewing = false;
                lv_obj_move_background(object);
            }
        }
    }
   
    if(currently_viewing) {
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

    ProtoCommand* first_active = find_first_active(data);
    if (first_active == NULL) {
        return false;
    }
    
    current_item = *first_active;
    currently_viewing = true;      
    lv_obj_move_foreground(object);
    switch (current_item.type) {
        case PROTO__COMMAND__TYPE__COM_PIT:
            lv_label_set_text(label, "PIT\nNOW");
        break;
        case PROTO__COMMAND__TYPE__COM_STINT_OVER:
            lv_label_set_text(label, "STINT\nOVER");
        break;
        case PROTO__COMMAND__TYPE__COM_FCK:
            lv_label_set_text(label, "FCK FCK\nFCK FCK");
        break;
        default:
        break;
    }
    return true;
}
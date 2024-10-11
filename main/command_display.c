#include <command_display.h>

static const char* COMMAND_TAG = "command";

ProtoCommand* find_first_active(ProtoMcuData* data) {  
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
int lsc_cnt = 0;
bool lvgl_set_commands(ProtoMcuData* data, lv_obj_t* object, lv_obj_t* label) {
    if (currently_viewing) {
        if(lsc_cnt++%100 == 0) {
            ESP_LOGI(COMMAND_TAG, "still viewing");
        }

        bool element_found = false;
        bool reset_data = false;

        for (int i = 0 ; i < data->n_incoming_commands; i++) {
            if(data->incoming_commands[i]->id == current_item.id) {
                element_found = true;
                if (data->incoming_commands[i]->has_handled) {
                    reset_data = true;
                }
            }
        }

        if(!element_found || reset_data) {
            ESP_LOGI(COMMAND_TAG, "RESETTING");
            currently_viewing = false;
            lv_obj_move_background(object);
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
    
    current_item.id = first_active->id;
    current_item.has_id = true;
    current_item.created = first_active->created;
    current_item.has_created = true;
    current_item.type = first_active->type;
    current_item.has_type = true;

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
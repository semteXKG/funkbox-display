#include <data.h>
#include <esp_timer.h>
#include <memory.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/timers.h>
#include <esp_log.h>

static char* DATA_TAG = "data";

ProtoCarSensor oil_warn = PROTO__CAR__SENSOR__INIT;
ProtoCarSensor water_warn = PROTO__CAR__SENSOR__INIT;


ProtoCarSensor get_oil_warn() {
    return oil_warn;
}
ProtoCarSensor get_water_warn() {
    return water_warn;
}

ProtoMcuData* data = NULL;
uint32_t data_age = 0;

SemaphoreHandle_t xSemaphore;

void data_start() {
    xSemaphore =  xSemaphoreCreateMutex();
}

struct time_str convert_millis_to_time(long millis) {
    if (millis < 0) {
        millis *= -1;
    }
    int minutes = millis / (60 * 1000);
    
    int rem = millis - (minutes * 60 * 1000);
    int seconds = rem / 1000;
    int msecs = rem - seconds * 1000;
    struct time_str conversion = {
        .milliseconds = msecs,
        .seconds = seconds,
        .minutes = minutes
    };
    return conversion;
}

struct time_str convert_i64t_millis_to_time(int64_t millis) {
    if (millis < 0) {
        millis *= -1;
    }
    int minutes = millis / (60 * 1000);
    
    int rem = millis - (minutes * 60 * 1000);
    int seconds = rem / 1000;
    int msecs = rem - seconds * 1000;
    struct time_str conversion = {
        .milliseconds = msecs,
        .seconds = seconds,
        .minutes = minutes
    };
    return conversion;
}

bool should_blink() {
    return (esp_timer_get_time() / 2000000) % 2 == 0;
}

ProtoMcuData* get_data() {
    return data;
}

uint32_t get_data_age() {
    return data_age;
}

SemaphoreHandle_t get_mutex() {
    return xSemaphore;
}

void print_data(ProtoMcuData* proto_data) {
    for (int i = 0; i < proto_data->n_incoming_commands; i++) {
        ESP_LOGI(DATA_TAG, "Event %d, Id: %"PRId32", Type: %d, Created %"PRId64", hasHandled: %d Handled %"PRId64, 
            i, 
            proto_data->incoming_commands[i]->id, 
            proto_data->incoming_commands[i]->type, 
            proto_data->incoming_commands[i]->created, 
            proto_data->incoming_commands[i]->has_handled,
            proto_data->incoming_commands[i]->handled);
    }
}

void set_data(ProtoMcuData* proto_data) {
    data = proto_data;
    data_age = esp_timer_get_time() / 1000;
//    print_data(proto_data);
}

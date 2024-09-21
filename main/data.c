#include <data.h>
#include <esp_timer.h>
#include <memory.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/timers.h>
#include <esp_log.h>

ProtoCarSensor oil_warn = PROTO__CAR__SENSOR__INIT;
ProtoCarSensor water_warn = PROTO__CAR__SENSOR__INIT;


ProtoCarSensor get_oil_warn() {
    return oil_warn;
}
ProtoCarSensor get_water_warn() {
    return water_warn;
}

ProtoMcuData* data = NULL;

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

SemaphoreHandle_t get_mutex() {
    return xSemaphore;
}

void set_data(ProtoMcuData* proto_data) {
    data = proto_data;
}

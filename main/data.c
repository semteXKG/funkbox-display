#include <data.h>
#include <esp_timer.h>
#include <memory.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <esp_log.h>

struct mcu_data primary_slot;
struct mcu_data secondary_slot;

struct mcu_data* active = &primary_slot;

struct car_sensor oil_warn;
struct car_sensor water_warn;

struct mcu_data* get_data() {
    return active;
}

struct car_sensor get_oil_warn() {
    return oil_warn;
}
struct car_sensor get_water_warn() {
    return water_warn;
}

struct mcu_data* get_inactive_data() {
    if (active == &primary_slot) {
        return &secondary_slot;
    } else {
        return &primary_slot;
    }
}

void data_switch_active() {
    ESP_LOGI("DATA", "switching data");
    active = get_inactive_data();
}

void update_timestamps(TimerHandle_t xTimer ) {
    struct mcu_data* data = get_data();
    if (data->lap_data.current_lap != -1) {
        data->lap_data.current_lap += 3;
    }

    if (data->stint.enabled) {
        data->stint.elapsed += 3;
    }
}

void init_data_structures(struct mcu_data* data) {
    memset(data, 0, sizeof(struct mcu_data));
    
    data->lap_data.current_lap = -1;
    for (int i = 0; i < 5; i++) {
        data->events[i].displayed_since = 1;
        data->incoming_commands[i].handled = 1;
    }

}

void data_start() {
    memset(&oil_warn, 0, sizeof(struct car_sensor));
    memset(&water_warn, 0, sizeof(struct car_sensor));

    init_data_structures(&primary_slot);
    init_data_structures(&secondary_slot);

    TimerHandle_t timer = xTimerCreate("millisecondAdvancer",
                    pdMS_TO_TICKS(3),
                    pdTRUE,
                    0,
                    update_timestamps);
    xTimerStart(timer, 0);
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


#include <data.h>
#include <driver/gptimer.h>
#include <esp_timer.h>
#include <memory.h>

struct mcu_data* backing_data;
struct car_sensor* backing_oil_warn;
struct car_sensor* backing_water_warn;

struct mcu_data* get_data() {
    return backing_data;
}
struct car_sensor* get_oil_warn() {
    return backing_oil_warn;
}
struct car_sensor* get_water_warn() {
    return backing_water_warn;
}


void data_start() {
    backing_data = (struct mcu_data*) malloc(sizeof(struct mcu_data));
    memset(backing_data, 0, sizeof(struct mcu_data));
    backing_oil_warn = (struct car_sensor*) malloc(sizeof(struct car_sensor));
    memset(backing_oil_warn, 0, sizeof(struct car_sensor));
    backing_water_warn = (struct car_sensor*) malloc(sizeof(struct car_sensor));
    memset(backing_water_warn, 0, sizeof(struct car_sensor));

    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1 * 1000 * 1000, // 1KHz, 1 tick = 1ms
    };
    gptimer_new_timer(&timer_config, &gptimer);
    gptimer_enable(gptimer);
    backing_data->stint.gptimer = gptimer;
    backing_data->stint.gptimer_running = false;

    gptimer_handle_t lap_running_timer = NULL;
    gptimer_new_timer(&timer_config, &lap_running_timer);
    gptimer_enable(lap_running_timer);
    gptimer_start(lap_running_timer);

    backing_data->lap_data.current_lap = lap_running_timer;
    backing_data->lap_data.current_lap_running = false;

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

bool should_blink() {
    return (esp_timer_get_time() / 2000000) % 2 == 0;
}


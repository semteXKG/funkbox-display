#include <data.h>
#include <driver/gptimer.h>

struct mcu_data backing_data;
struct car_sensor backing_oil_warn;
struct car_sensor backing_water_warn;

struct mcu_data* data() {
    return &backing_data;
}
struct car_sensor* oil_warn() {
    return &backing_oil_warn;
}
struct car_sensor* water_warn() {
    return &backing_water_warn;
}


void data_start() {
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1 * 1000 * 1000, // 1KHz, 1 tick = 1ms
    };
    gptimer_new_timer(&timer_config, &gptimer);
    gptimer_enable(gptimer);
    backing_data.stint.gptimer = gptimer;
    backing_data.stint.gptimer_running = false;
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
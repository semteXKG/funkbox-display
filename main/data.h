#pragma once
#include "stdbool.h"
#include "driver/gptimer.h"

enum warn_status {
    OK,
    WARN,
    CRIT
};

struct car_sensor {
    int temp;
    double preassure;

};

struct stint_data {
    bool running;
    bool enabled;
    long target;
    long elapsed;
    gptimer_handle_t gptimer;
    bool gptimer_running;
};

struct lap
{
    int lap_no;
    long lap_time_ms;
};


struct lap_data {
    int lap_no;
    long best_lap;
    bool current_lap_running;
    gptimer_handle_t current_lap;
    struct lap last_laps[5];
};

struct mcu_data {
    struct car_sensor water;
    struct car_sensor oil;
    struct car_sensor gas;
    struct stint_data stint;
    struct lap_data lap_data;  
};

struct time_str {
    int milliseconds;
    int seconds;
    int minutes;
};

struct mcu_data* get_data();
struct car_sensor* get_oil_warn();
struct car_sensor* get_water_warn();

void data_start();

struct time_str convert_millis_to_time(long millis);

bool should_blink();

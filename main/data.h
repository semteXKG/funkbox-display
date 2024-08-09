#pragma once
#include "stdbool.h"

enum severity {
    POSITIVE,
    NORMAL,
    WARN,
    CRIT
};

enum event_type {
    LAP,
    TIME_REMAIN,
    STATE_CHANGE
};

struct  event {
    int id;
    enum event_type type;
    enum severity severity;
    long created_at;
    long displayed_since;
    char text[100];
};

enum command_type {
    PIT,
    STINT_OVER,
    TBD
};

struct command {
    enum command_type type;
    long created;
    long handled;
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
};

struct lap
{
    int lap_no;
    long lap_time_ms;
};

struct lap_data {
    int lap_no;
    long best_lap;
    long current_lap;
    struct lap last_laps[5];
};

struct mcu_data {
    struct car_sensor water;
    struct car_sensor oil;
    struct car_sensor gas;
    struct stint_data stint;
    struct lap_data lap_data;  
    struct event events[5];
    struct command commands[5];
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

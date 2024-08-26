#pragma once
#include "stdbool.h"
#include "inttypes.h"

#define MAX_LAPS 5

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
    int64_t created_at;
    int64_t displayed_since;
    char text[100];
};

enum command_type {
    PIT,
    STINT_OVER,
    TBD
};

struct command {
    enum command_type type;
    int64_t created;
    int64_t handled;
};

struct car_sensor {
    int temp;
    double preassure;
};

struct stint_data {
    bool running;
    bool enabled;
    int64_t target;
    int64_t elapsed;
};

struct lap
{
    int lap_no;
    int64_t lap_time_ms;
};

struct lap_data {
    int lap_no;
    long best_lap;
    long current_lap;
    struct lap last_laps[5];
};

struct mcu_data {
    long network_time_adjustment;
    struct car_sensor water;
    struct car_sensor oil;
    struct car_sensor gas;
    struct stint_data stint;
    struct lap_data lap_data;  
    struct event events[5];
    int events_cnt;
    struct command outgoing_commands[5];
    int outgoing_commands_last_idx;
    struct command incoming_commands[5];
    int incoming_commands_last_idx;
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
struct time_str convert_i64t_millis_to_time(int64_t millis);

bool should_blink();

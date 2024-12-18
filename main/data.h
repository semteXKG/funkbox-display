#pragma once
#include "stdbool.h"
#include "inttypes.h"
#include "proto/message.pb-c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#define MAX_LAPS 5

struct time_str {
    int milliseconds;
    int seconds;
    int minutes;
};


ProtoCarSensor get_oil_warn();
ProtoCarSensor get_water_warn();
SemaphoreHandle_t get_data_mutex();
SemaphoreHandle_t get_lora_mutex();

ProtoMcuData* get_data();
void set_data(ProtoMcuData* proto_data);
uint32_t get_data_age();

void set_lora_stats(ProtoLoraStats* proto_lora_stats);
ProtoLoraStats* get_lora_stats();
uint32_t get_lora_stats_age();

void data_start();

struct time_str convert_millis_to_time(long millis);
struct time_str convert_i64t_millis_to_time(int64_t millis);

bool should_blink();

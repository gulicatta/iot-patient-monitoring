#pragma once

#include <stdbool.h>

bool initialize_fall_detection(void);
void process_alarm_logic(float ax, float ay, float az);
bool is_alarm_active(void);
void reset_alarm(void);
float get_max_acceleration_magnitude_since_last_read(void);
void* fall_detection_Thread(void* arg0);

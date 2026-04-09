#pragma once

#include <stdbool.h>

bool initialize_buttons(void);
void get_button_presses_for_packet(bool* button1_pressed, bool* button2_pressed);

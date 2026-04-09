#pragma once

#include "main_config.h"

/* Display Header files */
#include <ti/display/Display.h>
#include <ti/display/DisplayExt.h>

/* UART header files*/
#include <ti/drivers/UART.h>

#define DISPLAY_TASK_STACK_SIZE 4096

extern Display_Handle display;
extern uint8_t stack_buffer_display_task[DISPLAY_TASK_STACK_SIZE];

UART_Handle uart;
UART_Params uartParams;

void displayTask(uintptr_t arg0, uintptr_t arg1);

#include "display.h"
#include <stdlib.h>

Display_Handle display;
uint8_t stack_buffer_display_task[DISPLAY_TASK_STACK_SIZE];

/* Display constraints */
#define DISPLAY_LINE_WIDTH 21
#define DISPLAY_MAX_LINES  21

static void display_print_wrapped(uint8_t start_line, const char* text)
{
	uint8_t line  = start_line;
	const char* p = text ? text : "";

	while (*p != '\0' && line < DISPLAY_MAX_LINES)
	{
		char buf[DISPLAY_LINE_WIDTH + 1];
		size_t i = 0;

		for (; i < DISPLAY_LINE_WIDTH && p[i] != '\0'; i++)
		{
			buf[i] = p[i];
		}
		buf[i] = '\0';

		Display_printf(display, line, 0, "%-21s", buf);
		p += i;
		line++;
	}

	while (line < DISPLAY_MAX_LINES)
	{
		Display_printf(display, line, 0, "%-21s", "");
		line++;
	}
}

/* Printing received messages to display */
void displayTask(uintptr_t arg0, uintptr_t arg1)
{
	int last_messages_count = 0;
	while (1)
	{
		if (last_messages_count != messages_count)
		{
			Display_printf(display, 1, 0, "Messages: %d", messages_count);
			last_messages_count = messages_count;
			Display_printf(display, 2, 0, "Message:");
			display_print_wrapped(3, received_message);

			if(uart != NULL){
			 UART_write(uart, received_message, strlen(received_message));
			 UART_write(uart, "\r\n", 2);
			}
		}
	}
}

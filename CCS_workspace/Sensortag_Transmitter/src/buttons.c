#include "buttons.h"

#include <stdint.h>

#include <ti/drivers/dpl/HwiP.h>

#include "fall_detection.h"
#include "main_config.h"

#define BUTTON_PRESS_PACKET_HOLD_COUNT 6U

static PIN_Handle button1PinHandle;
static PIN_State button1PinState;
static PIN_Handle button2PinHandle;
static PIN_State button2PinState;
static volatile uint8_t button1_packets_remaining = 0U;
static volatile uint8_t button2_packets_remaining = 0U;

static PIN_Config button1PinTable[] = {Board_PIN_BUTTON0 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE | PIN_HYSTERESIS,
									   PIN_TERMINATE};

static PIN_Config button2PinTable[] = {Board_PIN_BUTTON1 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE | PIN_HYSTERESIS,
									   PIN_TERMINATE};

static void latch_button_press(volatile uint8_t* packets_remaining)
{
	*packets_remaining = BUTTON_PRESS_PACKET_HOLD_COUNT;
}

static bool consume_button_press_for_packet(volatile uint8_t* packets_remaining)
{
	bool pressed  = false;
	uintptr_t key = HwiP_disable();

	if (*packets_remaining > 0U)
	{
		pressed = true;
		(*packets_remaining)--;
	}

	HwiP_restore(key);
	return pressed;
}

static void button1Callback(PIN_Handle handle, PIN_Id pinId)
{
	(void)handle;
	(void)pinId;

	if (PIN_getInputValue(Board_PIN_BUTTON0) == 0)
	{
		latch_button_press(&button1_packets_remaining);
		reset_alarm();
	}
}

static void button2Callback(PIN_Handle handle, PIN_Id pinId)
{
	(void)handle;
	(void)pinId;

	if (PIN_getInputValue(Board_PIN_BUTTON1) == 0)
	{
		latch_button_press(&button2_packets_remaining);
	}
}

bool initialize_buttons(void)
{
	button1PinHandle = PIN_open(&button1PinState, button1PinTable);
	if (button1PinHandle == NULL)
	{
		return false;
	}

	if (PIN_registerIntCb(button1PinHandle, &button1Callback) != 0)
	{
		return false;
	}

	button2PinHandle = PIN_open(&button2PinState, button2PinTable);
	if (button2PinHandle == NULL)
	{
		return false;
	}

	if (PIN_registerIntCb(button2PinHandle, &button2Callback) != 0)
	{
		return false;
	}

	return true;
}

void get_button_presses_for_packet(bool* button1_pressed, bool* button2_pressed)
{
	bool button1_pressed_now = consume_button_press_for_packet(&button1_packets_remaining);
	bool button2_pressed_now = consume_button_press_for_packet(&button2_packets_remaining);

	if (button1_pressed != 0)
	{
		*button1_pressed = button1_pressed_now;
	}

	if (button2_pressed != 0)
	{
		*button2_pressed = button2_pressed_now;
	}
}

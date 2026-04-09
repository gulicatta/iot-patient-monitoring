/*
 * Sensortag Transmitter main application file
 */

/*
 * Copyright (c) 2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/***** Includes *****/
/* Standard C Libraries */
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

/* Application Header files */
#include "main_config.h"
#include "rf_transmitter.h"
#include "message_builder.h"
#include "sensors.h"
#include "fall_detection.h"
#include "device_id.h"
#include "buttons.h"

static PIN_Handle ledPinHandle;
static PIN_State ledPinState;

/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 */
static PIN_Config pinTable[] = {Board_PIN_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
								PIN_TERMINATE};

#define FALL_DETECTION_THREAD_STACKSIZE 1024
#define FALL_DETECTION_THREAD_PRIORITY	1

static bool start_fall_detection_thread(void)
{
	pthread_t thread;
	pthread_attr_t attrs;
	struct sched_param priParam;
	int retc;
	int detachState;

	retc = pthread_attr_init(&attrs);
	if (retc != 0)
	{
		return false;
	}

	detachState = PTHREAD_CREATE_DETACHED;
	retc		= pthread_attr_setdetachstate(&attrs, detachState);
	if (retc != 0)
	{
		return false;
	}

	priParam.sched_priority	 = FALL_DETECTION_THREAD_PRIORITY;
	retc					 = pthread_attr_setschedparam(&attrs, &priParam);
	retc					|= pthread_attr_setstacksize(&attrs, FALL_DETECTION_THREAD_STACKSIZE);
	if (retc != 0)
	{
		return false;
	}

	retc = pthread_create(&thread, &attrs, fall_detection_Thread, NULL);
	return (retc == 0);
}

static bool initialize_sensors(void)
{
	if (!initialize_I2C())
	{
		return false;
	}

	if (!initialize_sensor_lux())
	{
		return false;
	}

	if (!initialize_sensor_bmp280())
	{
		return false;
	}

	if (!initialize_sensor_hdc1000())
	{
		return false;
	}

	if (!initialize_sensor_mpu9250())
	{
		return false;
	}

	return true;
}

void* mainThread(void* arg0)
{
	(void)arg0;

	init_rf_transmitter();

	/* Open LED pins */
	ledPinHandle = PIN_open(&ledPinState, pinTable);
	if (ledPinHandle == NULL)
	{
		while (1)
			;
	}

	if (!initialize_sensors())
	{
		while (1)
			;
	}

	if (!initialize_fall_detection())
	{
		while (1)
			;
	}

	if (!initialize_device_id())
	{
		while (1)
			;
	}

	if (!initialize_buttons())
	{
		while (1)
			;
	}

	if (!start_fall_detection_thread())
	{
		while (1)
			;
	}

	while (1)
	{
		// float lux                 = read_lux();
		float bmp280_temperature_c	= 0.0f;
		float bmp280_pressure_hpa	= 0.0f;
		float hdc1000_temperature_c = 0.0f;
		float hdc1000_humidity_pct	= 0.0f;
		float average_temperature_c = 0.0f;
		float acc_magnitude_g		= 0.0f;
		bool button1_pressed		= false;
		bool button2_pressed		= false;

		get_button_presses_for_packet(&button1_pressed, &button2_pressed);

		read_bmp280(&bmp280_temperature_c, &bmp280_pressure_hpa);
		read_hdc1000(&hdc1000_temperature_c, &hdc1000_humidity_pct);
		acc_magnitude_g		  = get_max_acceleration_magnitude_since_last_read();
		average_temperature_c = (bmp280_temperature_c + hdc1000_temperature_c) / 2.0f;

		clear_message_buffer();

		JSON_open();
		JSON_add_string("id", get_device_id_hex());
		JSON_add_float("temp", average_temperature_c);
		JSON_add_float("hum", hdc1000_humidity_pct);
		JSON_add_bool("b1", button1_pressed);
		JSON_add_bool("b2", button2_pressed);
		JSON_add_float("a", acc_magnitude_g);
		JSON_add_bool("alarm", is_alarm_active());
		JSON_close();

		send_packet_rf(message_buffer);

		PIN_setOutputValue(ledPinHandle, Board_PIN_LED1, !PIN_getOutputValue(Board_PIN_LED1));

		/* Sleep for PACKET_INTERVAL us */
		usleep(PACKET_INTERVAL);
	}
}

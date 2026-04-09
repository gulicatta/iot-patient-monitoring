#include "fall_detection.h"
#include "sensors.h"

#include <math.h>
#include <pthread.h>
#include <stdbool.h>

static bool alarm_active		 = false;
const float THRESHOLD			 = 3.5f;
static float max_acc_magnitude_g = 0.0f;
static pthread_mutex_t magnitude_mutex;
static bool magnitude_mutex_initialized = false;

bool initialize_fall_detection(void)
{
	if (magnitude_mutex_initialized)
	{
		return true;
	}

	if (pthread_mutex_init(&magnitude_mutex, NULL) != 0)
	{
		return false;
	}

	magnitude_mutex_initialized = true;
	return true;
}

void process_alarm_logic(float ax, float ay, float az)
{
	float magnitude = sqrtf((ax * ax) + (ay * ay) + (az * az));

	if (magnitude_mutex_initialized)
	{
		pthread_mutex_lock(&magnitude_mutex);
	}

	if (magnitude > max_acc_magnitude_g)
	{
		max_acc_magnitude_g = magnitude;
	}

	if (magnitude_mutex_initialized)
	{
		pthread_mutex_unlock(&magnitude_mutex);
	}

	if (magnitude > THRESHOLD)
	{
		alarm_active = true;
	}
}

void reset_alarm(void)
{
	alarm_active = false;
}

bool is_alarm_active(void)
{
	return alarm_active;
}

float get_max_acceleration_magnitude_since_last_read(void)
{
	float max_magnitude = 0.0f;

	if (magnitude_mutex_initialized)
	{
		pthread_mutex_lock(&magnitude_mutex);
	}

	max_magnitude		= max_acc_magnitude_g;
	max_acc_magnitude_g = 0.0f;

	if (magnitude_mutex_initialized)
	{
		pthread_mutex_unlock(&magnitude_mutex);
	}

	return max_magnitude;
}

void* fall_detection_Thread(void* arg0)
{
	(void)arg0;

	while (1)
	{
		float acc_x_g	 = 0.0f;
		float acc_y_g	 = 0.0f;
		float acc_z_g	 = 0.0f;
		float gyro_x_dps = 0.0f;
		float gyro_y_dps = 0.0f;
		float gyro_z_dps = 0.0f;

		if (read_mpu9250(&acc_x_g, &acc_y_g, &acc_z_g, &gyro_x_dps, &gyro_y_dps, &gyro_z_dps))
		{
			process_alarm_logic(acc_x_g, acc_y_g, acc_z_g);
		}
	}
}

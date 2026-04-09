#pragma once

#include <stdbool.h>

/* Must be called once before any other sensors.* function. */
bool initialize_I2C(void);

/* Requires initialize_I2C() to have completed successfully. */
bool initialize_sensor_lux(void);

/* Requires initialize_I2C() to have completed successfully. */
float read_lux(void);

/* Requires initialize_I2C() to have completed successfully. */
bool initialize_sensor_bmp280(void);

/* Requires initialize_I2C() to have completed successfully. */
bool read_bmp280(float* temperature_c, float* pressure_hpa);

/* Requires initialize_I2C() to have completed successfully. */
bool initialize_sensor_hdc1000(void);

/* Requires initialize_I2C() to have completed successfully. */
bool read_hdc1000(float* temperature_c, float* humidity_pct);

/* Requires initialize_I2C() to have completed successfully. */
bool initialize_sensor_mpu9250(void);

/* Requires initialize_I2C() to have completed successfully. */
bool read_mpu9250(float* acc_x_g,
                  float* acc_y_g,
                  float* acc_z_g,
                  float* gyro_x_dps,
                  float* gyro_y_dps,
                  float* gyro_z_dps);

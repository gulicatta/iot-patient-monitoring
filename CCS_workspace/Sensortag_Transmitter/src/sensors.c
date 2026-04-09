#include "sensors.h"

#include <stdint.h>
#include <unistd.h>

#include "sensors/SensorI2C.h"
#include "sensors/SensorBmp280.h"
#include "sensors/SensorHdc1000.h"
#include "sensors/SensorMpu9250.h"
#include "sensors/SensorOpt3001.h"

bool initialize_I2C(void)
{
    return SensorI2C_open();
}

bool initialize_sensor_lux(void)
{
    if (!SensorOpt3001_init())
    {
        return false;
    }

    SensorOpt3001_enable(true);
    return true;
}

float read_lux(void)
{
    uint16_t raw_light = 0;

    if (!SensorOpt3001_read(&raw_light))
    {
        return -1.0f;
    }

    return SensorOpt3001_convert(raw_light);
}

bool initialize_sensor_bmp280(void)
{
    if (!SensorBmp280_init())
    {
        return false;
    }

    SensorBmp280_enable(true);
    return true;
}

bool read_bmp280(float* temperature_c, float* pressure_hpa)
{
    uint8_t raw[SENSOR_BMP280_DATASIZE] = {0};
    int32_t temp_centideg = 0;
    uint32_t pressure_q24_8 = 0;

    if (temperature_c == 0 || pressure_hpa == 0)
    {
        return false;
    }

    if (!SensorBmp280_read(raw))
    {
        return false;
    }

    SensorBmp280_convert(raw, &temp_centideg, &pressure_q24_8);

    *temperature_c = ((float)temp_centideg) / 100.0f;
    *pressure_hpa = ((float)pressure_q24_8) / 25600.0f;
    return true;
}

bool initialize_sensor_hdc1000(void)
{
    return SensorHdc1000_init();
}

bool read_hdc1000(float* temperature_c, float* humidity_pct)
{
    uint16_t raw_temp = 0;
    uint16_t raw_hum = 0;

    if (temperature_c == 0 || humidity_pct == 0)
    {
        return false;
    }

    SensorHdc1000_start();
    usleep(20000);

    if (!SensorHdc1000_read(&raw_temp, &raw_hum))
    {
        return false;
    }

    SensorHdc1000_convert(raw_temp, raw_hum, temperature_c, humidity_pct);
    return true;
}

bool initialize_sensor_mpu9250(void)
{
    if (!SensorMpu9250_init())
    {
        return false;
    }

    SensorMpu9250_enable(MPU_AX_ACC | MPU_AX_GYR);
    return true;
}

bool read_mpu9250(float* acc_x_g,
                  float* acc_y_g,
                  float* acc_z_g,
                  float* gyro_x_dps,
                  float* gyro_y_dps,
                  float* gyro_z_dps)
{
    uint16_t acc_raw[3] = {0};
    uint16_t gyro_raw[3] = {0};

    if (acc_x_g == 0 || acc_y_g == 0 || acc_z_g == 0 || gyro_x_dps == 0 || gyro_y_dps == 0 || gyro_z_dps == 0)
    {
        return false;
    }

    if (!SensorMpu9250_accRead(acc_raw))
    {
        return false;
    }

    if (!SensorMpu9250_gyroRead(gyro_raw))
    {
        return false;
    }

    *acc_x_g = SensorMpu9250_accConvert((int16_t)acc_raw[0]);
    *acc_y_g = SensorMpu9250_accConvert((int16_t)acc_raw[1]);
    *acc_z_g = SensorMpu9250_accConvert((int16_t)acc_raw[2]);

    *gyro_x_dps = SensorMpu9250_gyroConvert((int16_t)gyro_raw[0]);
    *gyro_y_dps = SensorMpu9250_gyroConvert((int16_t)gyro_raw[1]);
    *gyro_z_dps = SensorMpu9250_gyroConvert((int16_t)gyro_raw[2]);

    return true;
}

/*
 * sensor_system.cpp
 *
 *  Created on: Jul 13, 2014
 *      Author: pardeep
 */

#include <stdio.h>
#include <math.h>

#include "sensor_system.hpp"
#include "utilities.h"
#include "c_tlm_comp.h"
#include "c_tlm_var.h"



SensorSystem::SensorSystem() : mI2C(I2C2::getInstance())
{
    /* Equal operator so much easier than memset() to zero :)  */
    mAccelero = 0;
    mMagno = 0;
    mGyro = 0;
}

bool SensorSystem::regTlm(void)
{
    bool success = true;
    tlm_component *disk = tlm_component_get_by_name(SYS_CFG_DISK_TLM_NAME);

    if (success) {
        success = tlm_variable_register(disk, "cal_acc", &(mAccelero.offset), sizeof(mAccelero.offset.x),
                                        sizeof(mAccelero.offset)/sizeof(mAccelero.offset.x), tlm_int);
    }
    if (success) {
        success = tlm_variable_register(disk, "cal_mag", &(mMagno.offset), sizeof(mMagno.offset.x),
                                        sizeof(mMagno.offset)/sizeof(mMagno.offset.x), tlm_int);
    }
    if (success) {
        success = tlm_variable_register(disk, "cal_gyro", &(mGyro.offset), sizeof(mGyro.offset.x),
                                        sizeof(mGyro.offset)/sizeof(mGyro.offset.x), tlm_int);
    }

    return success;
}

bool SensorSystem::init(void)
{
    /* Initialize the acceleration sensor */
    bool a = mI2C.checkDeviceResponse(I2CAddr_LSM303_Accel);
    mI2C.writeReg(I2CAddr_LSM303_Accel, 0x20, 0x77);    /// Enable the 3-axis and set update rate to 400hz
    mI2C.writeReg(I2CAddr_LSM303_Accel, 0x23, 0x00);    /// Disable BLE at CTRL_REG4 and set to +/- 2G

    /* Initialize the magnometer sensor */
    bool m = mI2C.checkDeviceResponse(I2CAddr_LSM303_Mag);
    mI2C.writeReg(I2CAddr_LSM303_Mag, 0x00, 0x1C);  /// Set update rate to 220Hz
    mI2C.writeReg(I2CAddr_LSM303_Mag, 0x01, 0x60);  /// Set gain to +/- 2.5 gauss (must change convertRawMagno() if this changes)
    mI2C.writeReg(I2CAddr_LSM303_Mag, 0x02, 0x00);  /// Continuous conversion mode

    /* Initialize the gyroscope */
    bool g = mI2C.checkDeviceResponse(I2CAddr_L3GD20_Gyro);
    mI2C.writeReg(I2CAddr_L3GD20_Gyro, 0x20, 0x00);          /// Reset control register
    mI2C.writeReg(I2CAddr_L3GD20_Gyro, 0x20, (0xB0 | 0x0F)); /// Enable sensor, the 3 axis, and use 380Hz update rate
    mI2C.writeReg(I2CAddr_L3GD20_Gyro, 0x23, 0x00);          /// Disable BLE at CTRL_REG4

    /* Struct size should be six bytes, and all sensor should have responded */
    return (6 == sizeof(rawSensorVector_t) && a && m && g);
}

void SensorSystem::updateSensorData(void)
{
    /**
     * Bit7 must be set to perform address auto-increment on accelero and gyro
     * in order to read more than one register at a time.
     */
    const uint8_t readMultiBytes = (1 << 7);
    const uint8_t acceleroReg = (0x28 | readMultiBytes);
    const uint8_t gyroReg     = (0x28 | readMultiBytes);
    const uint8_t magnoReg    = 0x03;

    /* Read and update accelero data */
    mI2C.readRegisters(I2CAddr_LSM303_Accel, acceleroReg, (uint8_t*)&mAccelero.raw, sizeof(mAccelero.raw));
    convertRawAccelero(mAccelero);

    /* Read and update magno data */
    mI2C.readRegisters(I2CAddr_LSM303_Mag, magnoReg, (uint8_t*)&mMagno.raw, sizeof(mMagno.raw));
    convertRawMagno(mMagno);

    /* Read the gyroscope and convert the readings to radians per second that is needed by AHRS algorithm */
    mI2C.readRegisters(I2CAddr_L3GD20_Gyro, gyroReg, (uint8_t*)&mGyro.raw, sizeof(mGyro.raw));
    convertRawGyro(mGyro);
}

void SensorSystem::convertRawAccelero(sensorData_t &data)
{
#if 0
    /* Adafruit is doing this, and I've noticed that the smallest change is 32 at +/- 2G
     * It looks like actual data resolution is 12-bit, not 16-bit as stated in the datasheet.
     */
    data.raw.x >>= 4;
    data.raw.y >>= 4;
    data.raw.z >>= 4;
#else
    /* Actual data only increments by 16 */
    const int divider = 16;
    data.raw.x /= divider;
    data.raw.y /= divider;
    data.raw.z /= divider;
#endif

    /* Apply calibration values */
    data.raw += data.offset;

    /* Store the end result */
    data.converted.x = data.raw.x;
    data.converted.y = data.raw.y;
    data.converted.z = data.raw.z;
}

void SensorSystem::convertRawGyro(sensorData_t &data)
{
    const float degPerBitFor250dps = 8.75 / 1000.0f;    ///< Datasheet: 8.75 mdps for 250 deg/sec
    const float degPerSecToRadPerSec = 0.0174532925f;   ///< Standard conversion formula

    /* Apply calibration values */
    data.raw += data.offset;

    /* Store the result in floats before we convert to radians per second */
    data.converted.x = data.raw.x;
    data.converted.y = data.raw.y;
    data.converted.z = data.raw.z;

    /* Avoid multiplying (degPerBitFor250dps * degPerSecToRadPerSec) since this calculation
     * is too close to zero value of 32-bit single precision float.  I tested this on
     * windows compiler though, and the result doesn't zero out regardless... need to test here.
     */
    data.converted.x *= degPerBitFor250dps;
    data.converted.y *= degPerBitFor250dps;
    data.converted.z *= degPerBitFor250dps;

    data.converted.x *= degPerSecToRadPerSec;
    data.converted.y *= degPerSecToRadPerSec;
    data.converted.z *= degPerSecToRadPerSec;
}

void SensorSystem::convertRawMagno(sensorData_t &data)
{
#if 0
    /* These are the units at +/- 2.5 gauss */
    const float xyAxisLsbPerGauss = 670.0f;
    const float zAxisLsbPerGauss = 600.0f;
#endif

    /* Magno requires byte swap since HIGH byte register is read first before LOW byte */
    data.raw.byteSwap();

    /* Apply calibration values */
    data.raw += data.offset;

    /* Store to float
     * Datasheet says: Range should be 0xF800 -> 0x07FF (-2048 -> +2047)
     */
    data.converted.x = data.raw.x;
    data.converted.y = data.raw.y;
    data.converted.z = data.raw.z;

    /* Convert to the gauss units to normalize the data of this vector */
    /* TODO: Test to make sure that the raw readings are accurate, and byte order is correct */
#if 0
    data.converted.x /= xyAxisLsbPerGauss;
    data.converted.y /= xyAxisLsbPerGauss;
    data.converted.z /= zAxisLsbPerGauss;
#endif
}

bool SensorSystem::calibrateForZeroOffset(void)
{
    /* Need a large signed int to add up the sum of all samples */
    int64_t ax = 0, ay = 0, az = 0;
    int64_t gx = 0, gy = 0, gz = 0;
    const int32_t samples = 100;

    /* Important to zero out calibration values because updateSensorData() gives us
     * readings after applying the calibration
     */
    mAccelero.offset = 0;
    mGyro.offset = 0;

    /* Find the average zero offset when the sensor is at rest */
    for (int32_t i = 0; i < samples; i++)
    {
        /* Update sensor data at about 100Hz */
        updateSensorData();
        vTaskDelayMs(10);

        ax += mAccelero.raw.x;
        ay += mAccelero.raw.y;
        az += mAccelero.raw.z;

        gx += mGyro.raw.x;
        gy += mGyro.raw.y;
        gz += mGyro.raw.z;
    }

    /* Compute the average of accelero and gyro */
    ax /= samples;
    ay /= samples;
    az /= samples;
    gx /= samples;
    gy /= samples;
    gz /= samples;

    /* X and Y axis should be zero with the sensor flat, and at rest */
    mAccelero.offset.x = -ax;
    mAccelero.offset.y = -ay;

    /* Y should be equivalent to the full gravity pull (1G)
     * If z-axis average shows 1060, then :
     *      maxzValue = 2048   from pow(2, 11) since we add az/8 to intentionally go above
     *      halfzValue = 1024
     *      offset = (1060 - 1024) = 36
     */
    const int16_t maxzValue = (int16_t) pow(2, ceil(log(az + az/8)/log(2)));
    const int16_t halfzValue = (maxzValue / 2);
    mAccelero.offset.z = -(az - halfzValue);

    /* All axis of gyro should show zero when the gyro is at rest */
    mGyro.offset.x = -gx;
    mGyro.offset.y = -gy;
    mGyro.offset.z = -gz;

    /* TODO: Put in the code to calibrate the compass */

    printf("Average accelerometer readings: %d %d %d\n", (int)ax, (int)ay, (int)az);
    printf("Average gyroscope readings: %d %d %d\n", (int)gx, (int)gy, (int)gz);
    puts("Calibration complete!\n");

    return true;
}

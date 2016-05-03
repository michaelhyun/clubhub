/*
 * sensor_system.hpp
 *
 *  Created on: Jul 13, 2014
 *      Author: pardeep
 */

#ifndef SENSOR_SYSTEM_HPP_
#define SENSOR_SYSTEM_HPP_

#include <stdint.h>

#include "i2c2.hpp"
#include "sensor_ifaces.hpp"



/**
 * The sensor system class that communicates with the sensors.
 * This inherits the interfaces that provide readings to the sensors to the flight stabilizer class.
 */
class SensorSystem : public iMagnoIface, public iAcceleroIface, public iGyroIface
{
    public:
        /// The data structure read from the magno, accelero, and the gyro registers
        typedef union rawSensorVector_u
        {
            /// Structure of low and high bytes
            struct {
                uint8_t xl, xh;     ///< X-axis low and high byte
                uint8_t yl, yh;     ///< Y-axis low and high byte
                uint8_t zl, zh;     ///< Z-axis low and high byte
            };

            /// Structure of int16_t that overlaps with low and high bytes
            struct {
                int16_t x, y, z;    ///< Overlapping memory of high/low bytes into a single 16-bit 2's compliment int
            };

            /// Operator to write a single int value to all axis
            void operator =  (int num) { x = y = z = 0; }

            /// Operator to add another vector to this one
            void operator += (const rawSensorVector_u& c) { x += c.x; y += c.y; z += c.z; }

            /// Swaps the order of the high and low bytes
            void byteSwap(void)
            {
                uint8_t high;
                high = xh; xh = xl; xl = high;
                high = yh; yh = yl; yl = high;
                high = zh; zh = zl; zl = high;
            }

        } __attribute__ ((packed)) rawSensorVector_t;

    public:
        SensorSystem();

        /// Initializes the sensors and returns true if successful
        bool init(void);

        /// Registers telemetry
        bool regTlm(void);

        /// @returns interface method that returns the accelerometer readings
        threeAxisVector_t getAcceleroData(void) const       { return mAccelero.converted;   }

        /// @returns interface method that returns the magnetometer readings
        threeAxisVector_t getMagnoData(void) const          { return mMagno.converted;      }

        /// @returns interface method that returns the gyroscope readings
        threeAxisVector_t getGyroAngularData(void) const    { return mGyro.converted;       }

        /** @{ Get raw sensor data readings */
        inline rawSensorVector_t getRawAcceleroData(void)   { return mAccelero.raw;         }
        inline rawSensorVector_t getRawMagnoData(void)      { return mMagno.raw;            }
        inline rawSensorVector_t getRawGyroAngularData(void){ return mGyro.raw;             }
        /** @} */

        /// Updates all the sensor data to be later retrieved by the interface methods
        void updateSensorData(void);

        /**
         * Calibrates the accelerometer and gyroscope sensors when the sensors are sitting flat.
         * This computes the offsets that are needed to force the sensors reading to zero when
         * the sensors are at full rest.  In this case, the acclerometer X/Y and gyro X/Y/Z should
         * be zero values while the accelerometer Z axis should indicate full 1G gravity value.
         *
         * @pre The sensors MUST BE at rest on a flat surface with z-axis pointing down without an angle!
         */
        bool calibrateForZeroOffset(void);

    private:
        /// Structure that contains RAW data type, offset data type, and the converted data type
        typedef struct {
            rawSensorVector_t raw;          ///< Raw data
            rawSensorVector_t offset;       ///< Offset to raw data
            threeAxisVector_t converted;    ///< Converted data

            /// Equal operator to set all values to zero
            void operator = (int num) { raw = 0; offset = 0; converted = 0; }
        } sensorData_t;

        /** @{ Raw to meaningful sensor unit conversion functions */
        void convertRawAccelero(sensorData_t &data);
        void convertRawMagno(sensorData_t &data);
        void convertRawGyro(sensorData_t &data);
        /** @} */

        sensorData_t mAccelero; ///< Acceleration sensor data
        sensorData_t mMagno;    ///< Magno sensor data
        sensorData_t mGyro;     ///< Gyro sensor data

        I2C2         &mI2C;     ///< The instance of I2C bus that is used to communicate with the sensors
};

#endif /* SENSOR_SYSTEM_HPP_ */

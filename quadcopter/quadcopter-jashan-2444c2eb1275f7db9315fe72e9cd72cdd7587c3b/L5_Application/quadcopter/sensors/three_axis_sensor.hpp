/**
 * @file
 */
#ifndef SENSOR_DATA_HPP_
#define SENSOR_DATA_HPP_
#include <stdint.h>
#include "sensor_ifaces.hpp"



/**
 * The class for sensor data.
 * The idea behind this class is that any kind of sensor such as acceleration and gyro
 * can be generalized and put common sensor API here (such as a smoothing filter)
 */
class ThreeAxisSensor
{
    public:
        ThreeAxisSensor() : mSmoothingFilterAlpha(0)
        {
            mLatestData = 0;
            mPreviousData = 0;
        }

        /// Get the filtered sensor data values
        threeAxisVector_t getFilteredValues(void)
        {
            threeAxisVector_t v = mPreviousData;

            v.x = v.x + (mSmoothingFilterAlpha * (mLatestData.x - v.x));
            v.y = v.y + (mSmoothingFilterAlpha * (mLatestData.y - v.y));
            v.z = v.z + (mSmoothingFilterAlpha * (mLatestData.z - v.z));

            return v;
        }

        /// Gets the latest sensor data values set by setRawValues()
        threeAxisVector_t getRawValues(void)
        {
            return mLatestData;
        }

        /// Sets the latest sensor data values and saves the previous values to be used for smoothing filter
        void setRawValues(const threeAxisVector_t& values)
        {
            mPreviousData = mLatestData;
            mLatestData = values;
        }

        /**
         * Sets the smoothing filter's strength
         * @param [in] strength Between 0 and 1.  Closer to zero will generate more smoothing.
         */
        void setSmoothingFilterValue(float value)
        {
            mSmoothingFilterAlpha = value;
        }

    private:
        float mSmoothingFilterAlpha;      ///< Smoothing filter gain
        threeAxisVector_t mLatestData;    ///< Latest values of the sensor
        threeAxisVector_t mPreviousData;  ///< Previous values of the sensor

};


#endif /* SENSOR_DATA_HPP_ */

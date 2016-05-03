/**
 * @file
 */
#ifndef SENSOR_IFACES_HPP_
#define SENSOR_IFACES_HPP_
#include <stdint.h>



/**
 * The structure of 3 axis
 */
typedef struct {
    float x;    ///< X-axis
    float y;    ///< Y-axis
    float z;    ///< Z-axis

    /**
     * Operator to write a value to all axes.
     * Its useful to zero out values by simply using "threeaxis = 0;"
     */
    void operator=(int num) { x = y = z = 0; }
} threeAxisVector_t;

/**
 * The magno sensor interface to get the vector data of the 3 axis
 */
class iMagnoIface
{
    public:
        virtual ~iMagnoIface() {}
        virtual threeAxisVector_t getMagnoData(void) const = 0;
};

/**
 * The acceleration sensor interface to get the vector data of the 3 axis
 */
class iAcceleroIface
{
    public:
        virtual ~iAcceleroIface() {}
        virtual threeAxisVector_t getAcceleroData(void) const = 0;
};

/**
 * The gyro interface to get the angular change of each axis
 */
class iGyroIface
{
    public:
        virtual ~iGyroIface() {}
        virtual threeAxisVector_t getGyroAngularData(void) const = 0;
};



#endif /* SENSOR_IFACES_HPP_ */

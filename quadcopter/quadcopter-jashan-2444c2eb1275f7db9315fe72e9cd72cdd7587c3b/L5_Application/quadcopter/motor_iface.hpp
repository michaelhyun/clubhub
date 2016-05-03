/**
 * @file
 */
#ifndef MOTOR_IFACE_HPP_
#define MOTOR_IFACE_HPP_
#include "friend_for_tlm_reg.hpp"



/**
 * The motor controller interface
 * This is an abstract class that FlightStabilizer calls to set the propeller motor throttle values.
 */
class MotorControllerIface
{
    public:
        /// The structure that contains the motor values of each axis
        typedef struct {
            float north;    ///< North motor
            float south;    ///< South motor
            float east;     ///< East motor
            float west;     ///< West motor
        } motorValues_t;

        /// @returns the current motor values stored at this class
        inline motorValues_t getMotorValues(void) const {  return mMotorValues; } //why inline?

    protected:
        /// Virtual destructor of this abstract class
        virtual ~MotorControllerIface() { }

        /**
         * Interface method
         * This should set the PWM percentage value of the actual motor controllers
         * @param values    The motor values; @see motorValues_t
         */
        virtual void applyMotorValues(const motorValues_t& values) = 0;

        /**
         * Saves the motor values at this class, but doesn't apply them to the motors
         * @param values    The motor values; @see motorValues_t
         */
        inline void saveMotorValues(const motorValues_t& values) { mMotorValues = values; }

    private:
        motorValues_t mMotorValues; ///< The motor values

        // Allow private member access to register variables' telemetry
        ALLOW_FRIEND_TO_REGISTER_TLM();
};



#endif /* MOTOR_IFACE_HPP_ */

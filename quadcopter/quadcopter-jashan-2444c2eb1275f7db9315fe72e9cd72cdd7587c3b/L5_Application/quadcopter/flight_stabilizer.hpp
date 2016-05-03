/**
 * @file
 */
#ifndef FLIGHT_CONTROLLER_HPP_
#define FLIGHT_CONTROLLER_HPP_

#include <stdint.h>

#include "pid.hpp"
#include "sensor_ifaces.hpp"
#include "motor_iface.hpp"



/**
 * This is the flight controller class.
 * This allows the user to set the raw sensor values, process them, and apply
 * the user input to be able to fly the Quadcopter.
 */
class FlightStabilizer : public MotorControllerIface
{
    /* Public types */
    public:
        /// Structure of pitch, roll, and yaw
        typedef struct {
            float pitch;   ///< Pitch angle
            float roll;    ///< Roll angle
            float yaw;     ///< Yaw angle

            void operator=(int num) { pitch = roll = yaw = num; }
        } flightYPR_t;

        /// Common structure used for pitch, roll, yaw, and throttle values
        typedef struct {
            flightYPR_t angle;  /// Pitch, roll, and yaw angles
            uint8_t throttle;   ///< Throttle value

            void operator=(int num) { angle = num; throttle = num; }
        } flightParams_t;

    public:
        /**
         * @{ API for the PID parameters
         */
        inline void setPitchAxisPidParameters(const PID::pidParams_t& params) { mPitchPid.setPidParameters(params); }
        inline void setRollAxisPidParameters(const PID::pidParams_t& params)  { mRollPid.setPidParameters(params);  }
        inline void setYawAxisPidParameters(const PID::pidParams_t& params)   { mYawPid.setPidParameters(params);   }

        inline PID::pidParams_t getPitchAxisPidParameters(void) { return mPitchPid.getPidParameters(); }
        inline PID::pidParams_t getRollAxisPidParameters(void)  { return mRollPid.getPidParameters();  }
        inline PID::pidParams_t getYawAxisPidParameters(void)   { return mYawPid.getPidParameters();   }

        void setCommonPidParameters(float minOutputValue, float maxOutputValue, uint32_t pidUpdateTimeMs);
        /** @} */

        /**
         * Activates or deactivates the flight controller.
         * If the flight controller is already running, disarming will IMMEDIATELY turn off the propellers
         */
        void setArmed(bool armed);

        /// @returns true if the flight controller is armed
        inline bool getArmed(void) const { return mArmed;  }

        /// @returns The current flight angles computed by the sensors
        inline flightYPR_t getCurrentFlightAngles(void) const { return mCurrentAngles; }


        inline flightParams_t getFlightParameters(void)
        {
            return mFlightControllerAngles;
        }

        /**
         * Enables or Disables PID value logging
         * @param frequencyMs   The maximum frequency to log the messages (0 to turn it off)
         */
        void enablePidIoLogging(uint32_t frequencyMs);

    /* Next set of protected methods are meant to be called by the parent class */
    protected:
        /**
         * Computes the values of PRY from the sensor inputs using the AHRS algorithm.
         *
         * @param [in] magno   The reference to the magnetometer sensor interface
         * @param [in] acc     The reference to the acceleration sensor interface
         * @param [in] gyro    The reference to the gyroscope sensor interface
         *
         * The next step is to run the PID and find out the throttle values using computeThrottleValues()
         */
        void computePitchRollYawValues(const uint32_t loopTimeMs, iMagnoIface& magno, iAcceleroIface &acc, iGyroIface &gyro);

        /**
         * Computes the throttle values that should be applied on each motor
         * This runs the PID algorithm on current flight angles, and desired flight parameters
         * set by setFlightParameters() to yield the throttle values to be applied on each motor.
         *
         * The final step to fly is to apply the throttle values using applyThrottleValues()
         * You can obtain the computed values by calling the subclass's getMotorValues() method.
         */
        void computeThrottleValues(const uint32_t timeNowMs);

        /**
         * Applies the propeller values to the motors.
         * This calls the subclass method to retrieve its set values, and then calls
         * its virtual method to apply them.
         * @note If the flight controller is not armed, zero flight values will be applied.
         */
        void applyPropellerValues(void) ;

    protected:
        /// Protected constructor of this abstract class
        FlightStabilizer();

        /**
         * API to set flight parameters
         * @param [in] params   The flight parameters
         */
        inline void setFlightParameters(const flightParams_t& params)
        {
            mFlightControllerAngles = params;
        }

    private:
        flightParams_t mFlightControllerAngles;  ///< Input flight parameters
        flightYPR_t mCurrentAngles;              ///< The current flight angles

        PID mPitchPid;  ///< PID for pitch control
        PID mRollPid;   ///< PID for roll
        PID mYawPid;    ///< PID for yaw

        bool mArmed;                ///< Flag if the quadcopter is armed
        uint32_t mLogFrequencyMs;   ///< The log frequency in milliseconds

        // Allow private member access to register variables' telemetry
        ALLOW_FRIEND_TO_REGISTER_TLM();
};



#endif /* FLIGHT_CONTROLLER_HPP_ */

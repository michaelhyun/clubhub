/**
 * @file
 */
#ifndef QUADCOPTER_BASE_HPP_
#define QUADCOPTER_BASE_HPP_

#include <stdint.h>

#include "flight_stabilizer.hpp"
#include "singleton_template.hpp"
#include "friend_for_tlm_reg.hpp"
#include "sensor_system.hpp"


/**
 * Quadcopter class
 *
 * This is the Quadcopter's "manager" which controls the flight controller class.
 * The flight is stabilized by the flight controller, and this class is responsible
 * for high level control of the flight.
 */
class QuadcopterBase : public FlightStabilizer
{
    /* Public types */
    public:

        /// The GPS data type
        typedef struct {
            float latitude;    ///< GPS latitude
            float longitude;   ///< GPS longitude
            float altMeters;   ///< GPS altitude in meters
            void operator=(int num) { latitude = longitude = altMeters = num; }
        } gpsData_t;

        /// The quadcopter mode type
        typedef enum {
            mode_invalid,      ///< Invalid mode
            mode_auto,         ///< Automatic mode
            mode_manual,       ///< Fully manual mode
        } quadcopterMode_t;

    /* Public API */
    public:
        /**
         * Sets the flight parameters that are applied if the quadcopter is in the manual mode.
         * Certain conditions override the requested parameters, such as kill switch or low battery voltage.
         * @param [in] params   The flight parameters "requested" to be set.
         */
        inline void setFlightControl(const FlightStabilizer::flightParams_t& params) { mRequestedFlightParams = params; }

        /**
         * Sets the current GPS coordinates of the Quadcopter
         * @param [in] data The GPS data
         */
        inline void setCurrentGpsCoordinates(const gpsData_t& data)     {  mCurrentGps = data;    }

        /**
         * Set the destination coordinates to follow in autonomous mode
         * @param [in] data The GPS data
         */
        inline void setDestinationGpsCoordinates(const gpsData_t& data) { mDestinationGps = data; }

        /// Sets the GPS status if GPS has locked on or not
        inline void setGpsStatus(bool locked) { mGpsLocked = locked; }

        /// @returns true if the GPS is receiving a signal
        inline bool getGpsStatus(void) const  { return mGpsLocked; }

        /**
         * Sets the altitude according to the pressure sensor
         * @param [in] alt  The altitude according to the pressure senesor
         */
        inline void setPressureSensorAltitude(float alt)   { mPressureSensorAltitude = alt;  }

        /// @returns the altitude set by setPressureSensorAltitude()
        inline float getPressureSensorAltitude(void) const { return mPressureSensorAltitude; }

        /**
         * Sets the Quadcopter's mode
         * @param [in] mode     The quadcopter mode type
         */
        inline void setOperationMode(quadcopterMode_t mode) { mQuadcopterMode = mode; }

        /// @returns the mode set by setMode()
        inline quadcopterMode_t getOperationMode(void) const { return mQuadcopterMode; }

        /**
         * Sets the battery charge percentage
         * @param [in] batteryPercent  The current battery charge in percentage
         */
        void setBatteryPercentage(uint8_t batteryPercent);

        /// @returns the battery percentage set by setBatteryPercentage()
        inline uint8_t getBatteryPercentage(void) { return mBatteryPercentage; }

        /**
         * Sets the battery charge percentage that triggers low battery Quadcopter mode
         * @param [in] batteryPercent  The percent battery charge
         */
        void setLowBatteryTriggerPercentage(uint8_t batteryPercent);

        /**
         * Engages the "kill switch"
         * @note This may have a similar effect of disarming the FlightStabilizer, but the
         *       key difference is that there is no way of "disengaging" kill switch, while
         *       ARM and DISARM can be performed any time.
         */
        inline void engageKillSwitch(void) { mKillSwitchEngaged = true; }

        /// @returns true if the kill switch has been engaged
        inline bool isKillSwitchEngaged(void) const { return mKillSwitchEngaged; }

        /// Updates the status of the RC receiver; false means RC receiver is out of range or damaged.
        inline void setRcReceiverStatus(bool isHealthy) { mRcReceiverIsHealthy = isHealthy; }

        /** @{ Gets and sets the timing skewed counts */
        inline void incrTimingSkewedCount(void)          { ++mTimingSkewedCount;      }
        inline uint32_t getTimingSkewedCount(void) const { return mTimingSkewedCount; }
        /** @} */

        /// Processes the flying logic to apply the flight parameters to the flight controller
        void updateFlyLogic(void);

        /// Updates the sensor system
        void processSensorData(const uint32_t loopTimeMs, SensorSystem &ref);

        /// Updates the propeller values
        void updatePropellerValues(const uint32_t timeNowMs);

    protected:
        /// Protected constructor of this abstract class
        QuadcopterBase();

    private:
        /**
         * The Quadcopter's internal modes (imode) of operation
         */
        typedef enum {
            imode_invalid,                  ///< Invalid mode

            imode_low_battery,              ///< Low battery
            imode_no_rc_receiver,           ///< No RC receiver input
            imode_kill_switch_engaged,      ///< Kill swtich has been engaged

            imode_auto_mode_hover,          ///< Auto-mode, but hover only
            imode_auto_mode_follow_gps,     ///< Auto-mode, follow GPS

            imode_full_manual,              ///< Full manual mode
        } quadcopterInternalMode_t;

    private:
        quadcopterMode_t mQuadcopterMode;       ///< Quadcopter's mode
        quadcopterInternalMode_t mInternalMode; ///< Quadcopter's internal mode

        uint8_t mBatteryPercentage;             ///< Quadcopter's battery voltage
        uint8_t mLowBatteryTriggerPercentage;   ///< The trigger that enters low battery mode

        bool mRcReceiverIsHealthy;              ///< If valid input is being given by RC receiver remote
        bool mKillSwitchEngaged;                ///< Flag if kill switch has been engaged

        gpsData_t mCurrentGps;                  ///< Current GPS coordinates of the Quadcopter
        gpsData_t mDestinationGps;              ///< Destination GPS coordinates of the Quadcopter
        bool mGpsLocked;                        ///< GPS lock status

        float mPressureSensorAltitude;          ///< The altitude according to the pressure sensor

        /**
         * Count of how many times the timing of function calls is skewed.
         * This should be detected by the user manually and this class merely provides the
         * API to be able to track the number of times the timing was skewed.
         */
        uint32_t mTimingSkewedCount;

        /**
         * Flight parameters being requested by the user (RC receiver).
         * Certain conditions, such as unhealthy RC receiver or low battery voltage may override
         * these user parameters and the Quadcopter will override this input.
         */
        flightParams_t mRequestedFlightParams;

        // Allow private member access to register variables' telemetry
        ALLOW_FRIEND_TO_REGISTER_TLM();
};



#endif /* QUADCOPTER_BASE_HPP_ */

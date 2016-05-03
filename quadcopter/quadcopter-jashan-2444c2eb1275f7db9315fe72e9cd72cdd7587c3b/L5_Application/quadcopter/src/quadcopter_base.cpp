/**
 *
 */

#include <string.h>

#include "quadcopter_base.hpp"
#include "file_logger.h"


QuadcopterBase::QuadcopterBase() :
    mQuadcopterMode(mode_manual),
    mInternalMode(imode_full_manual),
    mBatteryPercentage(100),            /* Assume 100% until changed otherwise */
    mLowBatteryTriggerPercentage(20),   /* Default to 20% until changed otherwise */
    mRcReceiverIsHealthy(true),         /* Assume receiver is healthy until changed */
    mKillSwitchEngaged(false),
    mGpsLocked(false),
    mPressureSensorAltitude(0),
    mTimingSkewedCount(0)
{
    mCurrentGps = 0;
    mDestinationGps = 0;
    mRequestedFlightParams = 0;
}

void QuadcopterBase::setBatteryPercentage(uint8_t batteryPercent)
{
    const uint8_t max = 100;
    if (batteryPercent <= max) {
        mBatteryPercentage = batteryPercent;
    }
}

void QuadcopterBase::setLowBatteryTriggerPercentage(uint8_t batteryPercent)
{
    const uint8_t max = 100;
    if (batteryPercent <= max) {
        mLowBatteryTriggerPercentage = batteryPercent;
    }
}

void QuadcopterBase::updateFlyLogic(void)
{
    /* Ideas are documented here, need to collaborate and discuss:
     *
     *      - If kill switch engaged, go to "landing" mode, but wind down the propellers faster
     *          No way to get out of kill switch mode, must power off.
     *
     *      - If battery is low, go to "landing" mode
     *      - If RC receiver is not healthy, go to "landing" mode
     *
     *      - If autonomous mode, just hover at present location
     *      - If destination is set, slowly approach it
     *
     *      - Finally, if manual mode is engaged, apply that to the flight controller
     */

    /* The following transitions should take place regardless of current state:
     *
     *  - Kill switch is highest priority, and we switch to kill switch in all cases.
     *  - If kill switch not engaged, and battery is low, that's our next priority.
     *  - No RC receiver means we no longer have control of the Quadcopter, so what do we do?
     */
    if (mKillSwitchEngaged) {
        if (imode_kill_switch_engaged != mInternalMode) {
            mInternalMode = imode_kill_switch_engaged;

            LOG_SIMPLE_MSG("Kill switch activated");
        }
    }
    else if (mBatteryPercentage < mLowBatteryTriggerPercentage) {
        if (imode_low_battery != mInternalMode) {
            mInternalMode = imode_low_battery;

            LOG_SIMPLE_MSG("Low battery has been detected - %u/%u %%",
                            mBatteryPercentage, mLowBatteryTriggerPercentage);
        }
    }
    else if (!mRcReceiverIsHealthy) {

    }

    switch (mInternalMode)
    {
        case imode_low_battery:
        case imode_no_rc_receiver:
        case imode_kill_switch_engaged:

        case imode_auto_mode_hover:
        case imode_auto_mode_follow_gps:

        case imode_full_manual:
            FlightStabilizer::setFlightParameters(mRequestedFlightParams);
            break;

        case imode_invalid:
        default:
            LOG_ERROR("Quadcopter is in invalid mode");
            break;
    }

}

void QuadcopterBase::processSensorData(const uint32_t loopTimeMs, SensorSystem &ref)
{
    computePitchRollYawValues(loopTimeMs, ref, ref, ref);
}

void QuadcopterBase::updatePropellerValues(const uint32_t timeNowMs)
{
    /* Using the PID algorithms, compute the throttle values for each propeller */
    computeThrottleValues(timeNowMs);

    /* Finally, apply the propeller values */
    applyPropellerValues();
}


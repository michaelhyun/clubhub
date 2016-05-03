/**
 *
 */

#include <string.h>

#include "flight_stabilizer.hpp"
#include "file_logger.h"
#include "ahrs.hpp"
#include <stdio.h>


FlightStabilizer::FlightStabilizer() :
    mArmed(false),
    mLogFrequencyMs(0)
{
    mFlightControllerAngles = 0;
    mCurrentAngles = 0;
}

void FlightStabilizer::setCommonPidParameters(float minOutputValue, float maxOutputValue, uint32_t pidUpdateTimeMs)
{
    mPitchPid.setOutputLimits(minOutputValue, maxOutputValue);
    mRollPid.setOutputLimits(minOutputValue, maxOutputValue);
    mYawPid.setOutputLimits(minOutputValue, maxOutputValue);

    mPitchPid.setSampleTime(pidUpdateTimeMs);
    mRollPid.setSampleTime(pidUpdateTimeMs);
    mYawPid.setSampleTime(pidUpdateTimeMs);
}

void FlightStabilizer::setArmed(bool armed)
{
    mArmed = armed;

    /* When we are suddenly "ARMED", we don't want our PID to spike its output, so
     * we politely turn it on by using the latest value
     */
    const PID::pidMode_t pidMode = mArmed ? PID::pid_automatic : PID::pid_manual;

    mPitchPid.setMode(pidMode, mCurrentAngles.pitch);
    mRollPid.setMode(pidMode, mCurrentAngles.roll);
    mYawPid.setMode(pidMode, mCurrentAngles.yaw);
}

void FlightStabilizer::enablePidIoLogging(uint32_t frequencyMs)
{
    mLogFrequencyMs = frequencyMs;
}

void FlightStabilizer::computePitchRollYawValues(const uint32_t loopTimeMs, iMagnoIface& magno, iAcceleroIface &acc, iGyroIface &gyro)
{
    threeAxisVector_t m = magno.getMagnoData();
    threeAxisVector_t a = acc.getAcceleroData();
    threeAxisVector_t g = gyro.getGyroAngularData();
    float             ypr[3];

    AHRSupdate(g.x, g.y, g.z, a.x, a.y, a.z, m.x, m.y, m.z, loopTimeMs);
    getYawPitchRoll(ypr);

    mCurrentAngles.yaw = ypr[0];
    mCurrentAngles.pitch = ypr[1];
    mCurrentAngles.roll = ypr[2];
}

void FlightStabilizer::computeThrottleValues(const uint32_t timeNowMs)
{
    MotorControllerIface::motorValues_t values;
    const float throttle = (float) mFlightControllerAngles.throttle;

    /* Get the PRY values we need through the PID */
    const float pitchThrottle = mPitchPid.compute(mFlightControllerAngles.angle.pitch, mCurrentAngles.pitch, timeNowMs);
    const float rollThrottle = mRollPid.compute(mFlightControllerAngles.angle.roll, mCurrentAngles.roll, timeNowMs);
    const float yawThrottle = mYawPid.compute(mFlightControllerAngles.angle.yaw, mCurrentAngles.yaw, timeNowMs);

    /* Do not log the data at frequency higher than mLogFrequencyMs */
    static uint32_t lastTimeMs = 0;
    if (0 != mLogFrequencyMs && (timeNowMs - lastTimeMs) >= mLogFrequencyMs)
    {
        /* Maintain frequency.  So if caller rate is every 4ms, and frequency is 10ms, then
         * we want to log the message at 12, 20, 32, 40 ms etc (about every 10ms)
         *
         * 4 -->  8 --> 12 = LOG
         *       16 --> 20 = LOG
         * 24 -> 28 --> 32 = LOG
         *       36 --> 40 = LOG
         */
        lastTimeMs = timeNowMs - (timeNowMs % mLogFrequencyMs);

        // LOG_SIMPLE_MSG
        printf("%3i,%3i, %3.1f,  %3i,%3i, %3.1f,  %3i,%3i, %3.1f\n",
                (int)mFlightControllerAngles.angle.pitch, (int)mCurrentAngles.pitch, pitchThrottle,
                (int)mFlightControllerAngles.angle.roll, (int)mCurrentAngles.roll, rollThrottle,
                (int)mFlightControllerAngles.angle.yaw, (int)mCurrentAngles.yaw, yawThrottle);
    }

    /* Set the motor values that control the pitch
     * For example, if the desired pitch angle is 15deg (nose up), and actual is zero, then the
     * pitchThrottle value will be positive, and so we need to increase north motor and decrease
     * the south motor.
     */
    if(throttle == 0){
        values.east = 0;
        values.west = 0;
        values.north = 0;
        values.south= 0;
    }
    else{
    values.north = throttle + pitchThrottle;
    values.south = throttle - pitchThrottle;

    /* Set the motor values that control the roll
     * For example, if desired pitch angle is 15deg (to the right), and actual is zero, then the
     * rollThrottle value will be positive, and so we need to increase the west motor and decrease
     * the east motor.
     */
    values.east = throttle - rollThrottle;
    values.west = throttle + rollThrottle;
     }

    // TODO : Remove this hack and refactor to a function called fixPropellerValues()
    if (values.north < 0) values.north = 0;
    if (values.south < 0) values.south = 0;
    if (values.east < 0) values.east = 0;
    if (values.west < 0) values.west = 0;


    /* TODO: Alter the motor values to control the yaw
     * We can do this later once stable flight has been achieved
     */
    (void) yawThrottle; // Avoid the unused variable warning for now

    saveMotorValues(values);
}

void FlightStabilizer::applyPropellerValues(void)
{
    MotorControllerIface::motorValues_t values = { 0 };

    /* If we are armed, only then retrieve the values saved to the motor controller
     * interface through the saveMotorvalues()
     */
    if (mArmed) {
        values = getMotorValues();
    }
    applyMotorValues(values);
}

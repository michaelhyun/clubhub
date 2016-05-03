/**
 * @file
 */

#include <string.h>

#include "pid.hpp"
//#include "uart2.hpp"


PID::PID() :
        mPidOutput(0),
        mPidSetpoint(0),
        mLastTimeMs(0),
        mSampleTimeMs(1000),
        mIntegralTerm(0),
        mLastInput(0),
        mPidProcessingIsOn(false),
        mPidControllerDirection(pid_direction_positive),
        mOutputMin(0),
        mOutputMax(0)
{
    memset(&mPidParams, 0, sizeof(mPidParams));
}

float PID::compute(const float setpointValue, const float presentInputValue, const uint32_t timeNowMs)
{
    //Uart2& u2 = Uart2::getInstance();
    //u2.init(38400);

    /* If PID processing is not ON, we shouldn't update the PID state */
    if (!mPidProcessingIsOn)
    {
        return mPidOutput;
    }

    /* Only process the PID loop if enough time has elapsed since last computation.
     * The exception is when the user forces the PID to process its loop.
     */
    if ((timeNowMs - mLastTimeMs) >= mSampleTimeMs)
    {
        mPidSetpoint = setpointValue;

        /* Compute all the working error variables */
        const float error = mPidSetpoint - presentInputValue;
        mErrorValue = error;
        //u2.printf("sensor = %.2f    remote val=%.2f\n", error);
        mIntegralTerm += (mPidParams.ki * error);

        /* Cap the integral term to avoid PID from using unusable values.
         * http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-reset-windup/
         */
        if (mIntegralTerm > mOutputMax) {
            mIntegralTerm = mOutputMax;
        }
        else if (mIntegralTerm < mOutputMin) {
            mIntegralTerm = mOutputMin;
        }

        /* Avoid the derivative kick:
         * http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-derivative-kick/
         */
        const float dInput = (presentInputValue - mLastInput);

        /* Compute PID Output */
        mPidOutput = (mPidParams.kp * error) + mIntegralTerm - (mPidParams.kd * dInput);

        /* Cap the PID from using unusable values */
        if (mPidOutput > mOutputMax) {
            mPidOutput = mOutputMax;
        }
        else if (mPidOutput < mOutputMin) {
            mPidOutput = mOutputMin;
        }

        /* Remember some variables for next time */
        mLastInput = presentInputValue;
        mLastTimeMs = timeNowMs;
    }

    return mPidOutput;
}

void PID::setPidParameters(const pidParams_t& params)
{
    if (params.kp < 0 || params.ki < 0 || params.kd < 0) {
        return;
    }

    const float SampleTimeInSec = ((float) mSampleTimeMs) / 1000;
    mPidParams.kp = params.kp;
    mPidParams.ki = params.ki * SampleTimeInSec;
    mPidParams.kd = params.kd / SampleTimeInSec;

    /* Negative the PID parameters if the direction is negative
     * http://brettbeauregard.com/blog/2011/04/improving-the-beginners-pid-direction/
     */
    if (pid_direction_negative == mPidControllerDirection)
    {
        mPidParams.kp = (0 - mPidParams.kp);
        mPidParams.ki = (0 - mPidParams.ki);
        mPidParams.kd = (0 - mPidParams.kd);
    }
}

void PID::setSampleTime(const uint32_t newSampleTimeMs)
{
    if (newSampleTimeMs > 0)
    {
        float ratio = (float) newSampleTimeMs / (float) mSampleTimeMs;
        mPidParams.ki *= ratio;
        mPidParams.kd /= ratio;
        mSampleTimeMs = (int) newSampleTimeMs;
    }
}

void PID::setOutputLimits(float min, float max)
{
    if (min > max) {
        return;
    }

    mOutputMin = min;
    mOutputMax = max;

    if (mPidOutput > mOutputMax) {
        mPidOutput = mOutputMax;
    }
    else if (mPidOutput < mOutputMin) {
        mPidOutput = mOutputMin;
    }

    if (mIntegralTerm > mOutputMax) {
        mIntegralTerm = mOutputMax;
    }
    else if (mIntegralTerm < mOutputMin) {
        mIntegralTerm = mOutputMin;
    }
}

void PID::setMode(pidMode_t mode, float latestInput)
{
    bool newAuto = (mode == pid_automatic);

    /* If we just went from pid_manual to auto */
    if (newAuto == !mPidProcessingIsOn)
    {
        init(latestInput);
    }

    mPidProcessingIsOn = newAuto;
}

void PID::init(float latestInput)
{
    mLastInput = latestInput;
    mIntegralTerm = mPidOutput;

    if (mIntegralTerm > mOutputMax) {
        mIntegralTerm = mOutputMax;
    }
    else if (mIntegralTerm < mOutputMin) {
        mIntegralTerm = mOutputMin;
    }
}

void PID::setPidDirection(pidDirection_t pidDirection)
{
    mPidControllerDirection = pidDirection;
}

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "quad_tasks.hpp"
#include "lpc_sys.h"
#include "shared_handles.h"

#include "io.hpp"
#include "file_logger.h"
#include "soft_timer.hpp"
#include "c_tlm_var.h"

#include "uart2.hpp"

/// Define the stack size this task is estimated to use
#define QUADCOPTER_TASK_STACK_BYTES       (4 * 512)

/// Define the frequency of updating sensors and running the AHRS sensor loop
#define QUADCOPTER_SENSOR_FREQUENCY       (500)

/**
 * Define the frequency at which the ESC (electronic speed controllers) will update.
 * This should not be faster than the sensor frequency.  ESCs can usually go from 50-400Hz.
 * 100Hz is ideal for a Quadcopter, while > 100Hz may provide smoother operation.
 */
#define QUADCOPTER_ESC_UPDATE_FREQUENCY   (100) //TODO: Revert to 100 if 500 does not work



// This is the only method that has access to private members, which is used to register telemetry
bool quadcopterRegisterTelemetry(void)
{
    bool success = true;
    const char * tlmName = "quadcopter_vars";
    Quadcopter &q = Quadcopter::getInstance();
    tlm_component *quad = tlm_component_get_by_name(tlmName);
    tlm_component *disk = tlm_component_get_by_name(SYS_CFG_DISK_TLM_NAME);

    /* If telemetry component not found, then create it
     * When task telemetry is enabled, "quadcopter" component would already exist, so using
     * that name for tlmName fails telemetry registration, this needs to be investigated
     */
    if (NULL == quad) {
        quad = tlm_component_add(tlmName);
    }
    if (success) {
        success = (NULL != quad);
    }

    // Quick hack to register variables under quadcopter telemetry component
    #define TLM_REG_QUAD_VAR(name, var, type) \
                tlm_variable_register(quad, name, &(var), sizeof(var), 1, type)

    // Quick hack to register variables under "disk" telemetry
    #define TLM_REG_DISK_QUAD_VAR(name, var, type) \
            tlm_variable_register(disk, name, &(var), sizeof(var), 1, type)

    /* Register Pitch, Roll, and Yaw axis PIDs.
     * These shouldn't be directly modified by the user through telemetry because the PID
     * class manipulates these based on the timings.  terminal command should be used instead
     * to safely modify these parameters.
     */
    if (success) success = TLM_REG_DISK_QUAD_VAR("pid_pitch_kp", q.mPitchPid.mPidParams.kp, tlm_float);
    if (success) success = TLM_REG_DISK_QUAD_VAR("pid_pitch_ki", q.mPitchPid.mPidParams.ki, tlm_float);
    if (success) success = TLM_REG_DISK_QUAD_VAR("pid_pitch_kd", q.mPitchPid.mPidParams.kd, tlm_float);

    if (success) success = TLM_REG_DISK_QUAD_VAR("pid_roll_kp", q.mRollPid.mPidParams.kp, tlm_float);
    if (success) success = TLM_REG_DISK_QUAD_VAR("pid_roll_ki", q.mRollPid.mPidParams.ki, tlm_float);
    if (success) success = TLM_REG_DISK_QUAD_VAR("pid_roll_kd", q.mRollPid.mPidParams.kd, tlm_float);

    if (success) success = TLM_REG_DISK_QUAD_VAR("pid_yaw_kp", q.mYawPid.mPidParams.kp, tlm_float);
    if (success) success = TLM_REG_DISK_QUAD_VAR("pid_yaw_ki", q.mYawPid.mPidParams.ki, tlm_float);
    if (success) success = TLM_REG_DISK_QUAD_VAR("pid_yaw_kd", q.mYawPid.mPidParams.kd, tlm_float);

    /*
     * Register Motor PWM duty cycle values
     */
    if (success) success = TLM_REG_QUAD_VAR("West motor value", q.mMotorValues.west, tlm_float);
    if (success) success = TLM_REG_QUAD_VAR("East motor value", q.mMotorValues.east, tlm_float);
    if (success) success = TLM_REG_QUAD_VAR("North motor value", q.mMotorValues.north, tlm_float);
    if (success) success = TLM_REG_QUAD_VAR("South motor value", q.mMotorValues.south, tlm_float);

    /*This is the present value of the the system*/
    if (success) success = TLM_REG_QUAD_VAR("Present system value", q.mPitchPid.mIntegralTerm, tlm_float);
    if (success) success = TLM_REG_QUAD_VAR("Last system input", q.mPitchPid.mLastInput, tlm_float);
    if (success) success = TLM_REG_QUAD_VAR("Error value SONNNNNN 2AM SONNNN", q.mPitchPid.mErrorValue, tlm_float);

    /* Register the current angles being computed */
    if (success) success = TLM_REG_QUAD_VAR("current_pitch", q.mCurrentAngles.pitch, tlm_float);
    if (success) success = TLM_REG_QUAD_VAR("current_roll",  q.mCurrentAngles.roll, tlm_float);
    if (success) success = TLM_REG_QUAD_VAR("current_yaw",   q.mCurrentAngles.yaw, tlm_float);

    /* Register the requested angles by the user */
    if (success) success = TLM_REG_QUAD_VAR("requested_pitch", q.mRequestedFlightParams.angle.pitch, tlm_float);
    if (success) success = TLM_REG_QUAD_VAR("requested_roll",  q.mRequestedFlightParams.angle.roll, tlm_float);
    if (success) success = TLM_REG_QUAD_VAR("requested_yaw",   q.mRequestedFlightParams.angle.yaw, tlm_float);
    if (success) success = TLM_REG_QUAD_VAR("requested_throttle",   q.mRequestedFlightParams.throttle, tlm_uint);

    /* Register the latest angles applied by the flight controller */
    if (success) success = TLM_REG_QUAD_VAR("applied_pitch", q.mFlightControllerAngles.angle.pitch, tlm_float);
    if (success) success = TLM_REG_QUAD_VAR("applied_roll",  q.mFlightControllerAngles.angle.roll, tlm_float);
    if (success) success = TLM_REG_QUAD_VAR("applied_yaw",   q.mFlightControllerAngles.angle.yaw, tlm_float);
    if (success) success = TLM_REG_QUAD_VAR("applied_throttle",   q.mFlightControllerAngles.throttle, tlm_int);

    /* Register float array of current and destination GPS coordinates' longitude, latitude, and altitude */
    if (success) {
        success = tlm_variable_register(quad, "gps_current_long_lat_alt",
                                        &(q.mCurrentGps), sizeof(q.mCurrentGps), 3, tlm_float);
    }

    /* Register float array of destination GPS coordinates */
    if (success) {
        success = tlm_variable_register(quad, "gps_dst_long_lat_alt",
                                        &(q.mDestinationGps), sizeof(q.mDestinationGps), 3, tlm_float);
    }

    /* Register miscellaneous flags and counters */
    if (success) success = TLM_REG_QUAD_VAR("gps_locked", q.mGpsLocked, tlm_bit_or_bool);
    if (success) success = TLM_REG_QUAD_VAR("rc_recv_ok", q.mRcReceiverIsHealthy, tlm_bit_or_bool);
    if (success) success = TLM_REG_QUAD_VAR("armed", q.mArmed, tlm_bit_or_bool);
    if (success) success = TLM_REG_QUAD_VAR("kill_switch", q.mKillSwitchEngaged, tlm_bit_or_bool);
    if (success) success = TLM_REG_QUAD_VAR("timing_skew_count", q.mTimingSkewedCount, tlm_uint);
    if (success) success = TLM_REG_QUAD_VAR("batt_low_trigger", q.mLowBatteryTriggerPercentage, tlm_uint);
    if (success) success = TLM_REG_QUAD_VAR("batt_percentage",  q.mBatteryPercentage, tlm_uint);

    return success;
}

quadcopter_task::quadcopter_task(const uint8_t priority) :
    scheduler_task("quadcopter", QUADCOPTER_TASK_STACK_BYTES, priority),
    mQuadcopter(Quadcopter::getInstance()),
    mLowBatteryTriggerPercent(20),
    mHighestLoopTimeUs(0),
    mLastCallMs(0),
    mLastPidUpdateTimeMs(0),
    mSensorPollFrequencyMs(1000 / QUADCOPTER_SENSOR_FREQUENCY),
    //TODO: please comment the below line when running at 500Hz
    //mSensorPollFrequencyMs(500),
    mEscUpdateFrequencyMs(1000 / QUADCOPTER_ESC_UPDATE_FREQUENCY),
    mPrintYpr(0), mPrintAcc(0), mPrintGyro(0), mPrintMagno(0), mPrintRC(0)
{
    /* Use init() for memory allocation */
}

bool quadcopter_task::init(void)
{
    bool success = true;
    FlightStabilizer &f = mQuadcopter;

    // Set the PID's min and max PWM output along with the PID update rate
    const float pwmMinPercent = -100;
    const float pwmMaxPercent = 100;
    f.setCommonPidParameters(pwmMinPercent, pwmMaxPercent, mEscUpdateFrequencyMs);

    // Do not update task statistics for this task since it may cause timing skew
    setStatUpdateRate(0);

    // Set the frequency of the run() method
    setRunDuration(mSensorPollFrequencyMs);

    // Initialize the sensors
  // TODO: Revert once sensor is connected
    if (success) {
    success = mSensorSystem.init();
    }

    // Add the calibration signal
    if (success) {
        success = addSharedObject(shared_zeroCalibrate, xSemaphoreCreateBinary());
    }

    return success;
}

bool quadcopter_task::regTlm(void)
{
    bool success = true;

    /* Register all private variables of quadcopter class and its subclasses
     * Only quadcopterRegisterTelemetry() method has access to the private variables.
     */
    if (success) {
        success = quadcopterRegisterTelemetry();
    }

    /* Register the variable we want to preserve on the "disk" */
    if (success) {
        tlm_component *disk = tlm_component_get_by_name(SYS_CFG_DISK_TLM_NAME);
        success = TLM_REG_VAR(disk, mLowBatteryTriggerPercent, tlm_uint);
    }

    /* Register calibration data of the sensor system */
    if (success) {
        success = mSensorSystem.regTlm();
    }

    /* Register any debug counters */
    if (success) {
        tlm_component *dbg = tlm_component_get_by_name("debug");
        success = TLM_REG_VAR(dbg, mHighestLoopTimeUs, tlm_uint);

        TLM_REG_VAR(dbg, mPrintYpr, tlm_uint);
        TLM_REG_VAR(dbg, mPrintAcc, tlm_uint);
        TLM_REG_VAR(dbg, mPrintMagno, tlm_uint);
        TLM_REG_VAR(dbg, mPrintGyro, tlm_uint);
        TLM_REG_VAR(dbg, mPrintRC, tlm_uint);

    }

    return success;
}

bool quadcopter_task::taskEntry(void)
{
    bool success = true;

    /* "Disk" data is restored at this point, so we set it to the Quadcopter class */
    mQuadcopter.setLowBatteryTriggerPercentage(mLowBatteryTriggerPercent);

    while (0 != (xTaskGetTickCount() % mSensorPollFrequencyMs) )
    {
        /* Wait until we get aligned timer just so that the logging will happen
         * at even multiples of the sensor frequency.
         */
    }

    return success;
}

bool quadcopter_task::run(void *p)
{
    const uint32_t loopStart = sys_get_uptime_us();
    const uint32_t millis = sys_get_uptime_ms();

    if (xSemaphoreTake(getSharedObject(shared_zeroCalibrate), 0))
    {
        mSensorSystem.calibrateForZeroOffset();
        mLastCallMs = 0; /* Re-init timing skew */
        return true; /* Don't process rest of the loop for this time */
    }

    /* Detect any "call rate" skew in case we are not getting called at precise timings */
    detectTimingSkew(millis);

    /* Get the sensor inputs */
    mSensorSystem.updateSensorData();

    /* Update the flight sensor system to update AHRS (pitch, roll, and yaw values) */
    mQuadcopter.processSensorData(mSensorPollFrequencyMs, mSensorSystem);

    /* We apply our flying logic, and update propeller values at ideally slower rate
     * than the sensors.  The ESCs cannot respond faster than about 400Hz anyway.
     */
    if ((millis - mLastPidUpdateTimeMs) >= mEscUpdateFrequencyMs)
    {
        mLastPidUpdateTimeMs = millis;

        /* Figure out what the Quadcopter should do */
        mQuadcopter.updateFlyLogic();

        /* Run the PID loop to apply propeller values */
        mQuadcopter.updatePropellerValues(millis);
    }
    /* Do other stuff in a different cycle to not overlap with PID processing time */
    else
    {
        updateStatusLeds();
    }

    /* Capture and store the highest time we spend in our Quadcopter logic's loop */
    const uint32_t loopEnd = sys_get_uptime_us();
    const uint32_t diff = (loopEnd - loopStart);
    if (mHighestLoopTimeUs < diff) {
        mHighestLoopTimeUs = diff;
    }

    static SoftTimer printtimer(100);
    if (printtimer.expired())
    {
        printtimer.restart();

        if (mPrintYpr) {
            mPrintYpr--;
            Quadcopter::flightYPR_t ypr = mQuadcopter.getCurrentFlightAngles();
            printf("Yaw=[%4.1f], Pitch=[%4.1f], Roll=[%4.1f]\n", ypr.yaw, ypr.pitch, ypr.roll);
        }

        SensorSystem::rawSensorVector_t data;
        if (mPrintAcc) {
            mPrintAcc--;
            data = mSensorSystem.getRawAcceleroData();
            printf("Accelero: %6d, %6d, %6d\n", data.x, data.y, data.z);
        }

        if (mPrintGyro) {
            mPrintGyro--;
            data = mSensorSystem.getRawGyroAngularData();
            printf("Gyro: %6d, %6d, %6d\n", data.x, data.y, data.z);
        }

        if (mPrintMagno) {
            mPrintMagno--;
            data = mSensorSystem.getRawMagnoData();
            printf("Magno: %6d, %6d, %6d\n", data.x, data.y, data.z);
        }
        if(mPrintRC) {
            mPrintRC--;
            printf("Throttle: %3u, PRY: %2.1f, %2.1f, %2.1f\n",
                    mQuadcopter.getFlightParameters().throttle,
                    mQuadcopter.getFlightParameters().angle.pitch,
                    mQuadcopter.getFlightParameters().angle.roll,
                    mQuadcopter.getFlightParameters().angle.yaw);
        }
    }

    return true;
}

void quadcopter_task::detectTimingSkew(const uint32_t millis)
{
    /* We don't want to mark the timing skew during the first call (mLastCallMs will be zero at that time).
     *
     * For example, if the last call was at 4ms, and the run duration was 4ms, and if the next call is at >= 9,
     * then there is timing skew.  It was observed that sometimes, possibly due to imprecise timing to get
     * the millisecond counter, the call was made before the runtime, such as at 7ms instead of 8ms so this
     * is okay.
     */
    if (0 != mLastCallMs && millis > (mLastCallMs + getRunDuration()) )
    {
        const uint32_t maxLogMsgs = 10;
        if (mQuadcopter.getTimingSkewedCount() < maxLogMsgs)
        {
            LOG_ERROR("Quadcopter timing skew: Last call %u ms. This call: %u ms, needed %u ms",
                      mLastCallMs, millis, (mLastCallMs + getRunDuration()));
        }
        mQuadcopter.incrTimingSkewedCount();
    }
    mLastCallMs = millis;
}

void quadcopter_task::updateStatusLeds(void)
{
    /* Enumeration of LED number (1-4) */
    enum {
        led_all       = 0xff,
        led_error     = 1,
        led_armDisarm = 2,
        led_gps       = 3,
        led_spare     = 4, /* Available for an indication LED */
    };

    /* Light up all LEDs if the kill switch has been engaged */
    if (mQuadcopter.isKillSwitchEngaged())
    {
        LE.setAll(led_all);
    }
    else
    {
        LE.set(led_error,     (mQuadcopter.getTimingSkewedCount() > 0) );
        LE.set(led_armDisarm, mQuadcopter.getArmed());
        LE.set(led_gps,       mQuadcopter.getGpsStatus());
    }
}

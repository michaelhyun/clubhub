/**
 * @file
 */
#include <stdlib.h>

#include "quad_tasks.hpp"
#include "quadcopter.hpp"

#include "adc0.h"
#include "tlm/c_tlm_var.h"
#include "file_logger.h"



/// Define the stack size this task is estimated to use
#define BATTERY_TASK_STACK_BYTES        (3 * 512)

/// Define the ADC channel to grab external voltage
#define EXTERNAL_VOLTAGE_ADC_CH_NUM     3

/// Define to initialize ADC pin
#define EXTERNAL_VOLTAGE_ADC_INIT()     do { LPC_PINCON->PINSEL1 |= (1 << 20); } while(0)

/// Define the voltage divider used externally
#define EXTERNAL_VOLTAGE_DIVIDER        10



battery_monitor_task::battery_monitor_task(const uint8_t priority) :
    scheduler_task("battery", BATTERY_TASK_STACK_BYTES, priority),
    mLowestMilliVolts(999 * 1000),    /* A really high voltage, it will be reset upon actual voltage sensed */
    mHighestMilliVolts(-1 * 1000),    /* A really low voltage, it will be reset upon actual voltage sensed */
    mMilliVoltDeltaToLog(0.1 * 1000), /* Default configuration to log data if voltage changes */
    mPrevMilliVolts(0),
    /* We *possibly* need to see battery go through at least 1 volt of delta to determine its charge */
    mMinimumDeltaMilliVoltsForValidPercent(2.5 * 1000),
    mAdcSamples(mscNumAdcSamplesBeforeVoltageUpdate)
{
    /* Use init() for memory allocation */
}

bool battery_monitor_task::init(void)
{
    bool success = true;

    if (success) {
        EXTERNAL_VOLTAGE_ADC_INIT();
    }

    /* Register disk variables */
    if (success) {
        tlm_component *disk = tlm_component_get_by_name(SYS_CFG_DISK_TLM_NAME);
        if (success) success = TLM_REG_VAR(disk, mLowestMilliVolts, tlm_uint);
        if (success) success = TLM_REG_VAR(disk, mHighestMilliVolts, tlm_uint);
        if (success) success = TLM_REG_VAR(disk, mMilliVoltDeltaToLog, tlm_uint);
        if (success) success = TLM_REG_VAR(disk, mMinimumDeltaMilliVoltsForValidPercent, tlm_uint);
    }

    // Do not update task statistics (stack usage)
    setStatUpdateRate(0);

    // Monitor the battery at a very slow rate
    setRunDuration(mscSampleFrequencyMs);

    return success;
}

bool battery_monitor_task::run(void *p)
{
    /* Read the ADC */
    const uint16_t adcValue = adc0_get_reading(EXTERNAL_VOLTAGE_ADC_CH_NUM);

    /* Store the result into our samples' array */
    mAdcSamples.storeSample(adcValue);

    /* If samples not ready, just return */
    if (!mAdcSamples.allSamplesReady()) {
        return true;
    }

    /* All samples are ready, so get the average value */
    const uint16_t avgAdcValue = mAdcSamples.getAverage();

    /* Clear the samples for next update */
    mAdcSamples.clear();

    /* Convert ADC to voltage value without using floats
     * 12-bit resolution is in micro-volts, so use that first, then convert to millivolts
     */
    const int32_t adcConverterBitResolution = 12;
    const float adcReferenceVoltage = 3.3f;
    const int32_t microVoltsPerAdcStep = (adcReferenceVoltage * 1000 * 1000) / (1 << adcConverterBitResolution);
    const int32_t milliVolts = microVoltsPerAdcStep * (avgAdcValue * EXTERNAL_VOLTAGE_DIVIDER) / 1000;

    /* Update the high/low voltages */
    if (milliVolts > mHighestMilliVolts) {
        mHighestMilliVolts = milliVolts;
    }
    if (milliVolts < mLowestMilliVolts) {
        mLowestMilliVolts = milliVolts;
    }

    /* Estimate the percentage of charge based on the voltage right now
     * We add 1 to avoid divide by zero.
     */
    uint32_t percent = ((milliVolts - mLowestMilliVolts) * 100) / (1 + mHighestMilliVolts - mLowestMilliVolts);

    /* Set the value to the Quadcopter if we think we've determined a valid battery voltage.
     * If we haven't "learned" about the battery pack, we assume we've got 100% charge.
     * First loop will result in high and low values being the same
     */
    if ( (mHighestMilliVolts - mLowestMilliVolts) < mMinimumDeltaMilliVoltsForValidPercent)
    {
        percent = 100;
    }
    Quadcopter::getInstance().setBatteryPercentage((uint8_t) percent);

    /* Only log data if there is enough delta */
    if ( abs(milliVolts - mPrevMilliVolts) > mMilliVoltDeltaToLog)
    {
        mPrevMilliVolts = milliVolts;

        /* use commas to be in-line with CSV format to easily plot in excel */
        LOG_SIMPLE_MSG("Battery millivolts, %u, estimated charge %%, %u, (%u/%u)",
                        milliVolts, percent, mLowestMilliVolts, mHighestMilliVolts);
    }

    return true;
}

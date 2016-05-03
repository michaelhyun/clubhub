/**
 *
 */

#include "quadcopter.hpp"



/// The frequency of the ESC (electronic speed controller)
#define ESC_FREQUENCY_HZ    500



Quadcopter::Quadcopter() :
    mNorthMotor(PWM::pwm1, ESC_FREQUENCY_HZ),
    mSouthMotor(PWM::pwm2, ESC_FREQUENCY_HZ),
    mEastMotor (PWM::pwm3, ESC_FREQUENCY_HZ),
    mWestMotor (PWM::pwm4, ESC_FREQUENCY_HZ)
{
    /* Nothing to do */
}

// Virtual method implementation
void Quadcopter::applyMotorValues(const motorValues_t& values)
{
    const float minPwmDuty = 10.52;
    const float maxPwmDuty = 95;
    const float dutyDivider = 100/(maxPwmDuty - minPwmDuty);

    motorValues_t jashanEsc = values;
    jashanEsc.north = jashanEsc.north/dutyDivider + minPwmDuty;
    jashanEsc.south = jashanEsc.south/dutyDivider + minPwmDuty;
    jashanEsc.east = jashanEsc.east/dutyDivider + minPwmDuty;
    jashanEsc.west = jashanEsc.west/dutyDivider + minPwmDuty;



    mNorthMotor.set(jashanEsc.north);
    mSouthMotor.set(jashanEsc.south);
    mEastMotor.set(jashanEsc.east);
    mWestMotor.set(jashanEsc.west);
}

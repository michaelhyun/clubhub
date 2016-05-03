/**
 * @file
 */

#ifndef QUADCOPTER_HPP_
#define QUADCOPTER_HPP_

#include "quadcopter_base.hpp"
#include "lpc_pwm.hpp"



/**
 * The quadcopter class
 *
 * This provides the interface to set the rotor PWMs
 */
class Quadcopter : public QuadcopterBase, public SingletonTemplate<Quadcopter>
{
    public:
        /**
         * @{ Pure virtual method overrides of the MotorControllerIface
         */
        void applyMotorValues(const motorValues_t& values);
        /** @} */

    private:
        /// Private constructor for singleton class
        Quadcopter();

        PWM mNorthMotor;    ///< North motor PWM
        PWM mSouthMotor;    ///< South motor PWM
        PWM mEastMotor;     ///< East motor PWM
        PWM mWestMotor;     ///< West motor PWM
        ALLOW_FRIEND_TO_REGISTER_TLM();
        ///< Friend class used for Singleton Template
        friend class SingletonTemplate<Quadcopter>;
};



#endif /* QUADCOPTER_HPP_ */

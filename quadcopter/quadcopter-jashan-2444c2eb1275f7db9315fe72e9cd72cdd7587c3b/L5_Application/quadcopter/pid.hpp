/**
 * @file
 */
#ifndef PID_HPP_
#define PID_HPP_

#include <stdint.h>

#include "friend_for_tlm_reg.hpp"



/**
 * PID loop class, based off of:
 * http://brettbeauregard.com/blog/2011/04/improving-the-beginners-pid-introduction/
 *
 * How to tune your PID parameters:
 *  - Start with Kp=Ki=Kd = 0
 *  - Set Kp until output starts to overshoot and ring significantly.
 *  - Increase Kd until overshoot is reduced to an acceptable level.
 *  - Increase Ki until the final error is zero.
 *
 * @code
 *    // Init the PID
 *    PID p;
 *    p.setSampleTime(10);
 *    p.setPidParameters(2, 1, 0.5);
 *    p.setOutputLimits(0, 100);
 *
 *    // Loop:
 *    while (1) {
 *        float output = p.compute(setpoint, input, timeNowInMilliseconds);
 *
 *        // Apply the output ...
 *    }
 * @endcode
 */
class PID
{
    /* Public types */
    public:
        typedef enum {
            pid_invalid,
            pid_manual,
            pid_automatic,
        } pidMode_t;

        typedef enum {
            pid_direction_invalid,
            pid_direction_positive,
            pid_direction_negative,
        } pidDirection_t;

        /**
         * Kp = Proportional element of the PID, reduces large part of the overall error.
         *    - Increasing Kp will overshoot and oscillate, but reach the desired state faster.
         *    - Increasing Kp will reduce steady state error, but after a certain limit,
         *      increasing Kp will only increase the overshoot.
         *    - Kp reduces the rise time.
         *
         * Ki = Integral element of the PID, reduces the final error accumulated over time.
         *    - Ki eliminates the steady state error, but after a certain limit,
         *      increasing Ki will only increase the overshoot.
         *    - Ki reduces the rise time.
         *
         * Kd = Derivative element of the PID, counteracts Kp, and Ki when the output changes quickly.
         *    - Kd decreases the overshoot.
         *    - Kd reduces the settling time.
         */
        typedef struct {
            float kp;
            float ki;
            float kd;
        } pidParams_t;

    public:
        /// Default constructor
        PID();

        /**
         * Computes the PID output
         *
         * @param setpointValue         The desired setpoint
         * @param presentInputValue     The current input value from the system
         * @param timeNowMs             The current time in milliseconds
         *
         * @return    The PID output value that should be applied to the system
         */
        float compute(const float setpointValue, const float presentInputValue, const uint32_t timeNowMs);

        /// @returns the last output computed by the compute() method
        inline float getLastOutput(void) const { return mPidOutput; }

        /// @returns the set-point value last stored by compute()
        inline float getSetpointValue(void) const { return mPidSetpoint; }

        /**
         * Sets the sample times of the PID and alters the PID paramters kp and ki according to the new time.
         * http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-tuning-changes/
         *
         * The sample time at which the compute() method should run the PID loop.
         * @post The Kp and Ki are changed according to the new ratio of the sample time.
         */
        void setSampleTime(const uint32_t sampleTimeMs);

        /**
         * Sets the parameters of the PID feedback loop
         * @param params    PID parameters, @see pidParams_t
         *
         * @note The PID parameters should be all positive
         */
        void setPidParameters(const pidParams_t& params);

        /// @returns the PID parameters set by setPidParameters()
        inline pidParams_t getPidParameters(void) const { return mPidParams; }

        /**
         * Sets the output limits of the PID to avoid "Reset windup"
         * http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-reset-windup/
         *
         * @param min   The minimum value that can be applied to the system
         * @param max   The maximum value that can be applied to the system
         */
        void setOutputLimits(float min, float max);

        /**
         * Turns ON or OFF the PID processing loop.
         * This can be used to safely override the PID without a sudden change when turning PID on/off
         * http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-onoff/
         *
         * @param mode          The PID mode type
         * @param latestInput   The latest input is needed to re-initialize the PID parameters
         */
        void setMode(pidMode_t mode, float latestInput);

        /**
         * Re-initializes the PID parameters to avoid abrupt changes to the PID when PID
         * processing changes state from ON to OFF and vice versa.
         *
         * @param latestInput   The latest input is needed to re-initialize the PID parameters
         */
        void init(float latestInput);

        /**
         * Sets the PID direction.
         * Positive direction means PID assumes positive output yields positive input.
         * Negative direction means PID assumes negative output yields positive input.
         *
         * @param pidDirection  The PID direction
         */
        void setPidDirection(pidDirection_t pidDirection);

    protected:
    private:

        pidParams_t mPidParams; ///< PID parameters
        float mPidOutput;       ///< The output of the PID algorithm
        float mPidSetpoint;     ///< The target of the PID algorithm
        uint32_t mLastTimeMs;   ///< The last time we processed the PID loop
        uint32_t mSampleTimeMs; ///< The sample time in milliseconds that we should process the PID loop
        float mErrorValue;

        /**
         * http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-tuning-changes/
         * If the PID parameters are changed while the system is running, the integral term changes
         * rapidly that leads to a large output change.  We store this parameter at this variable
         * and re-initialize it when PID parameters are changed.
         */
        float mIntegralTerm;

        /**
         * http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-derivative-kick/
         * We store the last input to avoid the "derivative kick" when the setpoint is changed
         */
        float mLastInput;

        /**
         * http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-onoff/
         * When PID is being overriden and suddenly PID starts to be used, there is a big jump
         * in the output, so we store the PID operation at this variable to avoid it.
         */
        bool mPidProcessingIsOn;

        /**
         * http://brettbeauregard.com/blog/2011/04/improving-the-beginners-pid-direction/
         * The direction of the PID is stored here.  For example, it may be possible that negative value
         * of PID provides a positive change in output (ie: increase is AC causes temperature to drop).
         */
        pidDirection_t mPidControllerDirection;

        /**
         * @{ PID output limits
         *
         * http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-reset-windup/
         * The PID algorithm will stop summing if the output has already reached a max level.
         * This avoid a "lags" where PID has to
         */
        float mOutputMin, mOutputMax;
        /** @} */

        ALLOW_FRIEND_TO_REGISTER_TLM();
};



#endif /* PID_HPP_ */

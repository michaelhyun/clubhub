/**
 * @file
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "scheduler_task.hpp"
//#include "command_handler.hpp"
#include "quadcopter.hpp"
#include "quad_tasks.hpp"
#include "file_logger.h"
//#include "flight_stabilizer.hpp"
#include "soft_timer.hpp"
#include "scheduler_task.hpp"
#include "shared_handles.h"
#include <stdio.h>


/*******************************************************************
 *
 * The purpose of pid_tune is to monitor the output of the PID
 * algorithm so that response of the quadcopter can be determined
 * numerically. The set speed is set from 0% to 50% which is
 * equivalent to a step function. After PID values and output
 * are logged, the values can then be graphed into Excel. The
 * output along with the PID values that have the smoothest
 * curve/response will be chosen.
 *
 *******************************************************************
 */

/// Define the stack size this task is estimated to use
#define PID_TUNE_TASK_STACK_BYTES   (3 * 512)




pid_tune_task::pid_tune_task(const uint8_t priority) :
    scheduler_task("pidtune", PID_TUNE_TASK_STACK_BYTES, priority),
    mQuadcopter(Quadcopter::getInstance()),
    P(0),
    I(0),
    D(0),
    P_start(0),
    P_end(0),
    I_start(0),
    I_end(0),
    D_start(0),
    D_end(0)
{

}

bool pid_tune_task::init(void)
{
    addSharedObject(shared_PIDTuneSemaphore, xSemaphoreCreateBinary());

    return true;
}

/// Note: This is initially set to run on Roll axis. If you want to tune Pitch axis, you must change it accordingly.
bool pid_tune_task::run(void *p)
{
    static SoftTimer count(2000);

    if (xSemaphoreTake(getSharedObject(shared_PIDTuneSemaphore), portMAX_DELAY))
    {
        for (P = P_start; P < P_end; P++) {
            for (I = I_start; I < I_end; I++) {
                for (D = D_start; D < D_end; D++) {
                    motorSpeed.throttle = 0;                        // throttle = 0
                    mQuadcopter.setFlightControl(motorSpeed);       // Set throttle to flight control
                    LOG_SIMPLE_MSG("Throttle 0");
                    params.kp = P;
                    params.ki = I;
                    params.kd = D;
                    mQuadcopter.setRollAxisPidParameters(params);   // Set the PID values
                    Quadcopter::getInstance().enablePidIoLogging(100); //log every 100ms
                    LOG_SIMPLE_MSG("%i Throttle 50", (int) motorSpeed.throttle);
                    count.reset();                                  // start 2 second timer
                    while (count.isRunning()) {
                        motorSpeed.throttle = 50;                   // 0->50% Step function throttle
                        mQuadcopter.setFlightControl(motorSpeed);   // Set throttle = 50%
                    }
                    Quadcopter::getInstance().enablePidIoLogging(0); // stop logging
                }
            }
        }
    }
    else {

    }
    printf("***** Done PID Tune function *****\n");
    return true;
}

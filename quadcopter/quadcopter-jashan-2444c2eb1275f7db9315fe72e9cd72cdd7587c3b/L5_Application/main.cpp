/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * @brief This is the application entry point.
 */
#include "tasks.hpp"
#include "quad_tasks.hpp"
#include "file_logger.h"

#include "uart2.hpp"
#include "uart3.hpp"



/**
 * Hardware pins:
 *  P0.26   ADC-3   1/10th resistor divider of Quadcopter rotors' voltage
 *  P1.30   ADC-4   Available
 *  P1.31   ADC-5   Available
 *
 *  Port2 is the PWM output port (on first six  pins)
 *  P2.0    PWM Rotor 1 - North Motor
 *  P2.1    PWM Rotor 2 - South Motor
 *  P2.2    PWM Rotor 3 - East Motor
 *  P2.3    PWM Rotor 4 - West Motor
 *  P2.4    Available (reserved) PWM
 *  P2.5    Available (reserved) PWM
 *
 *  Port 2 supports interrupts that can be used to capture RC receiver inputs.
 *  P2.6    RC-Ch5 - Arm/Disarm Gear Switch on RC Remote
 *  P2.7    RC-Ch6 (available)
 *
 *  Port 0 supports interrupts, so for additional interrupt(s), we can use:
 *  P0.0    RC-Ch1 - Pitch (Ch2. of RC receiver - Grey wire)
 *  P0.1    RC-Ch2 - Roll (Ch1. of RC receiver - White wire)
 *  P0.29   RC-Ch3 - Yaw (Ch4. of RC receiver - Orange wire)
 *    of RC receiver - Yellow wire)
 *
 *  UART2 is routed to the Xbee socket
 *  P2.8    Uart2 Tx --> Bluetooth
 *  P2.9    Uart2 Rx <-- Bluetooth
 *
 *  UART3 is routed to pin-outs
 *  P4.28   Uart3 Tx --> GPS
 *  P4.29   Uart3 Rx <-- GPS
 *
 *  I2C2    External I2C sensor(s)
 *          Internal addresses: 0x38, 0x40, 0x90
 */

/**
 * The main() creates tasks or "threads".  See the documentation of scheduler_task class at cpp_task.hpp
 * for details.  There is a very simple example towards the beginning of this class's declaration.
 *
 * @warning SPI #1 bus usage notes (interfaced to SD & Flash):
 *      - You can read/write files from multiple tasks because it automatically goes through SPI semaphore.
 *      - If you are going to use the SPI Bus in a FreeRTOS task, you need to use the API at L4_IO/fat/spi_sem.h
 *
 * @warning SPI #0 usage notes (Nordic wireless)
 *      - This bus is more tricky to use because if FreeRTOS is not running, the RIT interrupt may use the bus.
 *      - If FreeRTOS is running, then wireless task may use it.
 *        In either case, you should avoid using this bus or interfacing to external components because
 *        there is no semaphore configured for this bus and it should be used exclusively by nordic wireless.
 */
int main(void)
{
    /* Very important to use & for reference - I learned it the hard way :(
     * Otherwise these go out of scope and out of scope reference can crash the system
     */
    Uart2 &bluetoothUart = Uart2::getInstance();
    Uart3 &gpsUart = Uart3::getInstance();

    /* Initialize the baud rates here, so when tasks run, their UART is ready telem
     * We cannot init() Uarts at the tasks since they only have pointer to UartDev,
     * but it is the Uart2 or Uart3 pointers whose init() initializes their pins.
     * UartDev only initializes UART registers, not the PINSEL or interrupt registers
     */
    bluetoothUart.init(38400, 128, 1024);
    gpsUart.init      (38400, 128, 32);

    /* Quadcopter task should be   the highest priority to process the flight controller algorithms
     * Nothing should be equal or above this priority because we do not want this task to ever
     * be preempted by another task.
     */
    scheduler_add_task(new quadcopter_task (priority_15));

   scheduler_add_task(new pid_tune_task (priority_13));

    /* Priority 14 available, possibly for pressure sensor computations */

    /* The kill-switch task with high priority (consumes very little CPU) */
    scheduler_add_task(new kill_switch_task(priority_12));

    /* Consumes very little CPU, but needs high priority to handle mesh network ACKs */
    scheduler_add_task(new wirelessTask    (priority_11));

    /* Priority 9,10 available */

    /* Terminal task needs high priority to access the system in case a task gets stuck */
    terminalTask *terminal = new terminalTask(priority_8);
    terminal->addCommandChannel(&bluetoothUart, true);
    scheduler_add_task(terminal);

    /* Priority 7 available */

    /* GPS and RC receiver tasks can execute and miss their deadline without a big issue.
     *
     * RC receiver input is queued by ISR, so it doens't need any attention up to 20ms
     * if the queue size is as deep as the number of channels used since the RC receiver
     * frequency is 50Hz for the inputs.
     *
     * Likewise, the GPS data is queued to the UART, so at 38400bps, there are only 4 chars
     * per millisecond.  So a 100 char string would take 25ms, so 100 sized queue should be
     * good enough such that this task won't need any CPU for 25ms.
     */
    scheduler_add_task(new rc_remote_task (priority_6));
    scheduler_add_task(new gps_task       (priority_5, &gpsUart));

    /* Priority 4 available */

    /* Low priority tasks are designed to only execute if there is any CPU left */
    scheduler_add_task(new battery_monitor_task (priority_3));

    /* Initialize the logger and the logger task */
    logger_init(priority_2);

    /* Priority 1 available */

    /* Log the time of Quadcopter startup */
    LOG_SIMPLE_MSG("Quadcopter Startup");

    /* Do not use Priority 0 as it is the IDLE/Sleep task.
     * SPI bus is known to not function correctly when used at the lowest priority task.
     */

    /* No need for IR remote control task */
    // scheduler_add_task(new remoteTask  (PRIORITY_LOW));

    scheduler_start(true, true); ///< This shouldn't return
    return -1;
}

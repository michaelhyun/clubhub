/**
 * @file
 */
#include "quadcopter.hpp"
#include "quad_tasks.hpp"

#include "file_logger.h"
#include "wireless.h"
#include "io.hpp"



/// Define the stack size this task is estimated to use
#define KILL_SWITCH_TASK_STACK_BYTES        (2 * 512)

/// Define the kill switch button API
#define KILL_SWITCH_BUTTON_PRESSED()        SW.getSwitch(1)

/// Define the ARM/DISARM button API
#define ARM_DISARM_SWITCH_BUTTON_PRESSED()  SW.getSwitch(2)



kill_switch_task::kill_switch_task(const uint8_t priority) :
    scheduler_task("killsw", KILL_SWITCH_TASK_STACK_BYTES, priority)
{
    /* Use init() for memory allocation */
}

bool kill_switch_task::init(void)
{
    bool success = true;

    if (success) {
        /* Init something here */
    }

    // Do not update task statistics (stack usage)
    setStatUpdateRate(0);

    return success;
}

bool kill_switch_task::run(void *p)
{
    mesh_packet_t pkt;
    const uint32_t wirelessPktPollMs = 100;
    Quadcopter &q = Quadcopter::getInstance();

    if (wireless_get_rx_pkt(&pkt, wirelessPktPollMs))
    {
        switch (pkt.data[0])
        {
            case wscmd_arm:
                q.setArmed(true);
                break;

            case wscmd_disarm:
                q.setArmed(false);
                break;

            case wscmd_kill:
            default:
                q.engageKillSwitch();
                break;
        }
    }

    if (KILL_SWITCH_BUTTON_PRESSED())
    {
        q.engageKillSwitch();
    }
    if (ARM_DISARM_SWITCH_BUTTON_PRESSED())
    {
        q.setArmed(!q.getArmed());
    }

    return true;
}

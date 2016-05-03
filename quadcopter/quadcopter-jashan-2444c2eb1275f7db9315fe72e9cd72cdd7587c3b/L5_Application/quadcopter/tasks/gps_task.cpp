/**
 * @file
 */

#include <string.h> // strtok_r()
#include <stdlib.h> // atof()

#include "quadcopter.hpp"
#include "quad_tasks.hpp"
#include "file_logger.h"

#include "FreeRTOS.h"
#include "semphr.h"



/// Define the stack size this task is estimated to use
#define GPS_TASK_STACK_BYTES        (4 * 512)



gps_task::gps_task(const uint8_t priority, UartDev *pGpsUart) :
    scheduler_task("gps", GPS_TASK_STACK_BYTES, priority),
    mpGpsUart(pGpsUart)
{
    /* Use init() for memory allocation */
}

bool gps_task::init(void)
{
    bool success = true;

    if (success) {
        /* Init something here */
    }

    // Do not update task statistics (stack usage)
    setStatUpdateRate(0);

    return success;
}

bool gps_task::run(void *p)
{
    Quadcopter &q = Quadcopter::getInstance();
    const uint32_t maxGpsStringLen = 192;
    char buffer[maxGpsStringLen] = { 0 };

    // Assuming 1Hz GPS, we should receive the data within 1100ms
    const uint32_t gpsTimeoutMs = 1000;

    // If GPS fails, or is un-attached, we don't want to log the error every second
    static uint32_t periodicLog = 0;
    const uint32_t periodSeconds = 3 * 60;

    /* Log an error if GPS data not retrieved within the expected time */
    if (!mpGpsUart->gets(&buffer[0], sizeof(buffer) - 1, OS_MS(gpsTimeoutMs))) {
        if (0 == (periodicLog++ % periodSeconds)) {
            LOG_ERROR("GPS data not received within %u ms", gpsTimeoutMs);
        }
        q.setGpsStatus(false);
    }
    else {
        /* Parse the GPS string */
        char *savePtr = NULL;
        const char *split = ",";
        const char *stringId = strtok_r(&buffer[0], split, &savePtr);

        if (0 == strcmp(stringId, "$GPGGA"))
        {
            const char *utcOfPosition = strtok_r(NULL, split, &savePtr);
            const char *latitude      = strtok_r(NULL, split, &savePtr);
            const char *northOrSouth  = strtok_r(NULL, split, &savePtr);
            const char *longitude     = strtok_r(NULL, split, &savePtr);
            const char *eastOrWest    = strtok_r(NULL, split, &savePtr);
            const char *gpsQuality    = strtok_r(NULL, split, &savePtr);
            const char *numSatellites = strtok_r(NULL, split, &savePtr);
            const char *horizontalDil = strtok_r(NULL, split, &savePtr);
            const char *altitude      = strtok_r(NULL, split, &savePtr);

            /* Avoid warnings for unused variables */
            (void) utcOfPosition;
            (void) latitude;
            (void) northOrSouth;
            (void) longitude;
            (void) eastOrWest;
            (void) gpsQuality;
            (void) numSatellites;
            (void) horizontalDil;
            (void) altitude;

            /* Set the GPS data on the Quadcopter class */
            Quadcopter::gpsData_t gpsData;
            gpsData.latitude  = atof(latitude);
            gpsData.longitude = atof(longitude);
            gpsData.altMeters = atof(altitude);
            q.setCurrentGpsCoordinates(gpsData);

            const int minSatsForLock = 3;
            const bool locked = atoi(numSatellites) >= minSatsForLock;
            q.setGpsStatus(locked);
        }
    }

    return true;
}

/**
 * NMEA reference: http://aprs.gids.nl/nmea/#gga
 *
 * GPGGA format:
 * $GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
1    = UTC of Position
2    = Latitude
3    = N or S
4    = Longitude
5    = E or W
6    = GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix)
7    = Number of satellites in use [not those in view]
8    = Horizontal dilution of position
9    = Antenna altitude above/below mean sea level (geoid)
10   = Meters  (Antenna height unit)
11   = Geoidal separation (Diff. between WGS-84 earth ellipsoid and
       mean sea level.  -=geoid is below WGS-84 ellipsoid)
12   = Meters  (Units of geoidal separation)
13   = Age in seconds since last update from diff. reference station
14   = Diff. reference station ID#
15   = Checksum
 */

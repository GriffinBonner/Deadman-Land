/*  Title: deadman_land.c
    Author: Griffin Bonner
    Lab: EEHPC (Tinoosh Mohsenin), University of Maryland Baltimore County
    Date: 10/2/2020
    Description: application layer landing module, rx command to land from failsafe
                 system task when rssi is too low or telemetry with crazyradio PA lost
*/

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "app.h"            // entry to application layer
#include "commander.h"      // position commander
#include "FreeRTOS.h"       // FreeRTOS API
#include "task.h"           // RTOS system tasks
#include "debug.h"          // debugging
#include "log.h"            // internal access to logged variables
#include "param.h"          // internal access to run-time parameters

#include "crtp_commander_high_level.h"  // compute setpoints for smooth trajectory commands

#define DEBUG_MODULE "PUSH" 

// pack setpoint to descrease altitude
static void setHoverSetpoint(setpoint_t *setpoint, float vx, float vy, float vz, float yawrate){
    setpoint->mode.z = modeVelocity; 
    setpoint->velocity.z = vz;
    setpoint->mode.yaw = modeVelocity;
    setpoint->attitudeRate.yaw = yawrate;
    setpoint->mode.x = modeVelocity;
    setpoint->mode.y = modeVelocity;
    setpoint->velocity.x = vx;
    setpoint->velocity.y = vy;
    setpoint->velocity_body = true;
}

// comparison macro
#define MAX(a,b) ((a>b)?a:b) 
#define MIN(a,b) ((a<b)?a:b)

// deadman_land module state control
typedef enum{
    idle,
    connected,
    disconnected,
    rssi_low,
    landing
} dl_state;

// initialize system state
static dl_state state = idle;

// initialize system variables
static uint8_t cur_con = 0;
static uint8_t rssi = 0;
static uint16_t zrange;
static const uint16_t ground_z = 35;

void appMain(){

    vTaskDelay(M2T(3000)); // block task for 3s ( depends on TICK_PERIOD_MS, one tick per period)

    static setpoint_t setpoint;

    uint16_t idConnection = logGetVarId("radio", "isConnected"); // id of connection variable
    uint16_t idRssi = logGetVarId("radio", "rssi"); // id of rssi variable
    uint16_t idZrange = logGetVarId("range","zrange"); // id of z-range from flow deck v2

    DEBUG_PRINT("deadman land module initialized, waiting for activation...\n"); // print debug message

    while(1){

        vTaskDelay(M2T(100)); // block task for 100 ms

        rssi = logGetUint(idRssi); // rssi log parameter
        cur_con = logGetUint(idConnection); // connectivity log parameter
        zrange = logGetUint(idZrange); // z-distance log parameter

        switch (state){
            case idle:
                DEBUG_PRINT("state: idle, rssi: %i, connection: %i, z-range: %i\n",rssi,cur_con,zrange);
                if (cur_con == 1) {
                  state = connected;
                }
                break;
            case connected:
                DEBUG_PRINT("state: connected, rssi: %i, connection: %i, z-range: %i\n",rssi,cur_con,zrange);
                if (cur_con == 0) {
                  state = disconnected;
                }
                break;
            case disconnected:
                DEBUG_PRINT("state: disconnected, rssi: %i, connection: %i, z-range: %i\n",rssi,cur_con,zrange);
                state = landing;
                break;
            case landing:
                DEBUG_PRINT("state: disconnected, rssi: %i, connection: %i, z-range: %i\n",rssi,cur_con,zrange);
                setHoverSetpoint(&setpoint,0,0,-0.15,0);
                commanderSetSetpoint(&setpoint,4);
                if (zrange <= ground_z) {
                  state = idle;
                }
                break;
            default:
                DEBUG_PRINT("rssi: %i, connection: %i, z-range: %i\n",rssi,cur_con,zrange);
                break;
        }
    }
}
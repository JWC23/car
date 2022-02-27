/**************************************************************************//**
 * @file    main.h
 * $Date:   2021/1/22 15:10 $
 * @brief   Project marcos
 *
 * @author JW Chang
 *
*****************************************************************************/
#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NUC029xAN.h"
#include "defs.h"
#include "io.h"
#include "utils.h"
#include "fmc.h"


#define     FW_VERSION_MAJOR        0x22
#define     FW_VERSION_MINOR        0x02
#define     FW_VERSION_TEST         0x27
#define     PROJECT_NAME            "CarPowerBank"

#define     DEBUG_MSG_EN            1
#define     TEST_PIN_EN             0
#define     LOW_POWER_MODE_EN       1
#define     RELAY_DETECTION         1
#define     ERROR_LED_EN            0

#define     RELAY_ONOFF_LIMIT       50000

#define     FLASH_ADDR_FW_VER       0x1F000
#define     FLASH_ADDR_RELAY_ONOFF  0x1F200

#define     FAST_CHARGE_TIMEOUT     252000      // 7 hours



//==========================================
//! Adapter input voltage thresholds
//==========================================
// #define     DC_28P8V                2144
// #define     DC_28V                  2085
// #define     DC_25V                  1861
// #define     DC_24P5V                1824
// #define     DC_24V                  1787    // 24V / (10+1) / 5 * 4096
// #define     DC_23P5V                1750
// #define     DC_23V                  1712
// #define     DC_7V                   521

#define     DC_INPUT_OVP            31000
#define     DC_INPUT_OVP_RLS        30500
#define     DC_INPUT_UVP_RLS        25500
#define     DC_INPUT_UVP            25000
#define     DC_INPUT_OFF            22000


//==========================================
//! Battery pack voltage thresholds
//==========================================
// #define     VBAT_24P5V              1824    // 24.5V / (10+1) / 5 * 4096
// #define     VBAT_24V                1787
// #define     VBAT_23V                1712
// #define     VBAT_1V                 74

#define     CHARGE_OVP_VOLT         31000
#define     CHARGE_UVP_VOLT         25000


//==========================================
// Charging current
//==========================================
#define     CHARGE_OCP              4000
#define     DISCHARGE_OCP           9500



extern vuint16_t g_u16SystemState;
enum eSystemState
{
    SYS_STAT_CHARGE         = MASK(0),
    SYS_STAT_DISCHARGE      = MASK(1),
    SYS_STAT_AC_OK          = MASK(2),
    SYS_STAT_LOAD_OK        = MASK(3),
    SYS_STAT_UART_ENABLE    = MASK(4),
    SYS_STAT_05             = MASK(5),
    SYS_STAT_06             = MASK(6),
    SYS_STAT_07             = MASK(7),
    SYS_STAT_08             = MASK(8),
    SYS_STAT_09             = MASK(9),
    SYS_STAT_10             = MASK(10)
};


extern vuint8_t g_u16SystemFlags;
enum eSystemFlags
{
    SYS_FLAG_100MS          = MASK(0),
    SYS_FLAG_1SEC           = MASK(1),
    SYS_FLAG_SW_1W_LED      = MASK(2),
    SYS_FLAG_DCHG_EN        = MASK(3),
    SYS_FLAG_RELAY_CURR_DETECTED = MASK(4),
    SYS_FLAG_RESET_RELAY_CNTR    = MASK(5),
    SYS_FLAG_SLEEP          = MASK(6),
    SYS_FLAG_SWITCH_INT     = MASK(7)
};


#endif

/**************************************************************************//**
 * @file    relay.c
 * $Date:   2021/4/13 15:00 $
 * @brief   Relay control source file.
 *
 * @author JW Chang
 *
*****************************************************************************/
#include "main.h"
#include "relay.h"
#include "power.h"


#define     ON_OFF_PERIOD       100     // 10 secs

bool g_bRelayTurnOn = FALSE;



//*****************************************************************************
//
//! \brief  Relay control behavior according to SV_RELAY+ voltage.
//!
//! \return None.
//
//*****************************************************************************
void RelayControl(void)
{
    static bool bPrevRelayState = FALSE;
    static uint8_t u8RelayOnOffCntr = 0;

    if ( bPrevRelayState != g_bRelayTurnOn )
    {
        bPrevRelayState = g_bRelayTurnOn;
        u8RelayOnOffCntr = 0;
        RY_NO_OFF();
        RY_NC_OFF();
    }
    else
    {
        if ( u8RelayOnOffCntr <= ON_OFF_PERIOD )
        {
            if ( u8RelayOnOffCntr )     // Assert RY_NO or RY_NC after 100ms delay.
            {
                if ( g_bRelayTurnOn )
                {
                    RY_NO_OFF();
                    RY_NC_ON();
                }
                else
                {
                    RY_NC_OFF();
                    RY_NO_ON();
                }
            }
            u8RelayOnOffCntr++;
        }
        else
        {
            RY_NO_OFF();
            RY_NC_OFF();
        }
    }
}




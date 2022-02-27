/**************************************************************************//**
 * @file    switch.c
 * $Date:   2021/2/23 09:48 $
 * @brief   Switch detection functions source.
 *
 * @author JW Chang
 *
*****************************************************************************/
#include "main.h"
#include "switch.h"
#include "led.h"


// uint8_t g_u8SwitchCntr = 0;

//*****************************************************************************
//
//! \brief Detection for 1W LED switch.
//!
//! \return None.
//
//*****************************************************************************
void Switch_1WDetect(void)
{
    static uint8_t u8Switch1WCntr = 0;

    // if ( SW_DIS_ACTIVE() )      // Ignore when discharge switch is also pressing.
    //     return;

    if ( SW_1WLED_ACTIVE() )
    {
        if ( IS_BIT_CLR(g_u16SystemFlags, SYS_FLAG_SW_1W_LED) )
        {
            if ( ++u8Switch1WCntr >= 3 )
            {
                u8Switch1WCntr = 0;
                SET_BIT(g_u16SystemFlags, SYS_FLAG_SW_1W_LED);
            }
        }
    }
    else
    {
        if ( IS_BIT_SET(g_u16SystemFlags, SYS_FLAG_SW_1W_LED) )
        {
            if ( ++u8Switch1WCntr >= 3 )
            {
                CLR_BIT(g_u16SystemFlags, SYS_FLAG_SW_1W_LED);
                u8Switch1WCntr = 0;
            }

        }
    }
}



//*****************************************************************************
//
//! \brief Detection for discharge switch.
//!
//! \return None.
//
//*****************************************************************************
void Switch_DischargeControl(void)
{
    static uint8_t u8SwitchDchgCntr = 0;
    static uint8_t u8RebootCntr = 0;
    // static bool bLongPress = FALSE;

    //
    // If discharge and 1W LED button are both pressed over 10 seconds, 
    // enable Tx/Rx and jump to LDROM for FW update.
    //
    if ( IS_BIT_SET(g_u16SystemFlags, SYS_FLAG_SW_1W_LED) && SW_DIS_ACTIVE() )
    {
        if ( ++u8RebootCntr >= 100 )
        {
            u8RebootCntr = 0;
            TXRX_ENABLE();
            SetBootFromLDROM();
            WDT_Open(WDT_TIMEOUT_2POW4, WDT_RESET_DELAY_3CLK, TRUE, FALSE);
            while(1);
        }
    }
    else
    {
        u8RebootCntr = 0;
    }
    
    
    if ( SW_DIS_ACTIVE() )
    {
        if ( u8SwitchDchgCntr < 0xFF )
            u8SwitchDchgCntr++;
        
        if ( u8SwitchDchgCntr == 100 )
        {
            if ( IS_BIT_CLR(g_u16SystemFlags, SYS_FLAG_SW_1W_LED) )
            {
                SET_BIT(g_u16SystemFlags, SYS_FLAG_RESET_RELAY_CNTR);
            }
        }

        if ( u8SwitchDchgCntr == 50 )
        {
            SET_BIT(g_u16SystemFlags, SYS_FLAG_DCHG_EN);
            // bLongPress = TRUE;
            // if ( SW_1WLED_ACTIVE() )
            // {
                // TOGGLE_BIT(g_u16SystemState, SYS_STAT_UART_ENABLE);
            // }
        }
    }
    else
    {
        // if ( bLongPress )
        // {
        //     bLongPress = FALSE;
        // }
        // else
        // {
                if ( u8SwitchDchgCntr >= 2 )
                {
                    g_u8LedCntr = 50;
                    // TOGGLE_BIT(g_u16SystemFlags, SYS_FLAG_DCHG_EN);
                }
        // }
        u8SwitchDchgCntr = 0;
    }
}

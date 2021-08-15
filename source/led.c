/**************************************************************************//**
 * @file    led.c
 * $Date:   2021/2/23 09:48 $
 * @brief   LED behavior control functions source.
 *
 * @author JW Chang
 *
*****************************************************************************/
#include "main.h"
#include "led.h"
#include "power.h"
#include "battery.h"
#include "protections.h"


static void LED_Charge(void);
static void LED_Discharge(void);

#if ERROR_LED_EN
static void LED_ErrorState(void);
#endif

uint8_t g_u8LedCntr = 0;


//*****************************************************************************
//
//! \brief LED behavior control.
//!
//! \return None.
//
//*****************************************************************************
void LED_Control(void)
{
    static uint8_t u8LedBlinkCntr = 0;

    if ( IS_BIT_SET(g_u16SystemFlags, SYS_FLAG_SW_1W_LED) )
    {
        LED_1W_ON();
    }
    else
    {
        LED_1W_OFF();
    }

    if ( ++u8LedBlinkCntr >= 5 )
    {
        u8LedBlinkCntr = 0;
    }

    #if ERROR_LED_EN
    if ( g_u16ChargeProtections || g_u8DischargeProtections )
    {
        LED_ErrorState();
        return;
    }
    #endif

        // if ( g_u8LedCntr )
        // {
            // g_u8LedCntr--;
            // LED_Discharge();
        // }
        // else
        // {
            // if ( IS_BIT_SET(g_u16SystemState, SYS_STAT_CHARGE) )
            // {
                // if ( g_sBatteryData.u8Rsoc == 100 )
                // {
                    // LED_GREEN_ON();
                    // LED_RED_OFF();
                // }
                // else
                // {
                    // LED_GREEN_OFF();
                    // LED_RED_ON();
                // }
            // }
            // else
            // {
                // LED_GREEN_OFF();
                // LED_RED_ON();
                // LED_C25_OFF();
                // LED_C50_OFF();
                // LED_C75_OFF();
                // LED_C100_OFF();
            // }
        // }

        // LED_RED_OFF();

    if ( IS_BIT_SET(g_u16SystemState, SYS_STAT_CHARGE) )
    {
        if ( !u8LedBlinkCntr )
        {
            LED_Charge();
        }
    }
    else if ( g_u8LedCntr )
    {
        g_u8LedCntr--;
        LED_Discharge();
    }
    else
    {
        LED_C25_OFF();
        LED_C50_OFF();
        LED_C75_OFF();
        LED_C100_OFF();
    }

}



//*****************************************************************************
//
//! \brief Charge LED display.
//!
//! \return None.
//
//*****************************************************************************
static void LED_Charge(void)
{
    if ( g_sBatteryData.u8Rsoc == 100 )
    {
        LED_C25_ON();
        LED_C50_ON();
        LED_C75_ON();
        LED_C100_ON();
        LED_GREEN_ON();
        LED_RED_OFF();
    }
    else
    {
        LED_GREEN_OFF();
        LED_RED_ON();
        if ( g_sBatteryData.u8Rsoc >= 75 )
        {
            LED_C25_ON();
            LED_C50_ON();
            LED_C75_ON();
            LED_C100_TOGGLE();
        }
        else if ( g_sBatteryData.u8Rsoc >= 50 )
        {
            LED_C25_ON();
            LED_C50_ON();
            LED_C75_TOGGLE();
            LED_C100_OFF();
        }
        else if ( g_sBatteryData.u8Rsoc >= 25 )
        {
            LED_C25_ON();
            LED_C50_TOGGLE();
            LED_C75_OFF();
            LED_C100_OFF();
        }
        else
        {
            LED_C25_TOGGLE();
            LED_C50_OFF();
            LED_C75_OFF();
            LED_C100_OFF();
        }
    }
}


//*****************************************************************************
//
//! \brief Discharge LED display.
//!
//! \return None.
//
//*****************************************************************************
static void LED_Discharge(void)
{
    LED_RED_OFF();
    LED_GREEN_OFF();

    if ( g_sBatteryData.u8Rsoc >= 75 )
    {
        LED_C25_ON();
        LED_C50_ON();
        LED_C75_ON();
        LED_C100_ON();
    }
    else if ( g_sBatteryData.u8Rsoc >= 50 )
    {
        LED_C25_ON();
        LED_C50_ON();
        LED_C75_ON();
        LED_C100_OFF();
    }
    else if ( g_sBatteryData.u8Rsoc >= 25 )
    {
        LED_C25_ON();
        LED_C50_ON();
        LED_C75_OFF();
        LED_C100_OFF();
    }
    else
    {
        LED_C25_ON();
        LED_C50_OFF();
        LED_C75_OFF();
        LED_C100_OFF();
    }
}


#if ERROR_LED_EN
//*****************************************************************************
//
//! \brief Update Error State and ID.
//!        Blink Error LED 4 times with long or short period, long represents 1
//!        and short represents 0.
//!
//! \return None.
//
//*****************************************************************************
static void LED_ErrorState(void)
{
    static uint8_t u8ErrorID = 0, u8BitCntr = 0, i = 0;

    if ( IS_BIT_SET(g_u16ChargeProtections, CHG_PROT_BATT_UVP) )            u8ErrorID = 1;
    else if ( IS_BIT_SET(g_u16ChargeProtections, CHG_PROT_BATT_OVP) )       u8ErrorID = 2;
    else if ( IS_BIT_SET(g_u16ChargeProtections, CHG_PROT_OTP) )            u8ErrorID = 3;
    else if ( IS_BIT_SET(g_u16ChargeProtections, CHG_PROT_OCP) )            u8ErrorID = 4;
    else if ( IS_BIT_SET(g_u16ChargeProtections, CHG_PROT_TIMEOUT) )        u8ErrorID = 5;
    else if ( IS_BIT_SET(g_u16ChargeProtections, CHG_PROT_INPUT_OVP) )      u8ErrorID = 6;
    else if ( IS_BIT_SET(g_u16ChargeProtections, CHG_PROT_INPUT_UVP) )      u8ErrorID = 7;
    else if ( IS_BIT_SET(g_u8DischargeProtections, DCHG_PROT_BATT_UVP) )    u8ErrorID = 8;
    else if ( IS_BIT_SET(g_u8DischargeProtections, DCHG_PROT_OCP) )         u8ErrorID = 9;
    else
        u8ErrorID = 0xF;

    if ( u8ErrorID )
    {
        if ( i == 4 )
        {
            LED_RED_OFF();
            if ( ++u8BitCntr >= 10 )     // turn off 1s then repeat
            {
                u8BitCntr = 0;
                i = 0;
            }
        }
        else
        {
            if ( u8BitCntr )
            {
                u8BitCntr--;
                if ( !u8BitCntr )
                {
                    LED_RED_OFF();
                    i++;
                }
            }
            else
            {
                if ( GBI(u8ErrorID, i) )
                {
                    u8BitCntr = 6;      // turn on long period
                }
                else
                {
                    u8BitCntr = 2;      // turn on short period
                }
                LED_RED_ON();
            }
        }
    }
    // else
    // {
    //     LED_RED_ON();
    // }

    LED_C25_OFF();
    LED_C50_OFF();
    LED_C75_OFF();
    LED_C100_OFF();
    LED_GREEN_OFF();
}

#endif



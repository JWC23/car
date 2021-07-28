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


#define     ON_OFF_PERIOD       30     // 3 secs



bool g_bRelayTurnOn = FALSE;
static uint32_t g_u32RelayOnOffTimes = 0;
static uint16_t g_u16FlashRelayIdx = 0;


static void StoreRelayOnOffTimes(void);



//*****************************************************************************
//
//! \brief  Get latest relay counter index.
//!
//! \return None.
//
//*****************************************************************************
void RelayVar_Init(void)
{
    uint16_t i;
    uint32_t u32ReadData = 0, u32PrevReadData = 0;

    for ( i = 0; i < FMC_FLASH_PAGE_SIZE; i+=4 )
    {
        u32ReadData = FMC_Read(FLASH_ADDR_RELAY_ONOFF+i);
        if ( u32ReadData == 0xFFFFFFFF )
        {
            g_u16FlashRelayIdx = i;
            g_u32RelayOnOffTimes = u32PrevReadData;
            return;
        }

        u32PrevReadData = u32ReadData;
    }

    // If all values in the flash page are written, then start writing from index 0.
    g_u16FlashRelayIdx = 0;
    g_u32RelayOnOffTimes = FMC_Read(FLASH_ADDR_RELAY_ONOFF);

}



//*****************************************************************************
//
//! \brief  Store relay on/off counter to data flash 0x1F200 ~ 0x1F3FF.
//!
//! \return None.
//
//*****************************************************************************
static void StoreRelayOnOffTimes(void)
{
    if ( g_u16FlashRelayIdx == 0 )
    {
        EraseAPROMArea(FLASH_ADDR_RELAY_ONOFF, FLASH_ADDR_RELAY_ONOFF+FMC_FLASH_PAGE_SIZE);
    }

    WriteAPROM(FLASH_ADDR_RELAY_ONOFF + g_u16FlashRelayIdx, &g_u32RelayOnOffTimes, 1);

    g_u16FlashRelayIdx += 4;

    if ( g_u16FlashRelayIdx >= FMC_FLASH_PAGE_SIZE )
    {
        g_u16FlashRelayIdx = 0;
    }
}



//*****************************************************************************
//
//! \brief  Reset relay counter and .
//!
//! \return None.
//
//*****************************************************************************
void CheckResetRelayCounter(void)
{
    if ( IS_BIT_SET(g_u16SystemFlags, SYS_FLAG_RESET_RELAY_CNTR) )
    {
        CLR_BIT(g_u16SystemFlags, SYS_FLAG_RESET_RELAY_CNTR);
        g_u32RelayOnOffTimes = 0;
        StoreRelayOnOffTimes();
    }
}



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

    // Do not allow turn on relay if it reaches limit.
    if ( (g_u32RelayOnOffTimes > RELAY_ONOFF_LIMIT) && (g_bRelayTurnOn == FALSE) )
    {
        return;
    }

    if ( bPrevRelayState != g_bRelayTurnOn )
    {
        bPrevRelayState = g_bRelayTurnOn;
        u8RelayOnOffCntr = 0;
        RY_NO_OFF();
        RY_NC_OFF();
        g_u32RelayOnOffTimes++;
        StoreRelayOnOffTimes();
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




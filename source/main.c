/**************************************************************************//**
 * @file    main.c
 * $Date:   2021/02/22 17:03 $
 * @brief   NUC029xAN project code.
 *
 * @nauthor JW Chang
 *
 ******************************************************************************/
#include "main.h"
#include "init.h"
#include "power.h"
#include "battery.h"
#include "relay.h"
#include "uart_func.h"
// #include "switch.h"


static void OneHundredMsHandler(void);
static void OneSecondHandler(void);
#if DEBUG_MSG_EN
static void DisplayDebugMsg(void);
#endif

vuint16_t g_u16SystemState = 0;     //!< System states, each bit indicates different status.
vuint8_t g_u16SystemFlags = 0;      //!< System flags, each bit indicates different flags.
uint32_t g_u32FwVer = 0;


//*****************************************************************************
//
//! \brief  Handle tasks execute every 100 ms.
//!
//! \return None.
//
//*****************************************************************************
static void OneHundredMsHandler(void)
{
    if ( IS_BIT_CLR(g_u16SystemFlags, SYS_FLAG_100MS) )
        return;

    CLR_BIT(g_u16SystemFlags, SYS_FLAG_100MS);

    UpdateAllAdcData();
    PowerControl();
    RelayControl();
    CheckResetRelayCounter();
    WDT_RESET_COUNTER();
}




//*****************************************************************************
//
//! \brief  Handle tasks execute every 1 second.
//!
//! \return None.
//
//*****************************************************************************
static void OneSecondHandler(void)
{
    if ( IS_BIT_CLR(g_u16SystemFlags, SYS_FLAG_1SEC) )
        return;

    CLR_BIT(g_u16SystemFlags, SYS_FLAG_1SEC);

    Batt_UpdateRsocByOCV();

    #if DEBUG_MSG_EN
    DisplayDebugMsg();
    #endif

}



#if DEBUG_MSG_EN
//*****************************************************************************
//
//! \brief  Send UART debug messages.
//!
//! \return None.
//
//*****************************************************************************
static void DisplayDebugMsg(void)
{
    printf("ADP(%d) PCK(%d) Chg(%d) Dchg(%d) RLY(%d) SVRLY(%d)",
        g_sAdcData.u16AdapterVolt, g_sAdcData.u16PackVolt, g_sAdcData.u16ChargeCurr,
        g_sAdcData.u16DischargeCurr, g_sAdcData.u16RelayCurr, g_sAdcData.u16RelayVolt);
    if ( IS_BIT_SET(g_u16SystemFlags, SYS_FLAG_SW_1W_LED) )
        printf(" (1W On)");
    if ( SW_DIS_ACTIVE() )
        printf(" (DisBtn On)");
    // printf(" Dchg= %d", g_u8SwitchCntr);
    // if ( TXRX_EN_STAT() )
        // printf(" EN: high");
    // else
        // printf(" EN: low");

    if ( IS_BIT_SET(g_u16SystemState, SYS_STAT_CHARGE) )
        printf(" Chg");
    else if ( IS_BIT_SET(g_u16SystemState, SYS_STAT_DISCHARGE) )
        printf(" Dis");
    else
        printf(" Idle");

    printf("\n");
}
#endif


//*****************************************************************************
//
//! \brief  Main function.
//!
//! \return None.
//
//*****************************************************************************
int main(void)
{
    // uint32_t u32TimerCntr = 0;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init UART0 for printf */
    UART0_Init();

    GPIO_Init();

    Timer0_Init();

    /* Enable FMC ISP function */
    FMC_Open();

    // if ( !SetIAPBoot() )
    // {
    //     #if DEBUG_MSG_EN
    //     printf("Failed to set IAP boot mode!\n");
    //     #endif
    // }

    #if DEBUG_MSG_EN
    printf("\n========== V%02X.%02X.%02X ==========\n", FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_TEST);
    #endif

    RelayVar_Init();

    g_u32FwVer = FMC_Read(FLASH_ADDR_FW_VER);

    /* Lock protected registers */
    // SYS_LockReg();

    // Update FW version.
    if ( g_u32FwVer != DWORD(0, FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_TEST) )
    {
        EraseAPROMArea(FLASH_ADDR_FW_VER, FLASH_ADDR_FW_VER+FMC_FLASH_PAGE_SIZE);
        WriteAPROM(FLASH_ADDR_FW_VER, &g_u32FwVer, 1);
    }

    // if(WDT_GET_RESET_FLAG() == 1)
    // {
        // WDT_CLEAR_RESET_FLAG();
        // printf("\n*** WDT time-out reset occurred ***\n");
    // }

    while(1)
    {
        if ( IS_BIT_SET(g_u16SystemFlags, SYS_FLAG_SLEEP) )
        {
            SleepModeHandler();
        }
        else
        {
            OneHundredMsHandler();
            OneSecondHandler();
            UartRxHandler();
        }
    }
}



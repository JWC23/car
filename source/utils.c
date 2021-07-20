/**************************************************************************//**
 * @file    utils.c
 * $Date:   2021/02/22 15:10 $
 * @brief   Source file utility functions.
 *
 * @author JW Chang
 *
*****************************************************************************/
#include "main.h"
#include "utils.h"




//*****************************************************************************
//
//! \brief Check state debounce.
//!
//! \return True if state is true for over delay time.
//
//*****************************************************************************
bool StateDebounce(bool bState, uint8_t u8Delay, uint8_t *u8Cntr)
{
    if ( bState )
    {
        if ( ++(*u8Cntr) >= u8Delay )
        {
            *u8Cntr = 0;
            return TRUE;
        }
    }
    else
    {
        *u8Cntr = 0;
    }

    return FALSE;
}


//*****************************************************************************
//
//! \breif Check for timeout.
//!
//! \return True if reaches timeout limit.
//
//*****************************************************************************
bool CheckTimeout(uint32_t *u32TimeoutCntr, uint32_t u32TimeoutLimit)
{
    if ( ++(*u32TimeoutCntr) >= u32TimeoutLimit )
    {
        *u32TimeoutCntr = 0;
        return TRUE;
    }

    return FALSE;
}




bool VerifyFlash(uint32_t u32StartAddr, uint32_t u32EndAddr, uint32_t u32Pattern)
{
    uint32_t u32Addr, u32Data;

    for(u32Addr = u32StartAddr; u32Addr < u32EndAddr; u32Addr += 4)
    {
        u32Data = FMC_Read(u32Addr);
        if(u32Data != u32Pattern)
        {
            // printf("\nFMC_Read data verify failed at address 0x%x, read=0x%x, expect=0x%x\n", u32Addr, u32Data, u32Pattern);
            return FALSE;
        }
    }
    return TRUE;
}


bool EraseAPROMArea(uint32_t u32StartAddr, uint32_t u32EndAddr)
{
    uint32_t u32Addr = 0;

    SYS_UnlockReg();
    FMC_EnableAPUpdate();
    for ( u32Addr = u32StartAddr; u32Addr < u32EndAddr; u32Addr+=FMC_FLASH_PAGE_SIZE )
    {
        FMC_Erase(u32Addr);
        if ( !VerifyFlash(u32Addr, u32Addr + FMC_FLASH_PAGE_SIZE, 0xFFFFFFFF) )
        {
            FMC_DisableAPUpdate();
            return FALSE;
        }
    }

    FMC_DisableAPUpdate();
    SYS_LockReg();
    return TRUE;
}


void WriteAPROM(uint32_t u32StartAddr, uint32_t *pu32Data, uint8_t u8Size)
{
    uint8_t i = 0;
    SYS_UnlockReg();
    FMC_EnableAPUpdate();
    for ( i = 0; i < u8Size; i++ )
    {
        FMC_Write(u32StartAddr + (i*4), *(pu32Data + i));
    }
    FMC_DisableAPUpdate();
    SYS_LockReg();
}



bool SetIAPBoot(void)
{
    uint32_t au32Config[2];
    uint32_t u32CBS;

    /* Read current boot mode */
    u32CBS = ( FMC->ISPSTA & FMC_ISPSTA_CBS_Msk ) >> FMC_ISPSTA_CBS_Pos;
    if ( u32CBS & 1 )
    {
        /* Modify User Configuration when it is not in IAP mode */

        FMC_ReadConfig(au32Config, 2);
        if ( au32Config[0] & 0x40 )
        {
            FMC_EnableConfigUpdate();
            au32Config[0] &= ~0x40;
            FMC_Erase(FMC_CONFIG_BASE);
            if ( FMC_WriteConfig(au32Config, 2) < 0 )
                return FALSE;

            // Perform chip reset to make new User Config take effect
            SYS_ResetChip();
        }
    }

    return TRUE;
}


void ResetToLDROM(void)
{
    SYS_UnlockReg();
    /* Mask all interrupt before changing VECMAP to avoid wrong interrupt handler fetched */
    __set_PRIMASK(1);

    /* Set VECMAP to LDROM for booting from LDROM */
    FMC_SetVectorPageAddr(FMC_LDROM_BASE);

    /* Software reset to boot to LDROM */
    NVIC_SystemReset();

    SYS_LockReg();
}


/*---------------------------------------------------------------------------------------------------------*/
/*  Function for System Entry to Power-down Mode                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void EnterPowerDownMode(void)
{
    // printf("System enter to power-down mode.\n\n");

    /* To check if all the debug messages are finished */
    // while( IsDebugFifoEmpty() == 0 );
    UART_WAIT_TX_EMPTY(UART0);

    SYS_UnlockReg();

    SCB->SCR = 4;

    /* To program PWRCON register, it needs to disable register protection first. */
    CLK->PWRCON = (CLK->PWRCON & ~(CLK_PWRCON_PWR_DOWN_EN_Msk | CLK_PWRCON_PD_WAIT_CPU_Msk)) |
                  CLK_PWRCON_PD_WAIT_CPU_Msk | CLK_PWRCON_PD_WU_INT_EN_Msk;
    CLK->PWRCON |= CLK_PWRCON_PWR_DOWN_EN_Msk;

    __WFI();

    SYS_LockReg();
}


// uint16_t FindNearestValue(uint16_t u16Target, uint16_t u16Unit)
// {
//     uint16_t u16Val = u16Unit, u16ValLow = 0;

//     if ( u16Target <= u16Unit )
//         return u16Unit;

//     while ( u16Val < u16Target )
//     {
//         u16Val += u16Unit;
//     }

//     u16ValLow = u16Val - u16Unit;
//     if ( (u16Val - u16Target) > (u16Target - u16ValLow) )
//     {
//         return u16ValLow;
//     }

//     return u16Val;
// }


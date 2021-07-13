/**************************************************************************//**
 * @file    protections.c
 * $Date:   2021/2/23 09:48 $
 * @brief   Protection functions for charge/discharge process.
 *
 * @author JW Chang
 *
*****************************************************************************/
#include "main.h"
#include "power.h"
#include "protections.h"


#define     PROTECTION_DELAY    20
#define     RELEASE_DELAY       30



static void CheckChargeOVP(void);
static void CheckChargeUVP(void);
static void CheckChargeOCP(void);
static void CheckDischargeOCP(void);
static void CheckChargeTimeout(void);
static void CheckCOF(void);
static void CheckDOF(void);


uint16_t g_u16ChargeProtections = 0;
uint8_t g_u8DischargeProtections = 0;


//*****************************************************************************
//
//! Check for charge protection.
//!
//! \return True if need to protect.
//
//*****************************************************************************
bool CheckChargeProtection(void)
{
    CheckChargeOVP();
    CheckChargeUVP();
    CheckChargeOCP();
    CheckChargeTimeout();
    CheckCOF();

    return (g_u16ChargeProtections) ? TRUE : FALSE;
}



//*****************************************************************************
//
//! Check for discharge protection.
//!
//! \return True if need to protect.
//
//*****************************************************************************
bool CheckDischargeProtection(void)
{
    CheckDischargeOCP();
    CheckDOF();

    return (g_u8DischargeProtections) ? TRUE : FALSE;
}



//*****************************************************************************
//
//! \brief Check if VBAT OVP during charging.
//!
//! \return None.
//
//*****************************************************************************
static void CheckChargeOVP(void)
{
    if ( IS_BIT_CLR(g_u16ChargeProtections, CHG_PROT_BATT_OVP) )
    {
        if ( StateDebounce((g_sAdcData.u16PackVolt > CHARGE_OVP_VOLT), PROTECTION_DELAY, &g_sChargeParam.u8BattOvpCntr) )
        {
            SET_BIT(g_u16ChargeProtections, CHG_PROT_BATT_OVP);
            #if DEBUG_MSG_EN
            printf("OVP\n");
            #endif
        }
    }
    else
    {
        if ( StateDebounce((g_sAdcData.u16PackVolt < CHARGE_OVP_VOLT), RELEASE_DELAY, &g_sChargeParam.u8BattOvpCntr) )
        {
            CLR_BIT(g_u16ChargeProtections, CHG_PROT_BATT_OVP);
        }
    }
}


//*****************************************************************************
//
//! Check if VBAT UVP during charging.
//!
//! \return None.
//
//*****************************************************************************
static void CheckChargeUVP(void)
{
    if ( IS_BIT_CLR(g_u16ChargeProtections, CHG_PROT_BATT_UVP) )
    {
        if ( StateDebounce((g_sAdcData.u16PackVolt < CHARGE_UVP_VOLT), PROTECTION_DELAY, &g_sChargeParam.u8BattUvpCntr) )
        {
            SET_BIT(g_u16ChargeProtections, CHG_PROT_BATT_UVP);
            #if DEBUG_MSG_EN
            printf("UVP\n");
            #endif
        }
    }
    else
    {
        if ( StateDebounce((g_sAdcData.u16PackVolt > CHARGE_UVP_VOLT), RELEASE_DELAY, &g_sChargeParam.u8BattUvpCntr) )
        {
            CLR_BIT(g_u16ChargeProtections, CHG_PROT_BATT_UVP);
        }
    }
}


//*****************************************************************************
//
//! Check charge over current.
//!
//! \return None.
//
//*****************************************************************************
static void CheckChargeOCP(void)
{
    if ( IS_BIT_CLR(g_u16ChargeProtections, CHG_PROT_OCP) )
    {
        if ( StateDebounce((g_sAdcData.u16ChargeCurr > CHARGE_OCP), PROTECTION_DELAY, &g_sChargeParam.u8COCPCntr) )
        {
            SET_BIT(g_u16ChargeProtections, CHG_PROT_OCP);
            #if DEBUG_MSG_EN
            printf("OCP\n");
            #endif
            // UpdateErrorState(ERR_CHARGE_OCP);
        }
    }
    // else
    // {
    //     if ( StateDebounce((g_sAdcData.u16ChargeCurr < CHARGE_OCP), RELEASE_DELAY, &g_sChargeParam.u8COCPCntr) )
    //     {
    //         CLR_BIT(g_u16ChargeProtections, CHG_PROT_OCP);
    //     }
    // }
}



//*****************************************************************************
//
//! Check discharge over current.
//!
//! \return None.
//
//*****************************************************************************
static void CheckDischargeOCP(void)
{
    if ( IS_BIT_CLR(g_u8DischargeProtections, DCHG_PROT_OCP) )
    {
        if ( StateDebounce((g_sAdcData.u16ChargeCurr > DISCHARGE_OCP), PROTECTION_DELAY, &g_sDischargeParam.u8DOCPCntr) )
        {
            SET_BIT(g_u8DischargeProtections, DCHG_PROT_OCP);
            #if DEBUG_MSG_EN
            printf("DOCP\n");
            #endif
        }
    }
}



//*****************************************************************************
//
//! Check for precharge and fast charge timeout and set flags.
//!
//! \return True if charge timeout occurs.
//
//*****************************************************************************
static void CheckChargeTimeout(void)
{
    if ( g_eChargeState == CHG_STATE_FASTCHARGE )
    {
        if ( IS_BIT_CLR(g_u16ChargeProtections, CHG_PROT_TIMEOUT) )
        {
            if ( CheckTimeout(&g_sChargeParam.u32ChargeTimeCntr, FAST_CHARGE_TIMEOUT) )
            {
                SET_BIT(g_u16ChargeProtections, CHG_PROT_TIMEOUT);
                #if DEBUG_MSG_EN
                printf("Charge timeout\n");
                #endif
            }
        }
    }
    else
    {
        g_sChargeParam.u32ChargeTimeCntr = 0;
    }
}



static void CheckCOF(void)
{
    if ( IS_BIT_CLR(g_u16ChargeProtections, CHG_PROT_COF) )
    {
        if ( StateDebounce(g_sAdcData.u16COF, PROTECTION_DELAY, &g_sChargeParam.u8COFCntr) )
        {
            SET_BIT(g_u16ChargeProtections, CHG_PROT_COF);
        }
    }
    else
    {
        if ( StateDebounce(!g_sAdcData.u16COF, RELEASE_DELAY, &g_sChargeParam.u8COFCntr) )
        {
            CLR_BIT(g_u16ChargeProtections, CHG_PROT_COF);
        }
    }
}


static void CheckDOF(void)
{
    if ( IS_BIT_CLR(g_u8DischargeProtections, DCHG_PROT_DOF) )
    {
        if ( StateDebounce(g_sAdcData.u16DOF, PROTECTION_DELAY, &g_sDischargeParam.u8DOFCntr) )
        {
            SET_BIT(g_u8DischargeProtections, DCHG_PROT_DOF);
        }
    }
    else
    {
        if ( StateDebounce(!g_sAdcData.u16DOF, RELEASE_DELAY, &g_sDischargeParam.u8DOFCntr) )
        {
            CLR_BIT(g_u8DischargeProtections, DCHG_PROT_DOF);
        }
    }
}


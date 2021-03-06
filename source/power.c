/**************************************************************************//**
 * @file    power.c
 * $Date:   2021/02/23 09:48 $
 * @brief   Source file of voltage monitor and charge/discharge control.
 *
 * @author JW Chang
 *
*****************************************************************************/
#include "main.h"
#include "power.h"
#include "battery.h"
#include "adc_func.h"
// #include "protections.h"
#include "init.h"
#include "relay.h"



// #define     RSENSE          10
// #define     OP_GAIN         50



static void ChargeProcess(void);
static void DischargeProcess(void);
static void IdleProcess(void);
#if LOW_POWER_MODE_EN
static void PowerDownSequence(void);
static void WakeupSequence(void);
#endif  // LOW_POWER_MODE_EN
static void ResetChargeParameters(void);
static void ResetDischargeParameters(void);
static uint16_t ConvertCurrent(uint16_t u16AdcData, float fRsense);
static uint16_t ConvertVoltage(uint16_t u16Val);
static uint16_t ConvertRelayCurrent(uint16_t u16Val);

sAdcValue_t g_sAdcData = {0};                       //!< Stores all ADC values.
eDCInputState_t g_eDCInputState = DC_INIT;          //!< Adapter input state.

sChargeParam_t g_sChargeParam = {0};                //!< Stores all charge related variables.

eDischargeState_t g_eDischargeState = DCHG_STATE_DISCHARGE; //!< Discharging state.
sDischargeParam_t g_sDischargeParam = {0};          //!< Stores all discharge related variables.

uint8_t g_u8PowerStateDelay = 0;//, g_u8WakeCntr = 0;

//*****************************************************************************
//
//! \brief  Read all ADC values.
//!
//! \return None.
//
//*****************************************************************************
void UpdateAllAdcData(void)
{
    static uint8_t u8RelayCurrDelay = 0;
    uint16_t u16Val = 0;

    u16Val = ADC_Read(ADC_CHG_CURR);
    g_sAdcData.u16ChargeCurr = ConvertCurrent(u16Val, 0.013);
    u16Val = ADC_Read(ADC_DCHG_CURR);
    g_sAdcData.u16DischargeCurr = ConvertCurrent(u16Val, 0.01);

    //
    // Calculate relay current and check if is discharging (>10A)
    //
    if ( u8RelayCurrDelay )
    {
        u8RelayCurrDelay--;
    }
    else
    {
        if ( IS_BIT_CLR(g_u16SystemFlags, SYS_FLAG_RELAY_CURR_DETECTED) )
        {
            u16Val = ADC_Read(ADC_RELAY_CURR);
            g_sAdcData.u16RelayCurr = ConvertCurrent(u16Val, 0.0004);
            if ( g_sAdcData.u16RelayCurr > 10000 )
            {
                SET_BIT(g_u16SystemFlags, SYS_FLAG_RELAY_CURR_DETECTED);
                u8RelayCurrDelay = 50;
            }
        }
    }

    // g_sAdcData.u16PackVolt = ADC_Read(ADC_SV_PACK);
    u16Val = ADC_Read(ADC_SV_PACK);
    g_sAdcData.u16PackVolt = ConvertVoltage(u16Val);

    u16Val = ADC_Read(ADC_SV_AD);
    g_sAdcData.u16AdapterVolt = ConvertVoltage(u16Val);


    u16Val = ADC_Read(ADC_SV_RELAY);
    // g_sAdcData.u16RelayVolt = ConvertVoltage(u16Val);

    if ( u16Val >= 248 ){ //for 0.3V offset
        g_sAdcData.u16RelayVolt = ConvertRelayCurrent(u16Val);
    }else{
        g_sAdcData.u16RelayVolt = 0;
    }
    // g_sAdcData.u16RelayVolt = ADC_Read(ADC_SV_RELAY);

    //g_sAdcData.u16DOF = ADC_Read(ADC_DOF);
    //g_sAdcData.u16COF = ADC_Read(ADC_COF);
}



//*****************************************************************************
//
//! \brief  Check AC insertion state.
//!
//! \return None.
//
//*****************************************************************************
void CheckACState(void)
{
    static uint8_t u8AcDetCntr = 0, u8ChgCurrCntr = 0;

    // Debounce for AC_IN signal.
    if ( IS_BIT_CLR(g_u16SystemState, SYS_STAT_AC_OK) )
    {
        if ( StateDebounce(AC_ACTIVE(), 10, &u8AcDetCntr) )
        {
            SET_BIT(g_u16SystemState, SYS_STAT_AC_OK);
        }
    }
    else
    {
        if ( StateDebounce(AC_INACTIVE(), 10, &u8AcDetCntr) )
        {
            CLR_BIT(g_u16SystemState, SYS_STAT_AC_OK);
        }
    }

    // Debounce for charge current.
    if ( IS_BIT_CLR(g_u16SystemState, SYS_STAT_CHG_CURR) )
    {
        if ( StateDebounce(g_sAdcData.u16ChargeCurr > 200, 10, &u8ChgCurrCntr) )
        {
            SET_BIT(g_u16SystemState, SYS_STAT_CHG_CURR);
        }
    }
    else
    {
        if ( StateDebounce(g_sAdcData.u16ChargeCurr <= 200, 10, &u8ChgCurrCntr) )
        {
            CLR_BIT(g_u16SystemState, SYS_STAT_CHG_CURR);
        }
    }
}

//*****************************************************************************
//
//! \brief  Charge/Discharge/Idle process control.
//!
//! \return None.
//
//*****************************************************************************
void PowerControl(void)
{
    #if RELAY_DETECTION
    static uint8_t u8RelayDetectCntr = 0;
    // bool bDischarging = FALSE;
    #else
    static uint16_t u16SleepCntr = 0;
    #endif

    if ( g_u8PowerStateDelay < 5 )
    {
        g_u8PowerStateDelay++;
        #if DEBUG_MSG_EN
        printf("-");
        #endif
        return;
    }

    CheckACState();

    // if ( StateDebounce(g_sAdcData.u16ChargeCurr > 20, 10, &u8ChargeCntr) )
    if ( IS_BIT_SET(g_u16SystemState, SYS_STAT_AC_OK) )
    {
        ChargeProcess();
        g_bRelayTurnOn = FALSE;
        u8RelayDetectCntr = 0;
    }
    else
    {
        #if RELAY_DETECTION
        if ( g_bRelayTurnOn || (g_sAdcData.u16DischargeCurr > 80))//|| IS_BIT_SET(g_u16SystemFlags, SYS_FLAG_SW_1W_LED) )
        {
            DischargeProcess();

			if ( ++u8RelayDetectCntr >= 200 )
            {
                u8RelayDetectCntr = 0;
                g_bRelayTurnOn = FALSE;
                CLR_BIT(g_u16SystemFlags, SYS_FLAG_DCHG_EN);
            }
        }
        else
        {
            if ( /*IS_BIT_CLR(g_u16SystemFlags, SYS_FLAG_SW_1W_LED) && */
                (g_sAdcData.u16DischargeCurr < 80) )
            {
                IdleProcess();
            }

            if ( IS_BIT_SET(g_u16SystemFlags, SYS_FLAG_DCHG_EN) )       // Set by pressing switch 5s
            {
                g_bRelayTurnOn = TRUE;
            }
            else
            {
                // Allow turn on relay when pack > 16V and relay > 2V
                if ( (g_sAdcData.u16RelayVolt >= 2000)  && (g_sAdcData.u16PackVolt >= 16000) )
                {
                    if ( ++u8RelayDetectCntr >= 10 )    // Delay 1 sec before turn on relay.
                    {
                        g_bRelayTurnOn = TRUE;
                        u8RelayDetectCntr = 0;
                    }
                }
                else
                {
                    u8RelayDetectCntr = 0;
                }
            }
        }
        #else
        if ( IS_BIT_SET(g_u16SystemFlags, SYS_FLAG_DCHG_EN) )
        {
            g_bRelayTurnOn = TRUE;
            DischargeProcess();

            // Enter idle mode if discharge current is less than 20mA(TBD) for over 1 min
            if ( g_sAdcData.u16DischargeCurr < 20 )
            {
                if ( ++u16SleepCntr >= 600 )
                {
                    CLR_BIT(g_u16SystemFlags, SYS_FLAG_DCHG_EN);
                }
            }
            else
            {
                u16SleepCntr = 0;
            }
        }
        else
        {
            g_bRelayTurnOn = FALSE;
            IdleProcess();
        }
        #endif
    }
}



//*****************************************************************************
//
//! \brief  Charging process.
//!
//! \return None.
//
//*****************************************************************************
static void ChargeProcess(void)
{
    static bool bStatus = FALSE;

    if ( IS_BIT_CLR(g_u16SystemState, SYS_STAT_CHARGE) )
    {
        #if DEBUG_MSG_EN
        printf("Charge\n");
        #endif
        SET_BIT(g_u16SystemState, SYS_STAT_CHARGE);
        CLR_BIT(g_u16SystemState, SYS_STAT_DISCHARGE);
        ResetChargeParameters();
        // g_u16ChargeProtections = 0;
        // g_bRelayTurnOn = FALSE;
    }

//    if ( CheckChargeProtection() )
//    {
//        g_eChargeState = CHG_STATE_FAULT;
//    }

    if ( IS_BIT_CLR(g_u8BattFlags, BATT_FLAG_FC) )
    {
        bStatus = ((g_sAdcData.u16PackVolt >= FULL_VOLTAGE) && (g_sAdcData.u16ChargeCurr <= TAPER_CURRENT)) ? TRUE : FALSE;
        if ( StateDebounce(bStatus, 10, &g_sChargeParam.u8FullRlsCntr) )
        {
            // g_eChargeState = CHG_STATE_BATFULL;
            SET_BIT(g_u8BattFlags, BATT_FLAG_FC);
            #if DEBUG_MSG_EN
            printf("Fully charged\n");
            #endif
        }
    }
}



//*****************************************************************************
//
//! \brief  Discharging process.
//!
//! \return None.
//
//*****************************************************************************
static void DischargeProcess(void)
{
    // static bool bStatus = FALSE;

    if ( IS_BIT_CLR(g_u16SystemState, SYS_STAT_DISCHARGE) )
    {
        #if DEBUG_MSG_EN
        printf("Discharge\n");
        #endif
        SET_BIT(g_u16SystemState, SYS_STAT_DISCHARGE);
        CLR_BIT(g_u16SystemState, SYS_STAT_CHARGE);
        ResetDischargeParameters();
    }

    if ( g_sAdcData.u16PackVolt < FULL_VOLTAGE )
    {
        CLR_BIT(g_u8BattFlags, BATT_FLAG_FC);
    }

}



//*****************************************************************************
//
//! \brief  Idle process.
//!
//! \return None.
//
//*****************************************************************************
static void IdleProcess(void)
{
    static uint8_t u8IdleCntr = 0;

    if ( IS_BIT_SET(g_u16SystemState, SYS_STAT_CHARGE | SYS_STAT_DISCHARGE) )
    {
        #if DEBUG_MSG_EN
        printf("Idle\n");
        #endif
        CLR_BIT(g_u16SystemState, SYS_STAT_CHARGE | SYS_STAT_DISCHARGE);
        u8IdleCntr = 0;
    }

    if ( u8IdleCntr < 30 )
    {
        u8IdleCntr++;
    }
    else
    {
        #if LOW_POWER_MODE_EN
        // Don't enter LPM if switch is still pressed.
        if ( SW_DIS_INACTIVE() && SW_1WLED_INACTIVE() && !g_bRelayTurnOn)
        {
            //Enter low power mode.
            PowerDownSequence();
            SET_BIT(g_u16SystemFlags, SYS_FLAG_SLEEP);
            // EnterPowerDownMode();

            // Wake up
            // WakeupSequence();
        }

        u8IdleCntr = 0;
        #endif  // LOW_POWER_MODE_EN
    }
}


#if LOW_POWER_MODE_EN
//*****************************************************************************
//
//! \brief  Handles behaviors under sleep mode.
//!
//! \return None.
//
//*****************************************************************************
void SleepModeHandler(void)
{
    //Enter low power mode.
    EnterPowerDownMode();

    if ( IS_BIT_SET(g_u16SystemFlags, SYS_FLAG_WAKEUP) )
    {
        CLR_BIT(g_u16SystemFlags, SYS_FLAG_WAKEUP | SYS_FLAG_SLEEP);
        WakeupSequence();
    }
}



//*****************************************************************************
//
//! \brief  Power down sequence.
//!
//! \return None.
//
//*****************************************************************************
static void PowerDownSequence(void)
{
    TIMER_Close(TIMER0);
    // Timer1_Init();
    // ADC_Close(ADC);
    RY_NO_OFF();
    RY_NC_OFF();

    #if DEBUG_MSG_EN
    UART_WAIT_TX_EMPTY(UART0);
    UART_Close(UART0);
    #endif

    EN_5V_OFF();
    EN_12V_OFF();
    LED_C25_OFF();
    LED_C50_OFF();
    LED_C75_OFF();
    LED_C100_OFF();
    LED_RED_OFF();
    LED_GREEN_OFF();
    LED_1W_OFF();
}



//*****************************************************************************
//
//! \brief  Wake-up sequence.
//!
//! \return None.
//
//*****************************************************************************
static void WakeupSequence(void)
{
    EN_5V_ON();
    EN_12V_ON();

    #if DEBUG_MSG_EN
    UART0_Init();
    #endif

    GPIO_Init();

    Timer0_Init();
    TIMER_Close(TIMER1);

    g_u8PowerStateDelay = 0;

}
#endif  // LOW_POWER_MODE_EN


//*****************************************************************************
//
//! Reset charge related variables.
//!
//! \return None.
//
//*****************************************************************************
static void ResetChargeParameters(void)
{
    // g_u16ChargeProtections = 0;
    memset(&g_sChargeParam, 0, sizeof(sChargeParam_t));
}


//*****************************************************************************
//
//! Reset discharge related variables.
//!
//! \return None.
//
//*****************************************************************************
static void ResetDischargeParameters(void)
{
    // g_u8DischargeProtections = 0;
    memset(&g_sDischargeParam, 0, sizeof(sDischargeParam_t));
}




//*****************************************************************************
//
//! \breif Convert ADC reading to actual current.
//!        Input ADC reading and R-Sense value in Ohm
//!
//! \return Converted current data in mA.
//
//*****************************************************************************
static uint16_t ConvertCurrent(uint16_t u16AdcData, float fRsense)
{
    float fVal = 0;

    // AdcData * 5000(mV) / 4096 / 50(op gain) / R-Sense(Ohm)
    // = AdcData * 100 / 4096 / R-Sense
    fVal = (float)u16AdcData * 25 / 1024 / fRsense;

    return (uint16_t)fVal;
}

//*****************************************************************************
//
//! \breif Convert ADC reading to Relay actual current.
//!
//! \return Converted current data in mA.
//
//*****************************************************************************
/*static uint16_t ConvertRelayCurrent(uint16_t u16Val)
{
    float fVal = 0;

    // AdcData * 5000 / 4096 / 50 / 0.01
    // --> * 100 / 4096 * 100
    // --> * 10000 / 4096
    fVal = (float)u16Val * 8138 / 100;

    return (uint16_t)fVal;
}
*/

//*****************************************************************************
//
//! \breif Convert ADC reading to original voltage.
//!
//! \return Converted voltage data in mV.
//
//*****************************************************************************
static uint16_t ConvertVoltage(uint16_t u16Val)
{
    float fVal = 0;

    // AdcData / 4096 * 5000 * (560 + 100) / 100
    // --> * 50 * 660 / 4096
    // --> * 33000 / 4096
    fVal = (float)u16Val * 4125 / 512;

    return (uint16_t)fVal;
}

static uint16_t ConvertRelayCurrent(uint16_t u16Val)
{
    float fVal = 0;

    // AdcData / 4096 * 5000 * (560 + 100) / 100
    // --> * 50 * 660 / 4096
    // --> * 33000 / 4096
    fVal = (float)(u16Val + 492) * 4125 / 512;

    return (uint16_t)fVal;
}


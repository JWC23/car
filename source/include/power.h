/**************************************************************************//**
 * @file    power.h
 * $Date:   2021/2/23 09:48 $
 * @brief   Power control header file.
 *
 * @author JW Chang
 *
*****************************************************************************/
#ifndef POWER_H_
#define POWER_H_


typedef struct sAdcValue
{
    uint16_t u16ChargeCurr;
    uint16_t u16DischargeCurr;
    uint16_t u16RelayCurr;
    uint16_t u16PackVolt;
    uint16_t u16AdapterVolt;
    uint16_t u16RelayVolt;
    uint16_t u16DOF;
    uint16_t u16COF;
}sAdcValue_t;

extern sAdcValue_t g_sAdcData;



typedef enum eDCInputState
{
    DC_INIT,
    DC_OFF,
    DC_UVP,
    DC_NORMAL,
    DC_OVP
}eDCInputState_t;

extern eDCInputState_t g_eDCInputState;



// typedef enum eChargeState
// {
//     CHG_STATE_INIT,
//     CHG_STATE_FASTCHARGE,
//     CHG_STATE_BATFULL,
//     CHG_STATE_FAULT
// }eChargeState_t;

// extern eChargeState_t g_eChargeState;



typedef enum eDischargeState
{
    DCHG_STATE_DISCHARGE,
    // DCHG_STATE_SLEEP,
    DCHG_STATE_FAULT
}eDischargeState_t;

extern eDischargeState_t g_eDischargeState;



typedef struct sChargeParam
{
    uint32_t u32ChargeTimeCntr;
    uint8_t u8FullRlsCntr;
    uint8_t u8COCPCntr;
    uint8_t u8BattOvpCntr;
    uint8_t u8BattUvpCntr;
    uint8_t u8COFCntr;
}sChargeParam_t;

extern sChargeParam_t g_sChargeParam;



typedef struct sDischargeParam
{
    uint8_t u8DOCPCntr;
    uint8_t u8BattUvpCntr;
    uint8_t u8DOFCntr;
}sDischargeParam_t;

extern sDischargeParam_t g_sDischargeParam;





extern void UpdateAllAdcData(void);
extern void PowerControl(void);
extern void SleepModeHandler(void);


#endif

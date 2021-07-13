/**************************************************************************//**
 * @file    battery.h
 * $Date:   2021/2/23 09:48 $
 * @brief   Battery related function headers.
 *
 * @author JW Chang
 *
*****************************************************************************/
#ifndef BATTERY_H_
#define BATTERY_H_

#define     DESIGN_CAPACITY         7461    // mAh
#define     FULL_VOLTAGE            28000
#define     RECHARGE_VOLTAGE        24000
#define     UVP_VOLTAGE             18466

#define     TAPER_CURRENT           200

// Use 340A, 5 secs to start car, 3400000(mA)/3600 * 5(s) = 472
#define     START_CAR_CAPACITY      472




typedef struct sBatteryData
{
    uint8_t u8Rsoc;
    int16_t i16Current;
    uint16_t u16FullChgCap;
    float fRemainCap;
}sBatteryData_t;

extern sBatteryData_t g_sBatteryData;



extern uint8_t g_u8BattFlags;
enum eBattFlags
{
    BATT_FLAG_INIT          = MASK(0),
    BATT_FLAG_FC            = MASK(1),
    BATT_FLAG_FD            = MASK(2),
    BATT_FLAG_3             = MASK(3),
    BATT_FLAG_4             = MASK(4),
    BATT_FLAG_5             = MASK(5),
    BATT_FLAG_6             = MASK(6),
    BATT_FLAG_7             = MASK(7)
};



extern void Batt_UpdateRsocByOCV(void);


#endif


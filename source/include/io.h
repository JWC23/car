/**************************************************************************//**
 * @file    gpio.h
 * $Date:   2021/2/22 15:10 $
 * @brief   Define GPIO, ADC ports and pins.
 *
 * @author JW Chang
 *
*****************************************************************************/
#ifndef GPIO_H_
#define GPIO_H_


#define     P00_SW_DIS                  P0, BIT0        //!< switch for discharge and display capacity
#define     SW_DIS_ACTIVE()             (P00 == 0)
#define     SW_DIS_INACTIVE()           (P00 == 1)

#define     P01_SW_1WLED                P0, BIT1        //!< 1W LED switch
#define     SW_1WLED_ACTIVE()           (P01 == 0)
#define     SW_1WLED_INACTIVE()         (P01 == 1)

#define     P06_TXRX_EN                 P0, BIT6
#define     TXRX_ENABLE()               (P06 = 0)
#define     TXRX_DISABLE()              (P06 = 1)
#define     TXRX_EN_STAT()              (P06 == 1)

#define     P07_CTLD2_EN                P0, BIT7
#define     CTLD2_EN_HIGH()             (P07 = 1)
#define     CTLD2_EN_LOW()              (P07 = 0)


#define     P10_AD_CHARGE_CURR          P1, BIT0        //!< AIN0, charging current.
#define     AD_INACTIVE()               (P10 == 0)

#define     P11_AD_DISCHARGE_CURR       P1, BIT1        //!< AIN1, discharging current.
#define     P12_AD_RELAY_CURR           P1, BIT2        //!< AIN2, relay current.
#define     P13_SD_PACK                 P1, BIT3        //!< AIN3, pack voltage.
#define     P14_SD_ADAPTER              P1, BIT4        //!< AIN4, adapter input voltage.
#define     P15_SD_RELAY                P1, BIT5        //!< AIN5, relay voltage.
#define     P16_DOF                     P1, BIT6        //!< AIN6, DOF
#define     P17_COF                     P1, BIT7        //!< AIN7, COF

#define     P20_LED_C25                 P2, BIT0        //!< 25% capacity LED
#define     LED_C25_ON()                (P20 = 0)
#define     LED_C25_OFF()               (P20 = 1)
#define     LED_C25_TOGGLE()            (P20 ^= 1)
#define     LED_C25_ACTIVE()            (P20 == 0)

#define     P21_LED_C50                 P2, BIT1        //!< 50% capacity LED
#define     LED_C50_ON()                (P21 = 0)
#define     LED_C50_OFF()               (P21 = 1)
#define     LED_C50_TOGGLE()            (P21 ^= 1)
#define     LED_C50_ACTIVE()            (P21 == 0)

#define     P22_LED_C75                 P2, BIT2        //!< 75% capacity LED
#define     LED_C75_ON()                (P22 = 0)
#define     LED_C75_OFF()               (P22 = 1)
#define     LED_C75_TOGGLE()            (P22 ^= 1)
#define     LED_C75_ACTIVE()            (P22 == 0)

#define     P23_LED_C100                P2, BIT3        //!< 100% capacity LED
#define     LED_C100_ON()               (P23 = 0)
#define     LED_C100_OFF()              (P23 = 1)
#define     LED_C100_TOGGLE()           (P23 ^= 1)
#define     LED_C100_ACTIVE()           (P23 == 0)

#define     P24_LED_RED                 P2, BIT4        //!< Red LED
#define     LED_RED_ON()                (P24 = 1)
#define     LED_RED_OFF()               (P24 = 0)
#define     LED_RED_TOGGLE()            (P24 ^= 1)
#define     LED_RED_ACTIVE()            (P24 == 0)

#define     P25_LED_GREEN               P2, BIT5        //!< Green LED
#define     LED_GREEN_ON()              (P25 = 1)
#define     LED_GREEN_OFF()             (P25 = 0)
#define     LED_GREEN_TOGGLE()          (P25 ^= 1)
#define     LED_GREEN_ACTIVE()          (P25 == 0)

#define     P26_LED_1W                  P2, BIT6        //!< 1W LED
#define     LED_1W_ON()                 (P26 = 0)
#define     LED_1W_OFF()                (P26 = 1)
#define     LED_1W_TOGGLE()             (P26 ^= 1)
#define     LED_1W_ACTIVE()             (P26 == 0)


#define     P32_CTLC2_EN                P3, BIT2
#define     CTLC2_EN_HIGH()             (P32 = 1)
#define     CTLC2_EN_LOW()              (P32 = 0)

#define     P33_EN_12V                  P3, BIT3        //!< 24V enable pin
#define     EN_12V_ON()                 (P33 = 0)
#define     EN_12V_OFF()                (P33 = 1)
#define     EN_12V_TOGGLE()             (P33 ^= 1)
#define     EN_12V_ACTIVE()             (P33 == 1)

#define     P34_EN_5V                   P3, BIT4        //!< 5V enable pin
#define     EN_5V_ON()                  (P34 = 0)
#define     EN_5V_OFF()                 (P34 = 1)
#define     EN_5V_TOGGLE()              (P34 ^= 1)
#define     EN_5V_ACTIVE()              (P34 == 1)

#define     P35_RY_NO                   P3, BIT5        //!< RY NO
#define     RY_NO_ON()                  (P35 = 1)
#define     RY_NO_OFF()                 (P35 = 0)
#define     RY_NO_TOGGLE()              (P35 ^= 1)
#define     RY_NO_ISHIGH()              (P35 == 1)
#define     RY_NO_ISLOW()               (P35 == 0)

#define     P36_RY_NC                   P3, BIT6        //!< RY NC
#define     RY_NC_ON()                  (P36 = 1)
#define     RY_NC_OFF()                 (P36 = 0)
#define     RY_NC_TOGGLE()              (P36 ^= 1)
#define     RY_NC_ISHIGH()              (P36 == 1)
#define     RY_NC_ISLOW()               (P36 == 0)


#define     P40_OP_EN1                  P4, BIT0
#define     OP_EN1_ENABLE()             (P40 = 1)
#define     OP_EN1_DISABLE()            (P40 = 0)

#define     P41_OP_EN2                  P4, BIT1
#define     OP_EN2_ENABLE()             (P41 = 1)
#define     OP_EN2_DISABLE()            (P41 = 0)

#define     P42_OP_EN3                  P4, BIT2
#define     OP_EN3_ENABLE()             (P42 = 1)
#define     OP_EN3_DISABLE()            (P42 = 0)


// #define     DISCHARGE_EN()              {RY_NO_ON();RY_NC_OFF();}
// #define     DISCHARGE_DIS()             {RY_NO_OFF();RY_NC_ON();}
// #define     DISCHARGE_ACTIVE()          (RY_NO_ISHIGH() || RY_NC_ISLOW())


#define     ADC_CHG_CURR                0               //!< AIN0, charging current.
#define     ADC_DCHG_CURR               1               //!< AIN1, discharging current.
#define     ADC_RELAY_CURR              2               //!< AIN2, relay current.
#define     ADC_SV_PACK                 3               //!< AIN3, pack voltage.
#define     ADC_SV_AD                   4               //!< AIN4, adapter input voltage.
#define     ADC_SV_RELAY                5               //!< AIN5, relay voltage.
#define     ADC_DOF                     6               //!< AIN6
#define     ADC_COF                     7               //!< AIN7


#endif

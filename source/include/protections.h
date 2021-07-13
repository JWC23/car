/**************************************************************************//**
 * @file    protections.h
 * $Date:   2021/2/23 09:48 $
 * @brief   Utility function headers.
 *
 * @author JW Chang
 *
*****************************************************************************/
#ifndef PROTECTION_H_
#define PROTECTION_H_


extern uint16_t g_u16ChargeProtections;
enum eChargeProtection
{
    CHG_PROT_BATT_UVP       = MASK(0),
    CHG_PROT_BATT_OVP       = MASK(1),
    CHG_PROT_OTP            = MASK(2),
    CHG_PROT_OCP            = MASK(4),
    CHG_PROT_TIMEOUT        = MASK(5),
    CHG_PROT_INPUT_OVP      = MASK(6),
    CHG_PROT_INPUT_UVP      = MASK(7),
    CHG_PROT_COF            = MASK(8),
    CHG_PROT_09             = MASK(9)
};


extern uint8_t g_u8DischargeProtections;
enum eDischargeProtection
{
    DCHG_PROT_BATT_UVP      = MASK(0),
    DCHG_PROT_OCP           = MASK(1),
    DCHG_PROT_DOF           = MASK(2),
    DCHG_PROT_03            = MASK(3),
    DCHG_PROT_04            = MASK(4),
    DCHG_PROT_05            = MASK(5),
    DCHG_PROT_06            = MASK(6),
    DCHG_PROT_07            = MASK(7)
};


extern bool CheckChargeProtection(void);
extern bool CheckDischargeProtection(void);


#endif


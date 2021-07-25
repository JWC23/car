/**************************************************************************//**
 * @file    utils.h
 * $Date:   2021/02/22 15:10 $
 * @brief   Utility function headers.
 *
 * @author JW Chang
 *
*****************************************************************************/
#ifndef UTILS_H_
#define UTILS_H_

// #include "fmc.h"



extern bool StateDebounce(bool bState, uint8_t u8Delay, uint8_t *u8Cntr);
extern bool CheckTimeout(uint32_t *u32TimeoutCntr, uint32_t u32TimeoutLimit);

extern bool VerifyFlash(uint32_t u32StartAddr, uint32_t u32EndAddr, uint32_t u32Pattern);
extern bool EraseAPROMArea(uint32_t u32StartAddr, uint32_t u32EndAddr);
extern void WriteAPROM(uint32_t u32StartAddr, uint32_t *pu32Data, uint8_t u8Size);
extern bool SetIAPBoot(void);
extern bool SetBootFromLDROM(void);
extern void ResetToLDROM(void);
extern void EnterPowerDownMode(void);
// extern uint16_t FindNearestValue(uint16_t u16Target, uint16_t u16Unit);


#endif

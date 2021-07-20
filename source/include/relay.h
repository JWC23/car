/**************************************************************************//**
 * @file    relay.h
 * $Date:   2021/4/13 15:00 $
 * @brief   Relay control header file.
 *
 * @author JW Chang
 *
*****************************************************************************/
#ifndef RELAY_H_
#define RELAY_H_



extern bool g_bRelayTurnOn;


extern void RelayVar_Init(void);
extern void RelayControl(void);
extern void CheckResetRelayCounter(void);



#endif

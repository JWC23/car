#ifndef UART_FUNC_H_
#define UART_FUNC_H_

#define     UART_BUFF_SIZE      128



extern uint8_t g_au8UartRxBuff[UART_BUFF_SIZE], g_u8UartRxIdx, g_u8UartCmdInt;



extern void UartRxHandler(void);

#endif

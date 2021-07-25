#include "main.h"
#include "uart_func.h"


uint8_t g_au8UartRxBuff[UART_BUFF_SIZE] = {0}, g_u8UartRxIdx = 0, g_u8UartCmdInt = 0;


void UartRxHandler(void)
{
    if ( !g_u8UartCmdInt )
        return;
    
    
    if ( g_au8UartRxBuff[0] == 'r' && g_au8UartRxBuff[1] == 's' && g_au8UartRxBuff[2] == 't' )
    {
        printf("Reset\n");
        // ResetToLDROM();
        SetBootFromLDROM();
        WDT_Open(WDT_TIMEOUT_2POW4, WDT_RESET_DELAY_3CLK, TRUE, FALSE);
        while(1);
    }
    
    g_u8UartCmdInt = 0;
    g_u8UartRxIdx = 0;
}




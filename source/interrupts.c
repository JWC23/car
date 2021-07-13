/**************************************************************************//**
 * @file    interrupts.c
 * $Date:   2021/02/22 15:10 $
 * @brief   Interrupt service routine source file
 *
 * @author JW Chang
 *
*****************************************************************************/
#include "main.h"
#include "adc_func.h"
#include "led.h"
#include "switch.h"


static uint8_t g_u8TimerCntr = 0;      //!< Timer counter, add 1 when interrupt occurs.
static uint8_t g_u8UARTOnOffCntr = 0;


//*****************************************************************************
//
//! \brief  Timer 0 Interrupt Service Routine
//!
//! \return None.
//
//*****************************************************************************
void TMR0_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER0) == 1)
    {
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);

        SET_BIT(g_u16SystemFlags, SYS_FLAG_100MS);

        Switch_1WDetect();
        Switch_DischargeControl();
        LED_Control();

        if ( ++g_u8TimerCntr >= 10 )
        {
            g_u8TimerCntr = 0;
            SET_BIT(g_u16SystemFlags, SYS_FLAG_1SEC);
        }

        if ( IS_BIT_SET(g_u16SystemState, SYS_STAT_UART_ENABLE) )
        {
            if ( ++g_u8UARTOnOffCntr >= 100 )
            {
                g_u8UARTOnOffCntr = 0;
                // CLR_BIT(g_u16SystemState, SYS_STAT_UART_ENABLE);
            }
            // TXRX_ENABLE();
            LED_GREEN_TOGGLE();
        }
        else
        {
            TXRX_DISABLE();
            LED_GREEN_OFF();
        }

    }
}



//*****************************************************************************
//
//! \brief  ADC Interrupt Service Routine
//!
//! \return None.
//
//*****************************************************************************
void ADC_IRQHandler(void)
{
    g_u8AdcIntFlag = 1;
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT); /* clear the A/D conversion flag */
}



//*****************************************************************************
//
//! \brief  Port0/Port1 IRQ
//!
//! \return None.
//!
//! \details The Port0/Port1 default IRQ, declared in startup_NUC029xAN.s.
//
//*****************************************************************************
void GPIOP0P1_IRQHandler(void)
{
    /* To check if P0.0, P0.1 interrupt occurred */
    if ( GPIO_GET_INT_FLAG(P0, BIT0) || GPIO_GET_INT_FLAG(P0, BIT1) )
    {
        GPIO_CLR_INT_FLAG(P0, BIT0);
        GPIO_CLR_INT_FLAG(P0, BIT1);
        // GPIO_CLR_INT_FLAG(P1, BIT0);
        // GPIO_DisableInt(P0, 0);
        // GPIO_DisableInt(P0, 1);
        // GPIO_DisableInt(P1, 0);
    }
    else
    {
        /* Un-expected interrupt. Just clear all PORT0, PORT1 interrupts */
        P0->ISRC = P0->ISRC;
        P1->ISRC = P1->ISRC;
        // printf("Un-expected interrupts.\n");
    }
}


/**************************************************************************//**
 * @file    init.c
 * $Date:   2021/1/22 15:10 $
 * @brief   NUC029xAN initialization source file
 *
 * @author JW Chang
 *
*****************************************************************************/
#include "main.h"
#include "init.h"


#define PLLCON_SETTING      CLK_PLLCON_50MHz_HIRC        //!< 50Mhz PLL clock
#define PLL_CLOCK           50000000



//*****************************************************************************
//
//! \brief  System clock and GPIO multi-function initialization.
//!
//! \return None.
//
//*****************************************************************************
void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable IRC22M clock */
    // CLK->PWRCON |= CLK_PWRCON_IRC22M_EN_Msk;

    // /* Waiting for IRC22M clock ready */
    // CLK_WaitClockReady(CLK_CLKSTATUS_IRC22M_STB_Msk);

    // /* Switch HCLK clock source to HIRC */
    // CLK->CLKSEL0 = CLK_CLKSEL0_HCLK_S_HIRC;

    // /* Set PLL to Power-down mode and PLL_STB bit in CLKSTATUS register will be cleared by hardware.*/
    // CLK->PLLCON |= CLK_PLLCON_PD_Msk;

    // /* Enable external 12 MHz XTAL, IRC10K */
    // CLK->PWRCON |= CLK_PWRCON_XTL12M_EN_Msk | CLK_PWRCON_OSC10K_EN_Msk;

    // /* Enable PLL and Set PLL frequency */
    // CLK->PLLCON = PLLCON_SETTING;

    // /* Waiting for clock ready */
    // CLK_WaitClockReady(CLK_CLKSTATUS_PLL_STB_Msk | CLK_CLKSTATUS_XTL12M_STB_Msk | CLK_CLKSTATUS_IRC10K_STB_Msk);

    // /* Switch HCLK clock source to PLL, STCLK to HCLK/2 */
    // CLK->CLKSEL0 = CLK_CLKSEL0_STCLK_S_HCLK_DIV2 | CLK_CLKSEL0_HCLK_S_PLL;

    // /* Enable peripheral clock */
    // CLK->APBCLK = CLK_APBCLK_UART0_EN_Msk |
    //               CLK_APBCLK_TMR0_EN_Msk;

    // /* Peripheral clock source */
    // CLK->CLKSEL1 = CLK_CLKSEL1_UART_S_PLL |
    //                CLK_CLKSEL1_TMR0_S_HXT;

    /* Enable clock source */
    // CLK_EnableXtalRC(CLK_PWRCON_OSC10K_EN_Msk|CLK_PWRCON_OSC22M_EN_Msk);

    // /* Waiting for clock source ready */
    // CLK_WaitClockReady(CLK_CLKSTATUS_OSC10K_STB_Msk|CLK_CLKSTATUS_OSC22M_STB_Msk);

    // /* Disable PLL first to avoid unstable when setting PLL */
    // CLK_DisablePLL();

    // /* Set PLL frequency */
    // CLK->PLLCON = (CLK->PLLCON & ~(0x000FFFFFul)) | 0x0008D66Ful;

    // /* Waiting for PLL ready */
    // CLK_WaitClockReady(CLK_CLKSTATUS_PLL_STB_Msk);

    // /* If the defines do not exist in your project, please refer to the related clk.h in the Header folder appended to the tool package. */
    // /* Set HCLK clock */
    // CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_PLL, CLK_CLKDIV_HCLK(1));

    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable IP clock */
    CLK_EnableModuleClock(ADC_MODULE);
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR1_MODULE);
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(WDT_MODULE);

    /* Set IP clock */
    CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL1_ADC_S_HIRC, CLK_CLKDIV_ADC(7));
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HIRC, MODULE_NoMsk);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1_S_LIRC, MODULE_NoMsk);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HIRC, CLK_CLKDIV_UART(1));
    CLK_SetModuleClock(WDT_MODULE, CLK_CLKSEL1_WDT_S_LIRC, MODULE_NoMsk);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set P3 multi-function pins for UART0 RXD, TXD */
    SYS->P3_MFP &= ~(SYS_MFP_P30_Msk | SYS_MFP_P31_Msk);
    SYS->P3_MFP |= (SYS_MFP_P30_RXD0 | SYS_MFP_P31_TXD0);

    /* Disable the P1.0 - P1.7 digital input path to avoid the leakage current */
    GPIO_DISABLE_DIGITAL_PATH(P1, 0xFF);

    /* Configure the P1.0 - P1.7 ADC analog input pins */
    SYS->P1_MFP &= ~(SYS_MFP_P10_Msk | SYS_MFP_P11_Msk | SYS_MFP_P12_Msk | SYS_MFP_P13_Msk | SYS_MFP_P14_Msk | SYS_MFP_P15_Msk | SYS_MFP_P16_Msk | SYS_MFP_P17_Msk);
    SYS->P1_MFP |= SYS_MFP_P10_AIN0 | SYS_MFP_P11_AIN1 | SYS_MFP_P12_AIN2 | SYS_MFP_P13_AIN3 | SYS_MFP_P14_AIN4 | SYS_MFP_P15_AIN5 | SYS_MFP_P16_AIN6 | SYS_MFP_P17_AIN7;

    GPIO_EnableInt(P0, 0, GPIO_INT_FALLING);    // Discharge switch
    GPIO_EnableInt(P0, 1, GPIO_INT_FALLING);    // 1W LED switch
    // GPIO_EnableInt(P1, 0, GPIO_INT_RISING);
    NVIC_EnableIRQ(GPIO_P0P1_IRQn);

    /* Enable WDT interrupt function */
    // WDT_EnableInt();

    /* Enable WDT NVIC */
    // NVIC_EnableIRQ(WDT_IRQn);

    // WDT_Open(WDT_TIMEOUT_2POW4, WDT_RESET_DELAY_3CLK, TRUE, FALSE);
    // WDT_Open(WDT_TIMEOUT_2POW14, WDT_RESET_DELAY_1026CLK, TRUE, FALSE);
}


//*****************************************************************************
//
//! \brief  Initialize GPIO modes.
//!
//! \return None.
//
//*****************************************************************************
void GPIO_Init(void)
{
    TXRX_DISABLE();
    // TXRX_ENABLE();
    CTLD2_EN_HIGH();
    CTLC2_EN_HIGH();
    LED_C25_OFF();
    LED_C50_OFF();
    LED_C75_OFF();
    LED_C100_OFF();
    LED_RED_OFF();
    LED_GREEN_OFF();
    LED_1W_OFF();
    EN_12V_ON();
    EN_5V_ON();
    RY_NO_OFF();
    RY_NC_OFF();
    OP_EN1_ENABLE();
    OP_EN2_ENABLE();
    OP_EN3_ENABLE();

    GPIO_SetMode(P00_SW_DIS, GPIO_PMD_INPUT);
    GPIO_SetMode(P01_SW_1WLED, GPIO_PMD_INPUT);
    // GPIO_SetMode(P04_DOF, GPIO_PMD_INPUT);
    // GPIO_SetMode(P05_COF, GPIO_PMD_INPUT);
    GPIO_SetMode(P06_TXRX_EN, GPIO_PMD_OUTPUT);
    GPIO_SetMode(P07_CTLD2_EN, GPIO_PMD_OUTPUT);

    GPIO_SetMode(P10_AD_CHARGE_CURR, GPIO_PMD_INPUT);
    GPIO_SetMode(P11_AD_DISCHARGE_CURR, GPIO_PMD_INPUT);
    GPIO_SetMode(P12_AD_RELAY_CURR, GPIO_PMD_INPUT);
    GPIO_SetMode(P13_SD_PACK, GPIO_PMD_INPUT);
    GPIO_SetMode(P14_SD_ADAPTER, GPIO_PMD_INPUT);
    GPIO_SetMode(P15_SD_RELAY, GPIO_PMD_INPUT);
    GPIO_SetMode(P16_DOF, GPIO_PMD_INPUT);
    GPIO_SetMode(P17_COF, GPIO_PMD_INPUT);

    GPIO_SetMode(P20_LED_C25, GPIO_PMD_OUTPUT);
    GPIO_SetMode(P21_LED_C50, GPIO_PMD_OUTPUT);
    GPIO_SetMode(P22_LED_C75, GPIO_PMD_OUTPUT);
    GPIO_SetMode(P23_LED_C100, GPIO_PMD_OUTPUT);
    GPIO_SetMode(P24_LED_RED, GPIO_PMD_OUTPUT);
    GPIO_SetMode(P25_LED_GREEN, GPIO_PMD_OUTPUT);
    GPIO_SetMode(P26_LED_1W, GPIO_PMD_OUTPUT);

    GPIO_SetMode(P32_CTLC2_EN, GPIO_PMD_OUTPUT);
    GPIO_SetMode(P33_EN_12V, GPIO_PMD_OUTPUT);
    GPIO_SetMode(P34_EN_5V, GPIO_PMD_OUTPUT);
    GPIO_SetMode(P35_RY_NO, GPIO_PMD_OUTPUT);
    GPIO_SetMode(P36_RY_NC, GPIO_PMD_OUTPUT);

    GPIO_SetMode(P40_OP_EN1, GPIO_PMD_OUTPUT);
    GPIO_SetMode(P41_OP_EN2, GPIO_PMD_OUTPUT);
    GPIO_SetMode(P42_OP_EN3, GPIO_PMD_OUTPUT);
}



//*****************************************************************************
//
//! \brief  Initialize UART0 module.
//!
//! \return None.
//
//*****************************************************************************
void UART0_Init(void)
{
    /* Reset IP */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);

    // UART_ENABLE_INT(UART0, (UART_IER_RDA_IEN_Msk | UART_IER_THRE_IEN_Msk | UART_IER_RTO_IEN_Msk));
    UART_ENABLE_INT(UART0, (UART_IER_RDA_IEN_Msk | UART_IER_RTO_IEN_Msk) );
    NVIC_EnableIRQ(UART0_IRQn);
}



//*****************************************************************************
//
//! \brief  Initialize Timer 0 module.
//!
//! \return None.
//
//*****************************************************************************
void Timer0_Init(void)
{
    /* Open Timer0 frequency to 0.5 Hz in periodic mode, and enable interrupt */
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 10);
    TIMER_EnableInt(TIMER0);

    /* Enable Timer0 NVIC */
    NVIC_EnableIRQ(TMR0_IRQn);

    /* Start Timer0 counting */
    TIMER_Start(TIMER0);
}


//*****************************************************************************
//
//! \brief  Initialize Timer 1 module.
//!
//! \return None.
//
//*****************************************************************************
void Timer1_Init(void)
{
    /* Open Timer0 frequency to 0.5 Hz in periodic mode, and enable interrupt */
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1);
    TIMER_EnableInt(TIMER1);

    /* Enable Timer0 NVIC */
    NVIC_EnableIRQ(TMR1_IRQn);

    TIMER_EnableWakeup(TIMER1);

    /* Start Timer0 counting */
    TIMER_Start(TIMER1);
}


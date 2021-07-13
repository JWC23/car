/**************************************************************************//**
 * @file    adc_func.c
 * $Date:   2021/02/22 15:10 $
 * @brief   ADC application function source file
 *
 * @author JW Chang
 *
*****************************************************************************/
#include "main.h"
#include "adc_func.h"
#include "adc.h"


vuint8_t g_u8AdcIntFlag = 0;        //!< ADC interrupt flag


//*****************************************************************************
//
//! \brief  Read ADC
//
//! \param  u8Chan Decides which channel to be read.
//
//! \return 12-ADC value.
//
//*****************************************************************************
uint16_t ADC_Read(uint8_t u8Chan)
{
    /* Power on ADC module */
    ADC_POWER_ON(ADC);

    /* Set the ADC operation mode as single, input mode as single-end and enable the analog input channel */
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE, 0x1 << u8Chan);

    /* Clear the A/D interrupt flag for safe */
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

    /* Enable the ADC interrupt */
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);

    /* Reset the ADC interrupt indicator and Start A/D conversion */
    g_u8AdcIntFlag = 0;

    ADC_START_CONV(ADC);

    /* Wait ADC interrupt (g_u8AdcIntFlag will be set at IRQ_Handler function)*/
    while(g_u8AdcIntFlag == 0);

    /* Disable the ADC interrupt */
    ADC_DisableInt(ADC, ADC_ADF_INT);

    /* Get the conversion result of the ADC channel */
    return ADC_GET_CONVERSION_DATA(ADC, u8Chan);
}


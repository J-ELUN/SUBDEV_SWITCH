#ifndef __ADC_H__
#define __ADC_H__
#include "stm32l0xx_hal.h"

#define VDD_APPLI                      ((uint32_t)2050)  //((uint32_t)2050)    /* Value of analog voltage supply Vdda (unit: mV) */
#define RANGE_12BITS                   ((uint32_t)4096)    /* Max value with a full range of 12 bits */
#define TIMER_FREQUENCY_HZ             ((uint32_t)1000)    /* Timer frequency (unit: Hz). With SysClk set to 2MHz, timer frequency TIMER_FREQUENCY_HZ range is min=1Hz, max=33.3kHz. */

#define COMPUTATION_DIGITAL_12BITS_TO_VOLTAGE(ADC_DATA)                        \
  (  VDD_APPLI * RANGE_12BITS  / (ADC_DATA))
    
/* ## Definition of ADC related resources ################################### */
/* Definition of ADCx clock resources */
#define ADCx                            ADC1
#define ADCx_CLK_ENABLE()               __ADC1_CLK_ENABLE()
#define ADCx_CLK_DISABLE()              __ADC1_CLK_DISABLE()
    
#define ADCx_FORCE_RESET()              __HAL_RCC_ADC1_FORCE_RESET()
#define ADCx_RELEASE_RESET()            __HAL_RCC_ADC1_RELEASE_RESET()

/* Definition of ADCx channels */
#define ADCx_CHANNELa                   ADC_CHANNEL_6  

/* Definition of ADCx channels pins */
#define ADCx_CHANNELa_GPIO_CLK_ENABLE()   __GPIOA_CLK_ENABLE()
#define ADCx_CHANNELa_GPIO_CLK_DISABLE()  __GPIOA_CLK_DISABLE()   
#define ADCx_CHANNELa_GPIO_PORT           GPIOA
#define ADCx_CHANNELa_PIN                 GPIO_PIN_6

#define ADCx_DMA_CLK_ENABLE()             __DMA1_CLK_ENABLE()
#define ADCx_DMA_CLK_DISABLE()            __DMA1_CLK_DISABLE()    
#define ADCx_DMA_CHANNEL                  DMA1_Channel1
#define ADCx_DMA_REQUEST                  DMA_REQUEST_0
    
/* Definition of ADCx NVIC resources */
#define ADCx_DMA_IRQn                   DMA1_Channel1_IRQn
#define ADCx_DMA_IRQHandler             ADC1_COMP_IRQHandler


extern ADC_HandleTypeDef    AdcHandle;
void ADC_Init(void);
void ADC_DeInit(void);
u16 Read_ADC_Voltage(void);

#endif

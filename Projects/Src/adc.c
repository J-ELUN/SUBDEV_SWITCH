#include "adc.h"
#include "stm32l0xx_hal_msp.h"

/* ADC handle declaration */
ADC_HandleTypeDef             AdcHandle;

/* ADC channel configuration structure declaration */
ADC_ChannelConfTypeDef        sConfig;

/* Converted value declaration */
u32 aResultDMA[10];

/**
* @brief  ADC MSP Init
* @param  hadc : ADC handle
* @retval None
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef                 GPIO_InitStruct;
  static DMA_HandleTypeDef         DmaHandle;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO clock ****************************************/
  ADCx_CHANNELa_GPIO_CLK_ENABLE();
  /* ADC1 Periph clock enable */
  ADCx_CLK_ENABLE();
  /* Enable DMA1 clock */
  ADCx_DMA_CLK_ENABLE();

  
  /*##- 2- Configure peripheral GPIO #########################################*/
  /* ADC3 Channel8 GPIO pin configuration */
  GPIO_InitStruct.Pin = ADCx_CHANNELa_PIN ;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ADCx_CHANNELa_GPIO_PORT, &GPIO_InitStruct);
  
  /*##- 3- Configure DMA #####################################################*/ 

  /*********************** Configure DMA parameters ***************************/
  DmaHandle.Instance                 = ADCx_DMA_CHANNEL;
  DmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  DmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  DmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  DmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  DmaHandle.Init.Mode                = DMA_CIRCULAR;
  DmaHandle.Init.Priority            = DMA_PRIORITY_MEDIUM;
  DmaHandle.Init.Request             = ADCx_DMA_REQUEST;
  
  /* Deinitialize  & Initialize the DMA for new transfer */
  HAL_DMA_DeInit(&DmaHandle);  
  HAL_DMA_Init(&DmaHandle);
  
  /* Associate the DMA handle */
  __HAL_LINKDMA(hadc, DMA_Handle, DmaHandle);
  
//  /* NVIC configuration for DMA Input data interrupt */
//  HAL_NVIC_SetPriority(ADCx_DMA_IRQn, 1, 1);
//  HAL_NVIC_EnableIRQ(ADCx_DMA_IRQn);
}


/**
* @brief  ADC MSP DeInit
* @param  hadc : ADC handle
* @retval None
*/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Disable GPIO clock ****************************************/
  ADCx_CHANNELa_GPIO_CLK_DISABLE();
  /* ADC1 Periph clock Disable */
  ADCx_CLK_DISABLE();
  /* Disable DMA1 clock */
  ADCx_DMA_CLK_DISABLE();
}

void ADC_Init(void)
{
  /* ### - 1 - Initialize ADC peripheral #################################### */
  /*
   *  Instance                  = ADC1.
   *  OversamplingMode          = Disabled
   *  ClockPrescaler            = PCLK clock with no division.
   *  LowPowerAutoOff           = Disabled (For this exemple continuous mode is enabled with software start)
   *  LowPowerFrequencyMode     = Enabled (To be enabled only if ADC clock is lower than 2.8MHz) 
   *  LowPowerAutoWait          = Enabled (New conversion starts only when the previous conversion is completed)       
   *  Resolution                = 12 bit (increased to 16 bit with oversampler)
   *  SamplingTime              = 7.5 cycles od ADC clock.
   *  ScanDirection             = Upward 
   *  DataAlign                 = Right
   *  ContinuousConvMode        = Enabled
   *  DiscontinuousConvMode     = Enabled
   *  ExternalTrigConvEdge      = None (Software start)
   *  EOCSelection              = End Of Conversion event
   *  DMAContinuousRequests     = ENABLE
   */
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
	__GPIOA_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
  
  AdcHandle.Instance = ADC1;
  
  AdcHandle.Init.OversamplingMode      = DISABLE;
  
  AdcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV1;
  AdcHandle.Init.LowPowerAutoOff       = ENABLE;
  AdcHandle.Init.LowPowerFrequencyMode = ENABLE;
  AdcHandle.Init.LowPowerAutoWait      = ENABLE;
    
  AdcHandle.Init.Resolution            = ADC_RESOLUTION12b;
  AdcHandle.Init.SamplingTime          = ADC_SAMPLETIME_239CYCLES_5;
  AdcHandle.Init.ScanDirection         = ADC_SCAN_DIRECTION_UPWARD;
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  AdcHandle.Init.ContinuousConvMode    = ENABLE;
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;
  AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIG_EDGE_NONE;
  AdcHandle.Init.EOCSelection          = EOC_SINGLE_CONV;
  AdcHandle.Init.DMAContinuousRequests = ENABLE;
 
  /* Initialize ADC peripheral according to the passed parameters */
  if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
  {
    Error_Handler();
  }
  
  
  /* ### - 2 - Start calibration ############################################ */
  if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_SINGLE_ENDED) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* ### - 3 - Channel configuration ######################################## */
  /* Select Channel 0 to be converted */
  sConfig.Channel = ADCx_CHANNELa;    
  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* ### - 4 - Start conversion in DMA mode ################################# */
  if (HAL_ADC_Start_DMA(&AdcHandle, aResultDMA, 10) != HAL_OK)
  {
    Error_Handler();
  }
  
}

void ADC_DeInit(void)
{
  HAL_ADC_DeInit(&AdcHandle);
}

u16 Read_ADC_Voltage(void)
{
  u8 i;
  u16 Vdd=0;
  u32 temp=0;
  for(i=0;i<10;i++)
    temp+=COMPUTATION_DIGITAL_12BITS_TO_VOLTAGE(aResultDMA[i]);
  Vdd=temp/10;
  return Vdd;
}












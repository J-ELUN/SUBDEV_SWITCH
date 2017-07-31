/**
  ******************************************************************************
  * @file    Templates/Src/stm32l0xx_hal_msp.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-June-2014
  * @brief   HAL MSP module.
  *         
  @verbatim
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
    [..]
    This file is generated automatically by STM32CubeMX and eventually modified 
    by the user

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal_msp.h"    
#include <string.h>

/** @addtogroup STM32L0xx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
	

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */
	
SPI_HandleTypeDef SpiHandle;


/* IWDG and TIM handlers declaration */ 
IWDG_HandleTypeDef   IwdgHandle;                        /* 独立看门狗 */
TIM_HandleTypeDef    Input_Handle;                      /* 独立看门狗定时器 */
u16 tmpCC4[2] = {0, 0};
volatile uint32_t uwLsiFreq = 0;
volatile uint32_t uwCaptureNumber = 0;
volatile uint32_t uwPeriodValue = 0;

volatile u8 Pvd_Flag=0;


/**
  * @brief  Turns selected PIN ON.
  * @param  GPIOx: Specifies GPIO.
  * @param  GPIO_Pinx: Specifies PIN.  
  * @retval None
  */
void GPIOx_PINx_SET(GPIO_TypeDef* GPIOx,u16 GPIO_Pinx)
{
  HAL_GPIO_WritePin(GPIOx, GPIO_Pinx, GPIO_PIN_SET);
}

/**
  * @brief  Turns selected PIN OFF.
  * @param  GPIOx: Specifies GPIO.
  * @param  GPIO_Pinx: Specifies PIN.  
  * @retval None
  */
void GPIOx_PINx_RESET(GPIO_TypeDef* GPIOx,u16 GPIO_Pinx)
{
  HAL_GPIO_WritePin(GPIOx, GPIO_Pinx, GPIO_PIN_RESET);
}

void Led_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable the GPIO Clock */  
  LED_RUN_GPIO_CLK_ENABLE();

  /* Configure the GPIO pin */
  GPIO_InitStruct.Pin = LED_RUN_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(LED_RUN_GPIO, &GPIO_InitStruct);  
  LED_RUN_ON();
  HAL_Delay(1000);
  LED_RUN_OFF();
}

void Led_DeInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Configure the GPIO pin */
  GPIO_InitStruct.Pin = LED_RUN_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LED_RUN_GPIO, &GPIO_InitStruct);  
}

void Led_Run_Toggle(void) 
{
  HAL_GPIO_TogglePin(LED_RUN_GPIO, LED_RUN_PIN);
}


void Led_Run_Blink(u8 fre, u16 delay_m)
{
  u8 i; 
  Led_Init();
  for(i=0;i<fre;i++)
  {
    LED_RUN_ON();
    HAL_Delay(delay_m);
    LED_RUN_OFF();
    HAL_Delay(delay_m);
  }
  Led_DeInit();
}

void Led_Run_Error(void)
{
  Led_Init();
  while(1)
  {
    Led_Run_Toggle();
    HAL_Delay(1000);
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1)
  {
    HAL_NVIC_SystemReset();
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 2000000
  *            HCLK(Hz)                       = 2000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_MUL                        = 4
  *            PLL_DIV                        = 2
  *            Flash Latency(WS)              = 1
  *            Main regulator output voltage  = Scale1 mode
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSI Oscillator and activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV4;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;
  RCC_OscInitStruct.HSICalibrationValue = 0x10;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}



/** @defgroup HAL_MSP_Private_Functions
  * @{
  */
void HAL_IWDG_MspInit(IWDG_HandleTypeDef* hiwdg)
{
  __PWR_CLK_ENABLE(); 

  HAL_PWR_EnableBkUpAccess();
   
}

/**
  * @brief TIM MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  * @param htim: TIM handle pointer
  * @retval None
  */
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
  
  /*## Enable peripherals and GPIO Clocks ####################################*/
  /* RCC LSI clock enable */
  __HAL_RCC_LSI_ENABLE();
  
  /* Wait till LSI is ready */
  while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET)
  {}

  /* TIMx Peripheral clock enable */
  __TIM21_CLK_ENABLE();
  
  /*## Configure the NVIC for TIMx ###########################################*/
  HAL_NVIC_SetPriority(TIM21_IRQn,1,0);
  
  /* Enable the TIM21 global Interrupt */
  HAL_NVIC_EnableIRQ(TIM21_IRQn);
}

/**
  * @brief  Configures TIM21 to measure the LSI oscillator frequency. 
  * @param  None
  * @retval LSI Frequency
  */
static u32 GetLSIFrequency(void)
{
  TIM_IC_InitTypeDef    TIMInput_Config;

  /* Configure the TIM peripheral *********************************************/ 
  /* Set TIMx instance */  
  Input_Handle.Instance = TIM21;
  
  /* TIM21 configuration: Input Capture mode ---------------------
     The LSI oscillator is connected to TIM21 CH1.
     The Rising edge is used as active edge.
     The TIM21 CCR1 is used to compute the frequency value. 
  ------------------------------------------------------------ */
  Input_Handle.Init.Prescaler         = 0; 
  Input_Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;  
  Input_Handle.Init.Period            = 0xFFFF; 
  Input_Handle.Init.ClockDivision     = 0;     
  if(HAL_TIM_IC_Init(&Input_Handle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /* Connect internally the TIM21_CH1 Input Capture to the LSI clock output */
  HAL_TIMEx_RemapConfig(&Input_Handle, TIM21_TI1_LSI);
  
  /* Configure the Input Capture of channel 1 */
  TIMInput_Config.ICPolarity  = TIM_ICPOLARITY_RISING;
  TIMInput_Config.ICSelection = TIM_ICSELECTION_DIRECTTI;
  TIMInput_Config.ICPrescaler = TIM_ICPSC_DIV8;
  TIMInput_Config.ICFilter    = 0;
  if(HAL_TIM_IC_ConfigChannel(&Input_Handle, &TIMInput_Config, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Start the TIM Input Capture measurement in interrupt mode */
  if(HAL_TIM_IC_Start_IT(&Input_Handle, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Wait until the TIM21 get 2 LSI edges */
  while(uwCaptureNumber != 2)
  {
  }

  /* Disable TIM21 CC1 Interrupt Request */
  HAL_TIM_IC_Stop_IT(&Input_Handle, TIM_CHANNEL_1);
  
  /* Deinitialize the TIM21 peripheral registers to their default reset values */
  HAL_TIM_IC_DeInit(&Input_Handle);

  return uwLsiFreq;
}

/**
  * @brief  Input Capture callback in non blocking mode 
  * @param  htim : TIM IC handle
  * @retval None
*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{    
  /* Get the Input Capture value */
  tmpCC4[uwCaptureNumber++] = HAL_TIM_ReadCapturedValue(&Input_Handle, TIM_CHANNEL_1);
  
  if (uwCaptureNumber >= 2)
  {
    /* Compute the period length */
    uwPeriodValue = (uint16_t)(0xFFFF - tmpCC4[0] + tmpCC4[1] + 1);
    
    /* Frequency computation */ 
    uwLsiFreq = (uint32_t) SystemCoreClock / uwPeriodValue;
    uwLsiFreq *= 8;
  }
}

/**
  * @brief  IWDOG_Init 
  * @param  None
  * @retval None
*/
void IWDOG_Init(void)
{
  /*##-1- Check if the system has resumed from IWDG reset ####################*/
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)
  {    
    /* Clear reset flags */
    __HAL_RCC_CLEAR_RESET_FLAGS();
  }

  /*##-2- Get the LSI frequency: TIM21 is used to measure the LSI frequency ###*/
  uwLsiFreq = GetLSIFrequency();
  
  /*##-3- Configure & Initialize the IWDG peripheral ######################################*/
  /* Set counter reload value to obtain 250ms IWDG TimeOut.
     IWDG counter clock Frequency = LsiFreq/32
     Counter Reload Value = 250ms/IWDG counter clock period
                          = 0.25s / (32/LsiFreq)
                          = LsiFreq/(32 * 4)
                          = LsiFreq/128 */
  IwdgHandle.Instance = IWDG;

  IwdgHandle.Init.Prescaler = IWDG_PRESCALER_64;
  IwdgHandle.Init.Reload = uwLsiFreq/128;
  IwdgHandle.Init.Window = IWDG_WINDOW_DISABLE;

  if(HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /*##-4- Start the IWDG #####################################################*/ 
  if(HAL_IWDG_Start(&IwdgHandle) != HAL_OK)
  {
    Error_Handler();
  }
}
/**
  * @brief  Feed_Dog
  * @param  None
  * @retval None
*/
void Feed_Dog(void)
{ 
  /* Refresh IWDG: reload counter */
  if(HAL_IWDG_Refresh(&IwdgHandle) != HAL_OK)
  {
    /* Refresh Error */
    Error_Handler();
  }
}


void Eeprom_Write(u32 Address,u8 *write_buf,u16 buf_len)
{
  u16 i;
  
  if(Address>=USER_DATA_EEPROM_BASE && Address<=USER_DATA_EEPROM_END)
  {
    if(HAL_FLASHEx_DATAEEPROM_Unlock() == HAL_OK)  
    {
      for(i=0;i<buf_len/4;i++)
        HAL_FLASHEx_DATAEEPROM_Erase(Address+i*4);  //erase a word
      
      for(i=0;i<buf_len;i++)
      {
        HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAM_FASTBYTE, Address+i, *(write_buf+i));
      }
      
      HAL_FLASHEx_DATAEEPROM_Lock();     
    }
  }
}


void Eeprom_Read(u32 Address,u8 *read_buf,u16 buf_len)
{
  u16 i;
  if(Address>=USER_DATA_EEPROM_BASE && Address<=USER_DATA_EEPROM_END)
  {
    for(i=0;i<buf_len/4;i++)    
      memcpy(read_buf+i*4,(const unsigned char *)(Address+i*4),4);
  }
}

/**
  * @brief This function provides accurate delay (in ms) based on a variable incremented.
  * @note In the default implementation , SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals where uwTick
  *       is incremented.
  * @note ThiS function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay: specifies the delay time length, in milliseconds.
  * @retval None
  */
void HAL_Delay(volatile u32 Delay)
{
  u32 tickstart = 0U;
  tickstart = HAL_GetTick();
  while((HAL_GetTick() - tickstart) < Delay)
  {
  }
}
					
/**
  * @brief SPI MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - NVIC configuration for SPI interrupt request enable
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  if (hspi->Instance == SPIx)
  {
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    
    /* Enable SPI clock */
    SPIx_CLK_ENABLE();
    
    /* Enable GPIO TX/RX clock */
    SPIx_SCK_GPIO_CLK_ENABLE();
    SPIx_MISO_GPIO_CLK_ENABLE();
    SPIx_MOSI_GPIO_CLK_ENABLE();
    SPIx_CS_GPIO_CLK_ENABLE();
         

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = SPIx_SCK_AF;
    HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.Pin = SPIx_MISO_PIN;
    GPIO_InitStruct.Alternate = SPIx_MISO_AF;
    HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
    GPIO_InitStruct.Alternate = SPIx_MOSI_AF;
    HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);
    
    /* CS MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin       = SPIx_CS_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    HAL_GPIO_Init(SPIx_CS_GPIO_PORT, &GPIO_InitStruct);
    SPI_CS_SET();	
    
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  if (hspi->Instance == SPIx)
  {
    /* Disnable GPIO TX/RX clock */
    SPIx_CLK_DISABLE();
    
//    SPIx_SCK_GPIO_CLK_DISABLE();
//    SPIx_MISO_GPIO_CLK_DISABLE();
//    SPIx_MOSI_GPIO_CLK_DISABLE();
//    SPIx_CS_GPIO_CLK_DISABLE();
         

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.Pin       = SPIx_MISO_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin       = SPIx_MOSI_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);
    
    /* CS MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin       = SPIx_CS_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    HAL_GPIO_Init(SPIx_CS_GPIO_PORT, &GPIO_InitStruct); 
  }
}


void SPI_Init(void)
{
  /*##-1- Configure the SPI peripheral #######################################*/
  /* Set the SPI parameters */
  SpiHandle.Instance               = SPIx;
  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  SpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE;
  SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
  SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLED;  
  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLED;
  SpiHandle.Init.CRCPolynomial     = 7;
  SpiHandle.Init.NSS               = SPI_NSS_SOFT;
  SpiHandle.Init.Mode = SPI_MODE_MASTER;
  
  if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}

void SPI_DeInit(void)
{
  HAL_SPI_DeInit(&SpiHandle);
}

/**
  * @brief  Configures the PVD resources.
  * @param  None
  * @retval None
  */
void PVD_Config(void)
{
  PWR_PVDTypeDef sConfigPVD;
  
  /*##-1- Enable Power Clock #################################################*/
  __PWR_CLK_ENABLE();

  /*##-2- Configure the NVIC for PVD #########################################*/
  HAL_NVIC_SetPriority(PVD_IRQn, 0, 3);
  HAL_NVIC_EnableIRQ(PVD_IRQn);

  /* Configure the PVD Level to 5 and generate an interrupt on rising 
     edges(PVD detection level set to 2.9V, refer to the electrical characteristics
     of you device datasheet for more details) */
  sConfigPVD.PVDLevel = PWR_PVDLEVEL_5;  
  sConfigPVD.Mode = PWR_MODE_IT_RISING;
  HAL_PWR_PVDConfig(&sConfigPVD);

  /* Enable the PVD Output */
  HAL_PWR_EnablePVD();
}


void HAL_PWR_PVDCallback(void)
{
  Pvd_Flag=1;
}


/**
  * @brief  Initializes the Global MSP.
  * @param  None
  * @retval None
  */
void HAL_MspInit(void)
{
  /* NOTE : This function is generated automatically by STM32CubeMX and eventually  
            modified by the user
   */ 
}

/**
  * @brief  DeInitializes the Global MSP.
  * @param  None  
  * @retval None
  */
void HAL_MspDeInit(void)
{
  /* NOTE : This function is generated automatically by STM32CubeMX and eventually  
            modified by the user
   */
}


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

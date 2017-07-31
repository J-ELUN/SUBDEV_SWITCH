/**
  ******************************************************************************
  * @file    Templates/Src/stm32l0xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-June-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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

#include "stm32l0xx_it.h"
#include "main.h"
#include "stm32l0xx_hal_msp.h" 
#include "zigbee.h"
#include "adc.h"
#include "switch.h"
#include <string.h>

/** @addtogroup STM32L0xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/******************************************************************************/
/*            Cortex-M0+ Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}


/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  { 
    HAL_NVIC_SystemReset();
  }
}

/**
  * @brief  This function handles TIM21 global interrupt request.
  * @param  None
  * @retval None
  */
//void IWDG_IRQHandler(void)
//{ 
//  HAL_TIM_IRQHandler(&Input_Handle);
//}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}


/**
  * @brief   Debug&ZigbeeDMA发送中断
  * @param  None
  * @retval None
  */
void USARTx_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(huartx_zigbee.hdmatx);
  //HAL_DMA_IRQHandler(huartx_debug.hdmatx);
}



/**
  * @brief   Debug串口总线空闲中断
  * @param  None
  * @retval None
  */
//void USARTx_DEBUG_IRQHandler(void)
//{
//  u16 CurrDataCount=0;
//  u16 rx_len=0;
//  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
//      
//  if(__HAL_UART_GET_FLAG(&huartx_debug,UART_FLAG_IDLE)!= RESET)
//  {
//    CurrDataCount = USART2->ISR;
//    CurrDataCount = USART2->RDR; 
//    
//    __HAL_UART_CLEAR_IT(&huartx_debug, UART_CLEAR_IDLEF); 
//     /* Disable DMA */
//    __HAL_DMA_DISABLE(huartx_debug.hdmarx); 
//    CurrDataCount = huartx_debug.hdmarx->Instance->CNDTR; 
//    rx_len=  UART_MAX_RX_LEN - CurrDataCount;
//    
//    if(rx_len>0 && rx_len<UART_MAX_RX_LEN) 
//    {
//      Debug_DMA_Rx_Buf.len=rx_len;
//      /* Send To Queue*/
//      xQueueSendToBackFromISR(MsgQueue_Debug,&Debug_DMA_Rx_Buf,0); 
//    }    
//    /* Set CNDTR */
//    huartx_debug.hdmarx->Instance->CNDTR=UART_MAX_RX_LEN;  
//           
//    /* Enable DMA*/
//     __HAL_DMA_ENABLE(huartx_debug.hdmarx); 
//    
//    /* 上下文切换*/
//    if(xHigherPriorityTaskWoken == pdTRUE )
//    {
//      portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );    
//    }
//  }
//}


  
/**
  * @brief   ZIGBEE串口总线空闲中断
  * @param  None
  * @retval None
  */
void USARTx_ZIGBEE_IRQHandler(void)  
{
  u16 CurrDataCount=0;
  u16 rx_len=0;
      
  if(__HAL_UART_GET_FLAG(&huartx_zigbee,UART_FLAG_IDLE)!= RESET)
  {
    CurrDataCount = LPUART1->ISR;
    CurrDataCount = LPUART1->RDR; 
    
    __HAL_UART_CLEAR_IT(&huartx_zigbee, UART_CLEAR_IDLEF); 
     /* Disable DMA */
    __HAL_DMA_DISABLE(huartx_zigbee.hdmarx); 
    CurrDataCount = huartx_zigbee.hdmarx->Instance->CNDTR; 
       
    rx_len=  UART_MAX_RX_LEN - CurrDataCount; 
    if(rx_len>0 && rx_len<UART_MAX_RX_LEN && (Zig_Uart_Buf.Rx_Len==0))    
    {
      Zig_Uart_Buf.Rx_Len =rx_len;  
      memcpy(Zig_Uart_Buf.Rx_Buf,Zig_DMA_RxBuf,rx_len);
    }    
    /* Set CNDTR */
    huartx_zigbee.hdmarx->Instance->CNDTR=UART_MAX_RX_LEN;  
           
    /* Enable DMA*/
     __HAL_DMA_ENABLE(huartx_zigbee.hdmarx); 
   
  }
}

/**
* @brief This function handles EXTI line 4 to 15 interrupts.
*/
void SWITCH_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */
  /* USER CODE END EXTI4_15_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(SWITCH_1_PIN);
  HAL_GPIO_EXTI_IRQHandler(SWITCH_2_PIN);
  HAL_GPIO_EXTI_IRQHandler(SWITCH_3_PIN);
  HAL_GPIO_EXTI_IRQHandler(SWITCH_4_PIN); 
  HAL_GPIO_EXTI_IRQHandler(SWITCH_SET_PIN);
  /* USER CODE BEGIN EXTI4_15_IRQn 1 */

  /* USER CODE END EXTI4_15_IRQn 1 */
}


/**
  * @brief  This function handles the PVD Output interrupt request.
  * @param  None
  * @retval None
  */
void PVD_IRQHandler(void)
{
  HAL_PWR_PVD_IRQHandler();
}


/******************************************************************************/
/*                 STM32L0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l0xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

///**************************************************************************************************
// *                                            INCLUDES
// **************************************************************************************************/
#include "usart.h"
#include "stm32l0xx_hal_msp.h"

UART_HandleTypeDef huartx_zigbee;                       /* zigbee´®¿Ú */
UART_HandleTypeDef huartx_debug;                       /* debug´®¿Ú */

u8 Zig_DMA_RxBuf[UART_MAX_RX_LEN];
/**************************************************************************************************
 *                                           FUNCTION
 **************************************************************************************************/

/**
  * @brief  USART2 for UM200 init function 
  * @param  None
  * @retval None
  */
void HAL_UARTx_Init(UART_HandleTypeDef *huartx)
{
  if(huartx==&huartx_debug)
  {
    huartx->Instance = USARTx_DEBUG;
    huartx->Init.BaudRate = BAUND_DEBUG;
    huartx->Init.WordLength = UART_WORDLENGTH_8B;
    huartx->Init.StopBits = UART_STOPBITS_1;
    huartx->Init.Parity = UART_PARITY_NONE;
    huartx->Init.HwFlowCtl = UART_HWCONTROL_NONE; 
    huartx->Init.Mode = UART_MODE_TX_RX;

    if(HAL_UART_Init(huartx) != HAL_OK)
    {
      Error_Handler(); 
    }
    /* 
    if(HAL_UART_Receive_DMA(huartx, Debug_DMA_Rx_Buf.Rx_Buf, UART_MAX_RX_LEN) != HAL_OK) //debug
    {
      Error_Handler();
    }*/
  }
  else
  {
    huartx->Instance = USARTx_ZIGBEE;
    huartx->Init.BaudRate   = BAUND_ZIGBEE;
    huartx->Init.WordLength = UART_WORDLENGTH_8B;
    huartx->Init.StopBits   = UART_STOPBITS_1;
    huartx->Init.Parity     = UART_PARITY_NONE;
    huartx->Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    huartx->Init.Mode       = UART_MODE_TX_RX;   
    huartx->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    
    if(HAL_UART_Init(huartx) != HAL_OK)
    {
      Error_Handler();
    }
    
    if(HAL_UART_Receive_DMA(huartx, Zig_DMA_RxBuf, UART_MAX_RX_LEN) != HAL_OK) 
    {
      Error_Handler();
    }
  }
}

/**
  * @brief  Initializes the Global MSP.
  * @param  None
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef* huart)   
{
  static DMA_HandleTypeDef hdma_debug_rx;
  static DMA_HandleTypeDef hdma_debug_tx;
  static DMA_HandleTypeDef hdma_zigbee_rx;
  static DMA_HandleTypeDef hdma_zigbee_tx;
  
  GPIO_InitTypeDef  GPIO_InitStruct;
  if(huart->Instance==USARTx_DEBUG)
  {
    
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    USARTx_DEBUG_RX_GPIO_CLK_ENABLE();
    USARTx_DEBUG_TX_GPIO_CLK_ENABLE();

    /* Enable USART2 clock */
    USARTx_DEBUG_CLK_ENABLE();  

    /* Enable DMA1 clock */
    DMAx_DEBUG_CLK_ENABLE();   

    /*##-2- Configure peripheral GPIO ##########################################*/  

    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = USARTx_DEBUG_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = USARTx_DEBUG_TX_AF;

    HAL_GPIO_Init(USARTx_DEBUG_TX_GPIO_PORT, &GPIO_InitStruct);

    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = USARTx_DEBUG_RX_PIN;
    GPIO_InitStruct.Alternate = USARTx_DEBUG_RX_AF;

    HAL_GPIO_Init(USARTx_DEBUG_RX_GPIO_PORT, &GPIO_InitStruct);

    /*##-3- Configure the DMA streams ##########################################*/
    /* Configure the DMA handler for Transmission process */
    hdma_debug_tx.Instance                 = USARTx_DEBUG_TX_DMA_CHANNEL;
    hdma_debug_tx.Init.Request             = USARTx_DEBUG_TX_DMA_REQUEST;
    hdma_debug_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_debug_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_debug_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_debug_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_debug_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_debug_tx.Init.Mode                = DMA_NORMAL;
    hdma_debug_tx.Init.Priority            = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_debug_tx); 

    /* Associate the initialized DMA handle to the the UART handle */   
    __HAL_LINKDMA(huart, hdmatx, hdma_debug_tx);

    /* Configure the DMA handler for Transmission process */
    hdma_debug_rx.Instance                 = USARTx_DEBUG_RX_DMA_CHANNEL;
    hdma_debug_rx.Init.Request             = USARTx_DEBUG_RX_DMA_REQUEST;
    hdma_debug_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_debug_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_debug_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_debug_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_debug_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_debug_rx.Init.Mode                = DMA_NORMAL;
    hdma_debug_rx.Init.Priority            = DMA_PRIORITY_HIGH;  
    HAL_DMA_Init(&hdma_debug_rx);

    /* Associate the initialized DMA handle to the the UART handle */
    __HAL_LINKDMA(huart, hdmarx, hdma_debug_rx);
    /*##-4- Configure the NVIC for DMA #########################################*/
    /* NVIC configuration for DMA transfer complete interrupt (USART1_TX) */
    HAL_NVIC_SetPriority(USARTx_DEBUG_DMA_TX_IRQn, 0, 2);
    HAL_NVIC_EnableIRQ(USARTx_DEBUG_DMA_TX_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt (USART1_RX) */
    HAL_NVIC_SetPriority(USARTx_DEBUG_DMA_RX_IRQn, 0, 2);   
    HAL_NVIC_EnableIRQ(USARTx_DEBUG_DMA_RX_IRQn); 

        /*##-3- Configure the NVIC for UART ########################################*/
    /* NVIC for USART2 */
    HAL_NVIC_SetPriority(USARTx_DEBUG_IRQn, 0, 2);
    HAL_NVIC_EnableIRQ(USARTx_DEBUG_IRQn);
    
  }
  else if(huart->Instance==USARTx_ZIGBEE)
  { 
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    USARTx_ZIGBEE_TX_GPIO_CLK_ENABLE();
    USARTx_ZIGBEE_RX_GPIO_CLK_ENABLE();
    /* Enable LPUART1 clock */
    USARTx_ZIGBEE_CLK_ENABLE(); 
    DMAx_ZIGBEE_CLK_ENABLE();
    
    /*##-2- Configure peripheral GPIO ##########################################*/  
    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = USARTx_ZIGBEE_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = USARTx_ZIGBEE_TX_AF;
    
    HAL_GPIO_Init(USARTx_ZIGBEE_TX_GPIO_PORT, &GPIO_InitStruct);
      
    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = USARTx_ZIGBEE_RX_PIN;
    GPIO_InitStruct.Alternate = USARTx_ZIGBEE_RX_AF;
      
    HAL_GPIO_Init(USARTx_ZIGBEE_RX_GPIO_PORT, &GPIO_InitStruct);
    
    /*##-3- Configure the DMA streams ##########################################*/
    /* Configure the DMA handler for Transmission process */
    hdma_zigbee_tx.Instance                 = USARTx_ZIGBEE_DMA_TX_CHANNEL;
    hdma_zigbee_tx.Init.Request             = USARTx_ZIGBEE_DMA_TX_REQUEST;
    hdma_zigbee_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_zigbee_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_zigbee_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_zigbee_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_zigbee_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_zigbee_tx.Init.Mode                = DMA_NORMAL;
    hdma_zigbee_tx.Init.Priority            = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_zigbee_tx); 

    /* Associate the initialized DMA handle to the the UART handle */   
    __HAL_LINKDMA(huart,hdmatx,hdma_zigbee_tx);

    /* Configure the DMA handler for Transmission process */
    hdma_zigbee_rx.Instance                 = USARTx_ZIGBEE_DMA_RX_CHANNEL;
    hdma_zigbee_rx.Init.Request             = USARTx_ZIGBEE_DMA_RX_REQUEST;
    hdma_zigbee_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_zigbee_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_zigbee_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_zigbee_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_zigbee_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_zigbee_rx.Init.Mode                = DMA_NORMAL;
    hdma_zigbee_rx.Init.Priority            = DMA_PRIORITY_HIGH;  
    HAL_DMA_Init(&hdma_zigbee_rx);

    /* Associate the initialized DMA handle to the the UART handle */
    __HAL_LINKDMA(huart, hdmarx, hdma_zigbee_rx);
    
    /*##-4- Configure the NVIC for DMA #########################################*/
    /* NVIC configuration for DMA transfer complete interrupt (USART1_TX) */
    HAL_NVIC_SetPriority(USARTx_ZIGBEE_DMA_TX_IRQn, 0, 1); 
    HAL_NVIC_EnableIRQ(USARTx_ZIGBEE_DMA_TX_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt (USART1_RX) */
    HAL_NVIC_SetPriority(USARTx_ZIGBEE_DMA_RX_IRQn, 0, 1);   
    HAL_NVIC_EnableIRQ(USARTx_ZIGBEE_DMA_RX_IRQn); 
    
    HAL_NVIC_SetPriority(USARTx_ZIGBEE_IRQn, 0, 1); 
    HAL_NVIC_EnableIRQ(USARTx_ZIGBEE_IRQn);
  }  
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)   
{  
  GPIO_InitTypeDef  GPIO_InitStruct;
  if(huart->Instance==USARTx_DEBUG)
  { 
     /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Disable LPUART1 clock */
    USARTx_DEBUG_CLK_DISABLE(); 
    DMAx_DEBUG_CLK_DISABLE();
    
    /*##-2- Configure peripheral GPIO ##########################################*/  
    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = USARTx_DEBUG_TX_PIN | USARTx_DEBUG_RX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;   
    HAL_GPIO_Init(USARTx_DEBUG_TX_GPIO_PORT, &GPIO_InitStruct);      
    HAL_GPIO_Init(USARTx_DEBUG_RX_GPIO_PORT, &GPIO_InitStruct);
    
    /*##-4- Disable the NVIC for DMA ###########################################*/
    HAL_NVIC_DisableIRQ(USARTx_DEBUG_DMA_TX_IRQn);
    HAL_NVIC_DisableIRQ(USARTx_DEBUG_DMA_RX_IRQn);
    HAL_NVIC_DisableIRQ(USARTx_DEBUG_IRQn);
  }
  else if(huart->Instance==USARTx_ZIGBEE)
  { 
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Disable LPUART1 clock */
    USARTx_ZIGBEE_CLK_DISABLE(); 
    DMAx_ZIGBEE_CLK_DISABLE();
    
    /*##-2- Configure peripheral GPIO ##########################################*/  
    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = USARTx_ZIGBEE_TX_PIN | USARTx_ZIGBEE_RX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;   
    HAL_GPIO_Init(USARTx_ZIGBEE_TX_GPIO_PORT, &GPIO_InitStruct);      
    HAL_GPIO_Init(USARTx_ZIGBEE_RX_GPIO_PORT, &GPIO_InitStruct);
    
    /*##-4- Disable the NVIC for DMA ###########################################*/
    HAL_NVIC_DisableIRQ(USARTx_ZIGBEE_DMA_TX_IRQn);
    HAL_NVIC_DisableIRQ(USARTx_ZIGBEE_DMA_RX_IRQn);
    HAL_NVIC_DisableIRQ(USARTx_ZIGBEE_IRQn);
  }  
}

///**
//  * @brief  DeInitializes the Global MSP.
//  * @param  None  
//  * @retval None
//  */
//void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
//{
//  static DMA_HandleTypeDef hdma_debug_rx;
//  static DMA_HandleTypeDef hdma_debug_tx;
//  
//   if(huart->Instance==USARTx_DEBUG)
//  {
//    /**USART2 GPIO Configuration   
//    PA2     ------> USART2_TX
//    PA3     ------> USART3_RX
//    */   
//    
//    /*##-1- Reset peripherals ##################################################*/
//    USARTx_DEBUG_FORCE_RESET();
//    USARTx_DEBUG_RELEASE_RESET();
//
//    /*##-2- Disable peripherals and GPIO Clocks #################################*/
//    /* Configure UART Tx as alternate function  */
//    HAL_GPIO_DeInit(USARTx_DEBUG_TX_GPIO_PORT, USARTx_DEBUG_TX_PIN);
//    /* Configure UART Rx as alternate function  */
//    HAL_GPIO_DeInit(USARTx_DEBUG_RX_GPIO_PORT, USARTx_DEBUG_RX_PIN);
//
//    /*##-3- Disable the DMA Streams ############################################*/
//    /* De-Initialize the DMA Stream associate to transmission process */
//    HAL_DMA_DeInit(&hdma_debug_tx); 
//    /* De-Initialize the DMA Stream associate to reception process */
//    HAL_DMA_DeInit(&hdma_debug_rx);
//
//    /*##-4- Disable the NVIC for DMA ###########################################*/
//    HAL_NVIC_DisableIRQ(USARTx_DEBUG_DMA_TX_IRQn);
//    HAL_NVIC_DisableIRQ(USARTx_DEBUG_DMA_RX_IRQn);
//  }
//}

void dbg_print(u8*buf,u16 len)
{
  HAL_UART_Transmit_DMA(&huartx_debug, buf, len);   
  while(__HAL_UART_GET_FLAG(&huartx_debug,UART_FLAG_TC)== RESET);  
}






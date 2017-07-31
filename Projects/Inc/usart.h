#ifndef __USART_H__
#define __USART_H__

#include "stm32l0xx_hal.h"

#define UART_MAX_RX_LEN                      100
#define UART_MAX_TX_LEN                      100
/////////////////DEBUG/////////////////////////////////////////////////////////////////
/* Definition for USARTx clock resources */
#define USARTx_DEBUG                           USART2
#define BAUND_DEBUG                            115200

#define USARTx_DEBUG_CLK_ENABLE()              __USART2_CLK_ENABLE()
#define USARTx_DEBUG_CLK_DISABLE()             __USART2_CLK_DISABLE()

#define DMAx_DEBUG_CLK_ENABLE()                __DMA1_CLK_ENABLE()
#define DMAx_DEBUG_CLK_DISABLE()               __DMA1_CLK_DISABLE()
#define USARTx_DEBUG_RX_GPIO_CLK_ENABLE()      __GPIOB_CLK_ENABLE()
#define USARTx_DEBUG_TX_GPIO_CLK_ENABLE()      __GPIOB_CLK_ENABLE()  

#define USARTx_DEBUG_FORCE_RESET()             __USART2_FORCE_RESET()
#define USARTx_DEBUG_RELEASE_RESET()           __USART2_RELEASE_RESET()


/* Definition for USARTx Pins */
#define USARTx_DEBUG_TX_PIN                    GPIO_PIN_6
#define USARTx_DEBUG_TX_GPIO_PORT              GPIOB
#define USARTx_DEBUG_TX_AF                     GPIO_AF0_USART2 
#define USARTx_DEBUG_RX_PIN                    GPIO_PIN_7
#define USARTx_DEBUG_RX_GPIO_PORT              GPIOB
#define USARTx_DEBUG_RX_AF                     GPIO_AF0_USART2

/* Definition for USARTx's DMA */
#define USARTx_DEBUG_TX_DMA_CHANNEL             DMA1_Channel4
#define USARTx_DEBUG_TX_DMA_REQUEST             DMA_REQUEST_4
#define USARTx_DEBUG_RX_DMA_CHANNEL             DMA1_Channel5
#define USARTx_DEBUG_RX_DMA_REQUEST             DMA_REQUEST_4

/* Definition for USARTx's NVIC */
#define USARTx_DEBUG_DMA_TX_IRQn                DMA1_Channel4_5_6_7_IRQn
#define USARTx_DEBUG_DMA_RX_IRQn                DMA1_Channel4_5_6_7_IRQn

/* Definition for USARTx's NVIC */
#define USARTx_DEBUG_IRQn                       USART2_IRQn
#define USARTx_DEBUG_IRQHandler                 USART2_IRQHandler

////////////////////////////////////////////////////////////////////////////////////


/////////////////ZIGBEE///////////////////////
//////////////////////////////////////////
#define USARTx_ZIGBEE                           LPUART1
#define BAUND_ZIGBEE                            115200
#define USARTx_ZIGBEE_CLK_ENABLE()              __LPUART1_CLK_ENABLE()
#define USARTx_ZIGBEE_RX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()
#define USARTx_ZIGBEE_TX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE() 
#define DMAx_ZIGBEE_CLK_ENABLE()                __DMA1_CLK_ENABLE()

#define USARTx_ZIGBEE_CLK_DISABLE()              __LPUART1_CLK_DISABLE()
#define DMAx_ZIGBEE_CLK_DISABLE()                __DMA1_CLK_DISABLE()

#define USARTx_ZIGBEE_FORCE_RESET()             __LPUART1_FORCE_RESET()
#define USARTx_ZIGBEE_RELEASE_RESET()           __LPUART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_ZIGBEE_TX_PIN                    GPIO_PIN_2
#define USARTx_ZIGBEE_TX_GPIO_PORT              GPIOA  
#define USARTx_ZIGBEE_TX_AF                     ((uint8_t)0x06U)//GPIO_AF0_LPUART1  
#define USARTx_ZIGBEE_RX_PIN                    GPIO_PIN_3
#define USARTx_ZIGBEE_RX_GPIO_PORT              GPIOA 
#define USARTx_ZIGBEE_RX_AF                     ((uint8_t)0x06U)//GPIO_AF0_LPUART1

/* Definition for USARTx's DMA */
#define USARTx_ZIGBEE_DMA_TX_CHANNEL             DMA1_Channel7
#define USARTx_ZIGBEE_DMA_TX_REQUEST             DMA_REQUEST_5
#define USARTx_ZIGBEE_DMA_RX_CHANNEL             DMA1_Channel6
#define USARTx_ZIGBEE_DMA_RX_REQUEST             DMA_REQUEST_5


/* Definition for USARTx's NVIC */
#define USARTx_ZIGBEE_DMA_TX_IRQn                DMA1_Channel4_5_6_7_IRQn
#define USARTx_ZIGBEE_DMA_RX_IRQn                DMA1_Channel4_5_6_7_IRQn

/* Definition for USARTx's NVIC */
#define USARTx_ZIGBEE_IRQn                       RNG_LPUART1_IRQn
#define USARTx_ZIGBEE_IRQHandler                 RNG_LPUART1_IRQHandler    

#define USARTx_DMA_TX_IRQHandler                 DMA1_Channel4_5_6_7_IRQHandler


extern u8 Zig_DMA_RxBuf[UART_MAX_RX_LEN];
extern UART_HandleTypeDef huartx_zigbee;
extern UART_HandleTypeDef huartx_debug;
void HAL_UARTx_Init(UART_HandleTypeDef *huartx);
void HAL_UART_MspInit(UART_HandleTypeDef* huart);
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void dbg_print(u8*buf,u16 len);

#endif

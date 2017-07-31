#ifndef __STM32L0XX_HAL_MSP_H__
#define __STM32L0XX_HAL_MSP_H__	 
#include "stm32l0xx_hal.h"
#include "main.h" 

#define USER_DATA_EEPROM_BASE       ((uint32_t)0x08080000U) /*!< DATA_EEPROM base address in the alias region */
#define USER_DATA_EEPROM_END        ((uint32_t)0x080803FFU) /*!< DATA EEPROM end address in the alias region */


/////////////////LED GPIO///////////////////////
#define LED_RUN_GPIO_CLK_ENABLE()          __GPIOA_CLK_ENABLE()
#define LED_RUN_GPIO                       GPIOA      
#define LED_RUN_PIN                        GPIO_PIN_8 

#define LED_RUN_ON()                       HAL_GPIO_WritePin(LED_RUN_GPIO, LED_RUN_PIN, GPIO_PIN_RESET)
#define LED_RUN_OFF()                      HAL_GPIO_WritePin(LED_RUN_GPIO, LED_RUN_PIN, GPIO_PIN_SET)

/* Definition for SPIx clock resources */
#define SPIx                             SPI1
#define SPIx_CLK_ENABLE()                __SPI1_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __GPIOB_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __GPIOB_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __GPIOB_CLK_ENABLE()
#define SPIx_CS_GPIO_CLK_ENABLE()        __GPIOA_CLK_ENABLE()

#define SPIx_CLK_DISABLE()                __SPI1_CLK_DISABLE()
#define SPIx_SCK_GPIO_CLK_DISABLE()       __GPIOB_CLK_DISABLE()
#define SPIx_MISO_GPIO_CLK_DISABLE()      __GPIOB_CLK_DISABLE()
#define SPIx_MOSI_GPIO_CLK_DISABLE()      __GPIOB_CLK_DISABLE()
#define SPIx_CS_GPIO_CLK_DISABLE()        __GPIOA_CLK_DISABLE()


#define SPIx_FORCE_RESET()               __HAL_RCC_SPI1_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI1_RELEASE_RESET()

/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                     GPIO_PIN_3
#define SPIx_SCK_GPIO_PORT               GPIOB
#define SPIx_SCK_AF                      GPIO_AF0_SPI1

#define SPIx_MISO_PIN                    GPIO_PIN_4
#define SPIx_MISO_GPIO_PORT              GPIOB
#define SPIx_MISO_AF                     GPIO_AF0_SPI1

#define SPIx_MOSI_PIN                    GPIO_PIN_5
#define SPIx_MOSI_GPIO_PORT              GPIOB
#define SPIx_MOSI_AF                     GPIO_AF0_SPI1

#define SPIx_CS_PIN                      GPIO_PIN_15
#define SPIx_CS_GPIO_PORT                GPIOA

#define SPI_CS_RESET()                   HAL_GPIO_WritePin(SPIx_CS_GPIO_PORT, SPIx_CS_PIN, GPIO_PIN_RESET) 
#define SPI_CS_SET()                     HAL_GPIO_WritePin(SPIx_CS_GPIO_PORT, SPIx_CS_PIN, GPIO_PIN_SET)      

/* Definition for SPIx's NVIC */
#define SPIx_IRQn                        SPI1_IRQn
#define SPIx_IRQHandler                  SPI1_IRQHandler  


#define IWDG_IRQHandler     TIM21_IRQHandler  
extern TIM_HandleTypeDef    Input_Handle;
extern IWDG_HandleTypeDef   IwdgHandle;
extern SPI_HandleTypeDef    SpiHandle;

extern volatile u8 Pvd_Flag;

void PVD_Config(void);

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void IWDOG_Init(void);
void Feed_Dog(void);

void HAL_Delay(volatile u32 Delay);
void Error_Handler(void);

void SystemClock_Config(void);
void GPIO_ANConfig(void);
void GPIOx_PINx_SET(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pinx);
void GPIOx_PINx_RESET(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pinx);

void Eeprom_Write(u32 Address,u8 *write_buf,u16 buf_len);
void Eeprom_Read(u32 Address,u8 *read_buf,u16 buf_len);

void Led_Init(void);
void Led_DeInit(void);
void Led_Run_Toggle(void);
void Led_Run_Blink(u8 fre, u16 delay_m);
void Led_Run_Error(void);

void SPI_Init(void);
void SPI_DeInit(void);

#endif



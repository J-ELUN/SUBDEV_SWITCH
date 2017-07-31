#ifndef __IR_H__
#define __IR_H__

#include "stm32l0xx_hal.h"

#define IR_GPIO_CLK_ENABLE()          __GPIOA_CLK_ENABLE()
#define IR_GPIO                       GPIOA      
#define IR_PIN                        GPIO_PIN_5 

extern TIM_HandleTypeDef htim2; 
void TIM2_PWM_Init(void); 
void TIM2_PWM_DeInit(void);
void IR_Send(void);

#endif

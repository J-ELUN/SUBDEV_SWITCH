#ifndef __SWITCH_H
#define __SWITCH_H
#include "stm32l0xx_hal.h"

#define SWITCH_CLK_ENABLE()      __GPIOA_CLK_ENABLE()	
#define SWITCH_PORT              GPIOA 

#define SWITCH_1_PIN             GPIO_PIN_4 
#define SWITCH_2_PIN             GPIO_PIN_15 
#define SWITCH_3_PIN             GPIO_PIN_12 
#define SWITCH_4_PIN             GPIO_PIN_11 
#define SWITCH_SET_PIN           GPIO_PIN_9 

#define SWITCH_IRQn              EXTI4_15_IRQn 
#define SWITCH_IRQHandler        EXTI4_15_IRQHandler



typedef enum 
{  
  SWITCH_NONE=0,			//无按键按下
  SWITCH_1_PRESSED,
  SWITCH_2_PRESSED,
  SWITCH_3_PRESSED,
  SWITCH_4_PRESSED,
  SWITCH_SET_PRESSED
}SWITCH_EVENT_TypeDef;

extern SWITCH_CHK_INFO_TYPEDEF   switch_chk_info;
extern SWITCH_CTRL_CMD_TYPEDEF   switch_ctrl_cmd;

void Switch_Init(void); 
SWITCH_EVENT_TypeDef Switch_Event_Handle(void);

#endif




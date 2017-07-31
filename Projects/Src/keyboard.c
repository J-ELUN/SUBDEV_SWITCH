#include "switch.h"
#include "zigbee.h"
#include "stm32l0xx_hal_msp.h" 


void KeyBoard_Init(void) 
{    
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOA_CLK_ENABLE();

  /*Configure GPIO pins : PA4 PA11 PA12 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15 |GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
  
  switch_ctrl_cmd.head='H';
  switch_ctrl_cmd.dev_type=DEV_SWITCH;
  memcopy(switch_ctrl_cmd.dev_id,zig_dev_info.MyIEEE,8);        
  switch_ctrl_cmd.tail='T';
 
}



/**
  * @brief GPIO EXTI callback
  * @param None
  * @retval None
  */
volatile SWITCH_EVENT_TypeDef switch_event=SWITCH_NONE;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) 
{  
  if(GPIO_Pin==GPIO_PIN_4)
  {
    switch_event=SWITCH1_PRESSED;
  }
  else if(GPIO_Pin==GPIO_PIN_11)
  {
    switch_event=SWITCH2_PRESSED;  
  }
  else if(GPIO_Pin==GPIO_PIN_12)
  {
    switch_event=SWITCH3_PRESSED; 
  }
  else if(GPIO_Pin==GPIO_PIN_15)
  {
    switch_event=SWITCH4_PRESSED;
  } 
  else if(GPIO_Pin==GPIO_PIN_9)
  {
   switch_event=SET_PRESSED;
  }   
  
}


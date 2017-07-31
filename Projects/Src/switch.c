#include "main.h"
#include "switch.h"
#include "zigbee.h"
#include "stm32l0xx_hal_msp.h" 
#include <string.h>

SWITCH_CHK_INFO_TYPEDEF   switch_chk_info;
SWITCH_CTRL_CMD_TYPEDEF   switch_ctrl_cmd;

volatile  SWITCH_EVENT_TypeDef switch_event=SWITCH_NONE;
/**
  * @brief Switch_Init
  * @param None
  * @retval None
  */
void Switch_Init(void) 
{    
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  SWITCH_CLK_ENABLE();

  /*Configure GPIO pins : PA4 PA11 PA12 PA15 */
  GPIO_InitStruct.Pin = SWITCH_1_PIN | SWITCH_2_PIN | SWITCH_3_PIN | SWITCH_4_PIN | SWITCH_SET_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SWITCH_PORT, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(SWITCH_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(SWITCH_IRQn);
  
  switch_ctrl_cmd.head='H';
  switch_ctrl_cmd.dev_type=DEV_SWITCH;
  memcpy(switch_ctrl_cmd.dev_id,zig_dev_info.MyIEEE,8);        
  switch_ctrl_cmd.tail='T';
 
}

/**
  * @brief GPIO EXTI callback
  * @param None
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) 
{    
  switch (GPIO_Pin)
  {
    case SWITCH_1_PIN:  
      switch_event=SWITCH_1_PRESSED;
      break;
    case SWITCH_2_PIN:  
      switch_event=SWITCH_2_PRESSED;
      break;
    case SWITCH_3_PIN:  
      switch_event=SWITCH_3_PRESSED;
      break;
    case SWITCH_4_PIN:  
      switch_event=SWITCH_4_PRESSED;
      break;
    case SWITCH_SET_PIN:  
      switch_event=SWITCH_SET_PRESSED;
      break;
    default:
      break;
  } 
}

/**
  * @brief Switch_Event_Handle
  * @param None
  * @retval None
  */
SWITCH_EVENT_TypeDef Switch_Event_Handle(void)
{ 
  SWITCH_EVENT_TypeDef  switch_event_ret=SWITCH_NONE;
  switch (switch_event)
  {
    case SWITCH_1_PRESSED:  
      switch_ctrl_cmd.switch_id=S0_ID;
      Zig_Send_Data((u8*)&switch_ctrl_cmd,sizeof(SWITCH_CTRL_CMD_TYPEDEF)); 
      switch_event_ret=SWITCH_1_PRESSED;
      break;
      
    case SWITCH_2_PRESSED:  
      switch_ctrl_cmd.switch_id=S1_ID;
      Zig_Send_Data((u8*)&switch_ctrl_cmd,sizeof(SWITCH_CTRL_CMD_TYPEDEF)); 
      switch_event_ret=SWITCH_2_PRESSED;
      break;
      
    case SWITCH_3_PRESSED:  
      switch_ctrl_cmd.switch_id=S2_ID;
      Zig_Send_Data((u8*)&switch_ctrl_cmd,sizeof(SWITCH_CTRL_CMD_TYPEDEF)); 
      switch_event_ret=SWITCH_3_PRESSED;
      break;
      
    case SWITCH_4_PRESSED:  
      switch_ctrl_cmd.switch_id=S3_ID;
      Zig_Send_Data((u8*)&switch_ctrl_cmd,sizeof(SWITCH_CTRL_CMD_TYPEDEF));
      switch_event_ret=SWITCH_4_PRESSED;
      break;
      
    case SWITCH_SET_PRESSED:  
      Led_Run_Blink(3,500);  //间隔500ms闪烁3次提示进入绑定状态。      
      if(Zigbee_Bind_Handle()!=STEP_BIND_OK)
         Zigbee_Set_BackUpInfo(); 
      break;
      
    default:
      break;  
  } 
  switch_event=SWITCH_NONE;
  return switch_event_ret;
}



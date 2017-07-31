#include "ir.h"
#include "stm32l0xx_hal_msp.h"    
#include "main.h"

TIM_HandleTypeDef htim2;

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_pwm)
{

  if(htim_pwm->Instance==TIM2)   
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* Peripheral clock enable */
    __TIM2_CLK_ENABLE();
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }

}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* htim_pwm)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(htim_pwm->Instance==TIM2)
  {
    __TIM2_CLK_DISABLE();
    GPIO_InitStruct.Pin = IR_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(IR_GPIO, &GPIO_InitStruct);
  }
}

static void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(htim->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspPostInit 0 */

  /* USER CODE END TIM2_MspPostInit 0 */
  
    /**TIM2 GPIO Configuration    
    PA5     ------> TIM2_CH1 
    */
    IR_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = IR_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF5_TIM2;
    HAL_GPIO_Init(IR_GPIO, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM2_MspPostInit 1 */

  /* USER CODE END TIM2_MspPostInit 1 */
  }

}

/* TIM2 init function */
void TIM2_PWM_Init(void)  //2MHZ
{
  TIM_OC_InitTypeDef sConfigOC;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 2000/38;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 2000/38/2;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim2);
}

void TIM2_PWM_DeInit(void)
{
  HAL_TIM_PWM_DeInit(&htim2);
}

static void TIM2_PWM_ENABLE(void)
{
  if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1) != HAL_OK)
  {
    /* PWM Generation Error */
    Error_Handler();
  }
}

static void TIM2_PWM_DISABLE(void)
{
  if (HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1) != HAL_OK)
  {
    /* PWM Generation Error */
    Error_Handler();
  }
}


void IR_Send_Byte(u8 getdata) 
{ 
  u8 senddata,i; 
  senddata=getdata; 
  for(i=0;i<8;i++) 
  { 
    if(senddata&0x01) 
    { 
      //1 
      TIM2_PWM_ENABLE(); 
      HAL_Delay_100us(6); 
      TIM2_PWM_DISABLE(); 
      HAL_Delay_100us(5);  
      HAL_Delay_100us(5); 
      HAL_Delay_100us(6); 
    } 
    else 
    { 
      //0 
      TIM2_PWM_ENABLE(); 
      HAL_Delay_100us(6); 
      TIM2_PWM_DISABLE(); 
      HAL_Delay_100us(6); 
    } 
    senddata=senddata>>1; 
  } 
}


void IR_Send1(void) 
{ 
  //9ms 
  TIM2_PWM_ENABLE(); 
  HAL_Delay(9);
  //4.5ms 
  TIM2_PWM_DISABLE(); 
  HAL_Delay(4);
 
  IR_Send_Byte(0x12);	//用户码 
  IR_Send_Byte(0x34);	//用户反码 
  IR_Send_Byte(0x56);	//数据码 
  IR_Send_Byte(0x78);	//数据反码 
  
  //下面增加是为了避免在发送最后一位时出现错码 
  TIM2_PWM_ENABLE(); 
  HAL_Delay_100us(6); 
  TIM2_PWM_DISABLE(); 
  HAL_Delay_100us(6); 
} 

void IR_Send2(void) 
{ 
  //9ms 
  TIM2_PWM_ENABLE(); 
  HAL_Delay(9);
  //4.5ms 
  TIM2_PWM_DISABLE(); 
  HAL_Delay(4);
  IR_Send_Byte(0x78);	//用户码 
  IR_Send_Byte(0x56);	//用户反码 
  IR_Send_Byte(0x32);	//数据码 
  IR_Send_Byte(0x12);	//数据反码 
  
  //下面增加是为了避免在发送最后一位时出现错码 
  TIM2_PWM_ENABLE(); 
  HAL_Delay_100us(6); 
  TIM2_PWM_DISABLE(); 
  HAL_Delay_100us(6); 
} 



u8 NECGREE_check(u8 * dat)
{
  u8 chk;
  chk=(*dat&0x07)-1+(*(dat+1)&0x0f)+5+(*(dat+4)&0x10)+(*(dat+3)&0x01);
  chk&=0x0f;
  chk<<=4;
  return chk;
}

void NECGREE_Send_link(void)
{
 
  //0.7ms 1
  TIM2_PWM_ENABLE(); 
  HAL_Delay_100us(7); 
  TIM2_PWM_DISABLE(); 
  HAL_Delay(20);  
}

//发送0
void NECGREE_Send_Bit_0(void)
{
  //0 
  TIM2_PWM_ENABLE(); 
  HAL_Delay_100us(6); 
  TIM2_PWM_DISABLE(); 
  HAL_Delay_100us(6); 

}

//发送1
void NECGREE_Send_Bit_1(void)
{
  TIM2_PWM_ENABLE(); 
  HAL_Delay_100us(6); 
  TIM2_PWM_DISABLE(); 
  HAL_Delay_100us(5);  
  HAL_Delay_100us(5); 
  HAL_Delay_100us(6); 
}

//发送引导码  发送方:9ms高电平 4.5ms低电平
void NECGREE_Send_Start_Bit(void)    
{
  //9ms 
  TIM2_PWM_ENABLE(); 
  HAL_Delay(9);
  //4.5ms 
  TIM2_PWM_DISABLE(); 
  HAL_Delay(4);
}

void NECGREE_Send_Char(u8 * dat)
{
  u8 j;
  NECGREE_Send_Start_Bit();
  for(j=0; j<4; j++)
  {
    IR_Send_Byte(*(dat+j));
  }
  NECGREE_Send_Bit_0();
  NECGREE_Send_Bit_1();
  NECGREE_Send_Bit_0();
  NECGREE_Send_link();
  for(j=4; j<8; j++)
  {
    IR_Send_Byte(*(dat+j));
  }
  NECGREE_Send_Bit_0();
}

void NECGREE_Send_Data(AIR_CTRL_TYPEDEF *p_air_ctrl)
{
	u8 GREE[8] = {0x0c,0x00,0x00,0x50,0x00,0x20,0x00,0x88};
	u8 NECGREE_temp=0;
	if(p_air_ctrl->power_sta==POWER_OFF)		//开机进入设置模式
	{
		GREE[0]&=(~(1<<3));

		GREE[7]&=0x0f;
		GREE[7]|=NECGREE_check(GREE);

		GREE[7]-=0x80;
		NECGREE_Send_Char( GREE );
		
	}
	else
	{
		/*****温度******/
		NECGREE_temp=(p_air_ctrl->Set_Temp[0]-0x30)*10 + p_air_ctrl->Set_Temp[1]-0x30;
		if(NECGREE_temp<16) NECGREE_temp=16;
		if(NECGREE_temp>30) NECGREE_temp=30;
		
		GREE[1]&=0xf0;										
		GREE[1]|=(NECGREE_temp-16);
		
		GREE[7]&=0x0f;
		GREE[7]|=NECGREE_check(GREE);
		
		/*****模式******/
		GREE[0]&=0xf8;
		GREE[0]|=p_air_ctrl->Work_Mode;
		
		GREE[7]&=0x0f;
		GREE[7]|=NECGREE_check(GREE);
		
		/*****风速******/
		GREE[0]&=0xcf;
		GREE[0]|=((p_air_ctrl->Fan_Speed)<<4);
		
		GREE[7]&=0x0f;
		GREE[7]|=NECGREE_check(GREE);
		
		NECGREE_Send_Char( GREE ); 
	}	
}
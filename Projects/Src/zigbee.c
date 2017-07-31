#include "main.h"
#include "zigbee.h"
#include "stm32l0xx_hal_msp.h"
#include "switch.h"
#include <string.h>
#include <stdio.h>


ZIG_DEV_INFO_TYPEDEF   zig_dev_info={0};
ZIG_BACKUP_DEV_INFO_TYPEDEF		zig_backup_dev_info={0};
ZIG_RX_WORKPANID_DSTADDR_TYPEDEF 	zig_rx_workpanid_dstaddr={0};

ZIG_UART_BUF_TYPEDEF  Zig_Uart_Buf;
EEPROM_BACKUP_INFO_TYPEDEF eeprom_backup_info;


void Zigbee_Uart_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable the GPIO Clock */  
  ZIG_WAKE_GPIO_CLK_ENABLE();

  /* Configure the GPIO pin */
  GPIO_InitStruct.Pin = ZIG_WAKE_PIN ;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(ZIG_WAKE_GPIO, &GPIO_InitStruct);   
  Zigbee_WakeUp(); 
  
  /* Configure the Uart */
  HAL_UARTx_Init(&huartx_zigbee);
}
/**
  * @brief  Zigbee_Init.
  * @param  None
  * @retval None
  */
void Zigbee_Init(void)
{
  Zigbee_Uart_Init();
  
  if(Zigbee_Get_Set_Info()!=RET_OK)
    Led_Run_Error();
}

void Zigbee_DeInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Configure the GPIO pin */
  GPIO_InitStruct.Pin = ZIG_WAKE_PIN ;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ZIG_WAKE_GPIO, &GPIO_InitStruct);   
  
  /* Configure the Uart */
  HAL_UART_DeInit(&huartx_zigbee);
}

/**
  * @brief  Zigbee_Sleep.SLEEP引脚下降沿有效(>1ms)
  * @param  None
  * @retval None
  */
void Zigbee_Sleep(void)
{  
  HAL_Delay(5);  //发送完数据不能立即进入sleep
  Zig_Send_Cmd(Zig_Uart_Buf.Tx_Buf,CMD_ENTER_SLEEP);  
}

/**
  * @brief  Zigbee_WakeUp.WAKE引脚下降沿有效(>1ms)
  * @param  None
  * @retval None
  */
void Zigbee_WakeUp(void)
{
  GPIOx_PINx_SET(ZIG_WAKE_GPIO, ZIG_WAKE_PIN);
  HAL_Delay(2);
  GPIOx_PINx_RESET(ZIG_WAKE_GPIO, ZIG_WAKE_PIN);
  HAL_Delay(2);
}


void Eeprom_Read_DEV_INFO(EEPROM_BACKUP_INFO_TYPEDEF *p_eeprom_backup_info)
{
  Eeprom_Read(USER_DATA_EEPROM_BASE,(u8*)p_eeprom_backup_info,sizeof(EEPROM_BACKUP_INFO_TYPEDEF));
  
  switch_chk_info.switch_num=(u8)p_eeprom_backup_info->eeprom_switch_num;
  if(switch_chk_info.switch_num==0 || switch_chk_info.switch_num > MAX_SWITCH_NUM)
  {
    switch_chk_info.switch_num=MAX_SWITCH_NUM;
    p_eeprom_backup_info->eeprom_switch_num=switch_chk_info.switch_num;
    Eeprom_Write(USER_DATA_EEPROM_BASE,(u8*)p_eeprom_backup_info,sizeof(EEPROM_BACKUP_INFO_TYPEDEF));
  }
}


/**
  * @brief  求和校验
  * @param  p_buf：point to buf
  * @param  len 
  * @retval None
  */
u8 Sum_Check(u8 *p_buf,u16 len)
{
  u16 i;
  u16 sum_chk=0;
  
  for(i=0;i<len;i++)
    sum_chk+=p_buf[i];
  return ((u8)sum_chk);	
}


void Zig_Send_Data(u8* pbuf,u16 len)
{ 	
  HAL_UART_Transmit_DMA(&huartx_zigbee, pbuf, len);   
  while(__HAL_UART_GET_FLAG(&huartx_zigbee,UART_FLAG_TC)== RESET); 
}


static ACK_TYPEDEF  Zig_RxCmd_Analysis(u8 *pbuf,u16 len)
{
  ZIG_GET_MYINFO_ACK_TYPEDEF *p_zig_get_myinfo_ack=NULL;
  ZIG_CFG_DEV_INFO_ACK_TYPEDEF 	*p_zig_cfg_dev_info_ack=NULL;
  ZIG_MODIFY_DEV_DSTADDR_ACK_TYPEDEF 	 *p_zig_modify_dev_dstaddr_ack=NULL;	 
  ZIG_RX_WORKPANID_DSTADDR_TYPEDEF     *p_zig_rx_workpanid_dstaddr=NULL;
  ZIG_CHK_SUBDEVONLINE_STATE_ACK_TYPEDEF  *p_zig_chk_subdevonline_state_ack=NULL;
  u8 function_code;
  ACK_TYPEDEF ack_ret=CMD_NONE_ACK;
  function_code=pbuf[3];
  
  switch (function_code)
  {
    case CMD_GET_DEV_INFO_ACK:		//获取本地配置响应
      if(len==sizeof(ZIG_GET_MYINFO_ACK_TYPEDEF))
      {
        p_zig_get_myinfo_ack=(ZIG_GET_MYINFO_ACK_TYPEDEF *)pbuf;			
        memcpy((u8*)&zig_dev_info,(u8 const*)&(p_zig_get_myinfo_ack->zig_dev_info) ,sizeof(ZIG_DEV_INFO_TYPEDEF));
        zig_backup_dev_info.DstAddr=zig_dev_info.DstAddr;
        zig_backup_dev_info.PanID=zig_dev_info.PanID;
        ack_ret=CMD_GET_DEV_INFO_ACK;
      }					
    break;

    case CMD_CFG_DEV_INFO_ACK:	 //修改配置响应
      p_zig_cfg_dev_info_ack=(ZIG_CFG_DEV_INFO_ACK_TYPEDEF*)pbuf;
      if(p_zig_cfg_dev_info_ack->respond_state==OK)
      {
        Zig_Send_Cmd(Zig_Uart_Buf.Tx_Buf,CMD_RESET_DEV_INFO);			//配置成功后发送复位命令
        ack_ret=CMD_CFG_DEV_INFO_ACK;
      }													
    break;

    case CMD_MODIFY_DEV_DSTADDR_ACK:		//修改目的地址响应
      p_zig_modify_dev_dstaddr_ack=(ZIG_MODIFY_DEV_DSTADDR_ACK_TYPEDEF *)pbuf;
      if(p_zig_modify_dev_dstaddr_ack->respond_state==OK)	//respond_state=0成功,其他失败					
        ack_ret=CMD_MODIFY_DEV_DSTADDR_ACK;																			
    break;

    case DATA_RX_WORKPANID_DSTADDR_ACK:		//收到网关发来的work_panid和dstaddr			
      p_zig_rx_workpanid_dstaddr=(ZIG_RX_WORKPANID_DSTADDR_TYPEDEF *)pbuf;
      if(p_zig_rx_workpanid_dstaddr->sum_chk==Sum_Check(pbuf,len-1))	
      {				
        //保存workid和dstadd
        zig_rx_workpanid_dstaddr.work_panid=p_zig_rx_workpanid_dstaddr->work_panid;
        zig_rx_workpanid_dstaddr.dst_addr=p_zig_rx_workpanid_dstaddr->dst_addr;				
        ack_ret=DATA_RX_WORKPANID_DSTADDR_ACK;
      }																			
    break;

    case SUBCMD_CHK_SUBDEVONLINE_STATE_ACK:			//检查设备是否在线
      p_zig_chk_subdevonline_state_ack=(ZIG_CHK_SUBDEVONLINE_STATE_ACK_TYPEDEF *)pbuf;
      if(p_zig_chk_subdevonline_state_ack->sum_chk==Sum_Check(pbuf,len-1))
      {
        Zig_Send_Cmd(Zig_Uart_Buf.Tx_Buf,SUBCMD_CHK_SUBDEVONLINE_STATE_ACK);	//发送ACK
        ack_ret=SUBCMD_CHK_SUBDEVONLINE_STATE_ACK;		
      }					
    break;

    default:
    break;
  }
  return ack_ret;
}


ACK_TYPEDEF Zig_Send_Cmd(u8* pbuf,CMD_TYPEDEF cmd_type)
{	
  u16  index=0;
  u8 over_time=0;
  
  ACK_TYPEDEF ack_ret=CMD_NONE_ACK;
  switch (cmd_type)
  {
    //获取本地设备信息
    case CMD_GET_DEV_INFO:
      index=0;
      pbuf[index]=0xAB;index++;
      pbuf[index]=0xBC;index++;
      pbuf[index]=0xCD;index++;
      pbuf[index]=CMD_GET_DEV_INFO;index++;	
      pbuf[index]=0x05;index++;	
      Zig_Send_Data(pbuf,index);
    break;

    //配置设备MYADDR
    case SUBCMD_CFG_DEV_MYADDR:
      index=0;
      pbuf[index]=0xAB;index++;
      pbuf[index]=0xBC;index++;
      pbuf[index]=0xCD;index++;
      pbuf[index]=CMD_CFG_DEV_INFO;index++;
      pbuf[index]=(u8)zig_dev_info.MyAddr;index++;	
      pbuf[index]=(u8)(zig_dev_info.MyAddr>>8);	index++;		
      
      zig_dev_info.MyAddr=(u16)(zig_dev_info.MyIEEE[1]<<8)+zig_dev_info.MyIEEE[3];	
      
      memcpy(pbuf+index,(u8 const*)&zig_dev_info,sizeof(ZIG_DEV_INFO_TYPEDEF));index+=	sizeof(ZIG_DEV_INFO_TYPEDEF);					
      pbuf[index]=Sum_Check(pbuf,index);index++;
      Zig_Send_Data(pbuf,index);
    break;

    //配置设备PANID为Search_PanID
    case SUBCMD_CFG_DEV_PANID_SEARCH:
      index=0;
      pbuf[index]=0xAB;index++;
      pbuf[index]=0xBC;index++;
      pbuf[index]=0xCD;index++;
      pbuf[index]=CMD_CFG_DEV_INFO;index++;
      pbuf[index]=(u8)zig_dev_info.MyAddr;index++;		
      pbuf[index]=(u8)(zig_dev_info.MyAddr>>8);index++;
      
      zig_dev_info.PanID=SEARCH_PANID;
      
      memcpy(pbuf+index,(u8 const*)&zig_dev_info,sizeof(ZIG_DEV_INFO_TYPEDEF));index+=sizeof(ZIG_DEV_INFO_TYPEDEF);			
      pbuf[index]=Sum_Check(pbuf,index);index++;
      Zig_Send_Data(pbuf,index);
    break;

    //修改DSTADDR
    case CMD_MODIFY_DEV_DSTADDR:
      index=0;
      pbuf[index]=0xDE;index++;
      pbuf[index]=0xDF;index++;
      pbuf[index]=0xEF;index++;
      pbuf[index]=CMD_MODIFY_DEV_DSTADDR;	index++;			
      pbuf[index]=(u8)zig_rx_workpanid_dstaddr.dst_addr;index++;
      pbuf[index]=(u8)(zig_rx_workpanid_dstaddr.dst_addr>>8);	index++;	//目的地址
      Zig_Send_Data(pbuf,index);
    break;

    //发送my Devinfo给网关
    case DATA_TX_MY_DEVINFO:
      index=0;
      pbuf[index]=0xFE;index++;
      pbuf[index]=0xED;index++;
      pbuf[index]=0xDF;index++;
      pbuf[index]=DATA_TX_MY_DEVINFO;index++;							
      pbuf[index]=DEV_TYPE;index++;		//设备类型			
      pbuf[index]=(u8)zig_dev_info.MyAddr;index++;
      pbuf[index]=(u8)(zig_dev_info.MyAddr>>8);index++;
          
      memcpy(pbuf+index,(u8 const*)&zig_dev_info.MyIEEE,sizeof(zig_dev_info.MyIEEE));index+=sizeof(zig_dev_info.MyIEEE);
      pbuf[index]=switch_chk_info.switch_num;index++;
      pbuf[index]=Sum_Check(pbuf,index);index++;		
      Zig_Send_Data(pbuf,index);
    break;

    //配置设备WORKPANID和DSTADDR
    case SUBCMD_CFG_DEV_WORKPANID_DSTADDR:
      index=0;
      pbuf[index]=0xAB;index++;
      pbuf[index]=0xBC;index++;
      pbuf[index]=0xCD;index++;
      pbuf[index]=CMD_CFG_DEV_INFO;index++;
      pbuf[index]=(u8)zig_dev_info.MyAddr;index++;		
      pbuf[index]=(u8)(zig_dev_info.MyAddr>>8);index++;

      zig_dev_info.PanID=zig_rx_workpanid_dstaddr.work_panid;
      zig_dev_info.DstAddr=zig_rx_workpanid_dstaddr.dst_addr;
      
      memcpy(pbuf+index,(u8 const*)&zig_dev_info,sizeof(ZIG_DEV_INFO_TYPEDEF));index+=sizeof(ZIG_DEV_INFO_TYPEDEF);	
      pbuf[index]=Sum_Check(pbuf,index);index++;
      Zig_Send_Data(pbuf,index);
    break;
    
  //配置设备PANID和DSTADD为备份值
    case SUBCMD_CFG_DEV_BACKUP_PANID_DSTADDR:
      index=0;
      pbuf[index]=0xAB;index++;
      pbuf[index]=0xBC;index++;
      pbuf[index]=0xCD;index++;
      pbuf[index]=CMD_CFG_DEV_INFO;index++;
      pbuf[index]=(u8)zig_dev_info.MyAddr;index++;		
      pbuf[index]=(u8)(zig_dev_info.MyAddr>>8);index++;
     
      Eeprom_Read_DEV_INFO(&eeprom_backup_info);   
      zig_dev_info.PanID=eeprom_backup_info.eeprom_zigbee_panid_dstaddr>>16;
      zig_dev_info.DstAddr=eeprom_backup_info.eeprom_zigbee_panid_dstaddr&0x0000ffff;	
      
      memcpy(pbuf+index,(u8 const*)&zig_dev_info,sizeof(ZIG_DEV_INFO_TYPEDEF));index+=sizeof(ZIG_DEV_INFO_TYPEDEF);	
      pbuf[index]=Sum_Check(pbuf,index);index++;
      Zig_Send_Data(pbuf,index);
    break;

    case SUBCMD_CHK_SUBDEVONLINE_STATE_ACK:
      index=0;
      pbuf[index]=0xFE;index++;
      pbuf[index]=0xED;index++;
      pbuf[index]=0xDF;index++;
      pbuf[index]=SUBCMD_CHK_SUBDEVONLINE_STATE_ACK;index++;
      pbuf[index]=Sum_Check(pbuf,index);index++;		
      Zig_Send_Data(pbuf,index);
    break;

    //复位
    case CMD_RESET_DEV_INFO:
      index=0;
      pbuf[index]=0xAB;index++;
      pbuf[index]=0xBC;index++;
      pbuf[index]=0xCD;index++;
      pbuf[index]=0xd9;index++;				
      pbuf[index]=pbuf[index];index++;//(u8)zig_dev_info.MyAddr;
      pbuf[index]=pbuf[index];index++;//(u8)(zig_dev_info.MyAddr>>8);
      pbuf[index]=0x00;index++;
      pbuf[index]=0x01;index++;
      pbuf[index]=Sum_Check(pbuf,index);index++;
      Zig_Send_Data(pbuf,index);
    break;
    
    //进入休眠
    case CMD_ENTER_SLEEP:
      index=0;
      pbuf[index]=0xDE;index++;
      pbuf[index]=0xDF;index++;
      pbuf[index]=0xEF;index++;
      pbuf[index]=CMD_ENTER_SLEEP;index++;				
      pbuf[index]=0x01;index++;
      Zig_Send_Data(pbuf,index);
    break;    

    default:
    break;
  }

  if(cmd_type!=CMD_RESET_DEV_INFO && cmd_type!=DATA_TX_MY_DEVINFO && cmd_type!=SUBCMD_CHK_SUBDEVONLINE_STATE_ACK && cmd_type!=CMD_ENTER_SLEEP) //发送复位命令,进入休眠模式和发送DEVINFO信息给网关没有ACK
  {
    while(over_time<250)
    {
      if(Zig_Uart_Buf.Rx_Len>0)
      {
        ack_ret=Zig_RxCmd_Analysis(Zig_Uart_Buf.Rx_Buf,Zig_Uart_Buf.Rx_Len);
        Zig_Uart_Buf.Rx_Len=0;
        break;
      }
      else
      {        
        over_time++;
        HAL_Delay(2);
      }
    }
  }
  return ack_ret;	
}


ZIG_BIND_STEP_TYPEDEF Zigbee_Bind_Handle(void)
{
  ZIG_BIND_STEP_TYPEDEF  zig_bind_step=STEP_CFG_DEV_PANID_SEARCH;
  u16 bind_over_time=0;
  
  Led_Init();
  while(zig_bind_step!=STEP_BIND_OK && zig_bind_step!=OVERTIME_FAUSE)
  {
    switch (zig_bind_step)
    {
      case STEP_CFG_DEV_PANID_SEARCH:
        if(Zig_Send_Cmd(Zig_Uart_Buf.Tx_Buf,SUBCMD_CFG_DEV_PANID_SEARCH)==CMD_CFG_DEV_INFO_ACK)//设置网络号为Search PanID(广播搜索)
        {
          zig_bind_step=STEP_RX_WORKPANID_DSTADDR;	
        }                      
        else
        {
          zig_bind_step=STEP_CFG_DEV_PANID_SEARCH;	
          bind_over_time++;
        }
      break;

      case STEP_RX_WORKPANID_DSTADDR:		//等待接收Work_PanID和网关的MyAddr
        bind_over_time=0;        
        while(bind_over_time<BIND_OVER_TIME*10)
        {
          if(Zig_Uart_Buf.Rx_Len>0)
          {
            if(Zig_RxCmd_Analysis(Zig_Uart_Buf.Rx_Buf,Zig_Uart_Buf.Rx_Len)==	DATA_RX_WORKPANID_DSTADDR_ACK)
            {
              zig_bind_step=STEP_CFG_DEV_DSTADDR;
              Zig_Uart_Buf.Rx_Len=0;
              break;
            }
            Zig_Uart_Buf.Rx_Len=0;
          }
          bind_over_time++;
          HAL_Delay(100);
          Led_Run_Toggle();  //100ms闪烁
        }
        
        if(bind_over_time==BIND_OVER_TIME*10)  //超时        
          zig_bind_step=OVERTIME_FAUSE;	       
        bind_over_time=0;
      break;

      case STEP_CFG_DEV_DSTADDR:				
        if(Zig_Send_Cmd(Zig_Uart_Buf.Tx_Buf,CMD_MODIFY_DEV_DSTADDR)==CMD_MODIFY_DEV_DSTADDR_ACK) 		//修改DstAddr,发送my devinfo
          zig_bind_step=STEP_SEND_DEV_INFO;
        else
        {
          zig_bind_step=STEP_CFG_DEV_DSTADDR;	
          bind_over_time++;	
        }
      break;

      case STEP_SEND_DEV_INFO:
        Zig_Send_Cmd(Zig_Uart_Buf.Tx_Buf,DATA_TX_MY_DEVINFO);		//成功修改目的地址后发送my devinfo 无ACK
        zig_bind_step=STEP_CFG_DEV_WORKPANID_DSTADDR;
      break;			

      case STEP_CFG_DEV_WORKPANID_DSTADDR:
        if(Zig_Send_Cmd(Zig_Uart_Buf.Tx_Buf,SUBCMD_CFG_DEV_WORKPANID_DSTADDR)==CMD_CFG_DEV_INFO_ACK) 		//修改PANID和DstAddr
        {
          zig_bind_step=STEP_BIND_OK;          
          eeprom_backup_info.eeprom_zigbee_panid_dstaddr=(u32)(zig_rx_workpanid_dstaddr.work_panid<<16) | zig_rx_workpanid_dstaddr.dst_addr;
          Eeprom_Write(USER_DATA_EEPROM_BASE,(u8*)&eeprom_backup_info,sizeof(EEPROM_BACKUP_INFO_TYPEDEF));
        }        
        else
        {
          zig_bind_step=STEP_CFG_DEV_WORKPANID_DSTADDR;	
          bind_over_time++;
        }	
      break;
      
      default:
      break;
    }
    Led_Run_Toggle();
    HAL_Delay(100);
    if(bind_over_time>(BIND_OVER_TIME*2))
      zig_bind_step=OVERTIME_FAUSE;	    
  }
  if(zig_bind_step==OVERTIME_FAUSE)
    LED_RUN_ON();  //绑定失败led常亮
  else
    LED_RUN_OFF();//绑定成功led熄灭
  
  Led_DeInit();
  return zig_bind_step;
}

RESULT_TYPEDEF Zigbee_Get_Set_Info(void)
{
  RESULT_TYPEDEF ret=RET_OK;
  u16 set_myaddr=0;
  
  if(Zig_Send_Cmd(Zig_Uart_Buf.Tx_Buf,CMD_GET_DEV_INFO)==CMD_GET_DEV_INFO_ACK)
  {
    set_myaddr=(u16)(zig_dev_info.MyIEEE[1]<<8)+zig_dev_info.MyIEEE[3];	
    if(zig_dev_info.MyAddr!=set_myaddr)
    {
      if(Zig_Send_Cmd(Zig_Uart_Buf.Tx_Buf,SUBCMD_CFG_DEV_MYADDR)!=CMD_CFG_DEV_INFO_ACK)	//获取zigbee配置信息后配置MyAddr(取MAC其中2个字节)
        ret=RET_ERR;
    }
    
    Eeprom_Read_DEV_INFO(&eeprom_backup_info);
    if(eeprom_backup_info.eeprom_zigbee_panid_dstaddr!=0 && eeprom_backup_info.eeprom_zigbee_panid_dstaddr!=0xffffffff)
    {
      if(Zig_Send_Cmd(Zig_Uart_Buf.Tx_Buf,SUBCMD_CFG_DEV_BACKUP_PANID_DSTADDR)!=CMD_CFG_DEV_INFO_ACK)	//配置设备PANID和DSTADD为备份值
        ret=RET_ERR; 
    }
  }
  else
  {
    ret=RET_ERR;  
  }
   
  return ret;
}

void Zigbee_Set_BackUpInfo(void)
{
  if(zig_dev_info.PanID==SEARCH_PANID)
  {
    if(Zig_Send_Cmd(Zig_Uart_Buf.Tx_Buf,SUBCMD_CFG_DEV_BACKUP_PANID_DSTADDR)!=CMD_CFG_DEV_INFO_ACK)	//配置设备PANID和DSTADD为备份值
    {
      Led_Run_Error();  
    }
  }				
}

void Zig_RxData_Analysis(ZIG_UART_BUF_TYPEDEF *p_zig_uart_buf)
{  
  u8 i;
  SWITCH_CHK_CMD_TYPEDEF *p_switch_chk_cmd=NULL;
  
  if(p_zig_uart_buf->Rx_Len>0)
  {
    if(p_zig_uart_buf->Rx_Len == sizeof(SWITCH_CHK_CMD_TYPEDEF))
    {
      p_switch_chk_cmd=(SWITCH_CHK_CMD_TYPEDEF*)p_zig_uart_buf->Rx_Buf;
      if(p_switch_chk_cmd->head=='H' && p_switch_chk_cmd->tail=='T')
      {
        switch_chk_info.head=p_switch_chk_cmd->head;
        switch_chk_info.dev_type=p_switch_chk_cmd->dev_type;
        memcpy(switch_chk_info.dev_id,p_switch_chk_cmd->dev_id,8);        
        for(i=0;i<MAX_SWITCH_NUM;i++)
        {
          if(i==0)
            switch_chk_info.switch_id[i]=S0_ID; 
          else if(i==1)
            switch_chk_info.switch_id[i]=S1_ID;  
          else if(i==2)
            switch_chk_info.switch_id[i]=S2_ID;    
          else if(i==3)
            switch_chk_info.switch_id[i]=S3_ID;             
        }
        switch_chk_info.tail=p_switch_chk_cmd->tail;        
        Zig_Send_Data((u8*)&switch_chk_info,sizeof(SWITCH_CHK_INFO_TYPEDEF));
      }
    }                    
    p_zig_uart_buf->Rx_Len=0;
  }
}











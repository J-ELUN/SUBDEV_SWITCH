#ifndef __ZIGBEE_H__
#define __ZIGBEE_H__
#include "stm32l0xx_hal.h"
#include "main.h"
#include "usart.h"

/////////////////GPIO///////////////////////
#define ZIG_WAKE_GPIO_CLK_ENABLE()          __GPIOA_CLK_ENABLE()
#define ZIG_SLEEP_GPIO_CLK_ENABLE()         __GPIOA_CLK_ENABLE()
#define ZIG_RESET_GPIO_CLK_ENABLE()         __GPIOA_CLK_ENABLE()

#define ZIG_WAKE_GPIO                       GPIOA      
#define ZIG_WAKE_PIN                        GPIO_PIN_7 

#define ZIG_SLEEP_GPIO                      GPIOA      
#define ZIG_SLEEP_PIN                       GPIO_PIN_10 

#define ZIG_RESET_GPIO                      GPIOA      
#define ZIG_RESET_PIN                       GPIO_PIN_8

/******Zigbee基本信息****************/
#define DEV_TYPE    			    DEV_SWITCH 
#define SEARCH_PANID 			    0x0FF0	
#define BIND_OVER_TIME			    30	

	  			
//命令类型
typedef enum 
{
  CMD_GET_DEV_INFO=0xD1,		//获取设备参数信息命令
  CMD_MODIFY_DEV_DSTADDR=0xD2,	//修改目的地址命令	 
  CMD_CFG_DEV_INFO=0xD6,			//配置设备参数信息命令
  CMD_ENTER_SLEEP=0xD8,     //进入休眠
  CMD_RESET_DEV_INFO=0xD9,			//复位设备命令			
  SUBCMD_CFG_DEV_WORKPANID_DSTADDR=0x10,			 		//自定义配置命令，设置panid
  SUBCMD_CFG_DEV_MYADDR=0x11,					//自定义配置命令，设置myaddr
  SUBCMD_CFG_DEV_PANID_SEARCH=0x12,	 //自定义配置命令，设置panid为Search PanID
  DATA_TX_MY_DEVINFO=0x13,
  SUBCMD_CFG_DEV_BACKUP_PANID_DSTADDR=0x14,	 //自定义配置命令，设置panid为Search PanID		
}CMD_TYPEDEF;	

//ACK类型
typedef enum 
{
  CMD_NONE_ACK=0,
  CMD_GET_DEV_INFO_ACK=0xD1,
  CMD_MODIFY_DEV_DSTADDR_ACK=0xD2,		 
  CMD_CFG_DEV_INFO_ACK=0xD6,
  DATA_RX_WORKPANID_DSTADDR_ACK=0x13,
  SUBCMD_CHK_SUBDEVONLINE_STATE_ACK=0x15,	   //自定义配置命令，查询子设备在线状态			
}ACK_TYPEDEF;	

																					
//zigbee工作类型
typedef enum 
{
  ENDPOINT=0,		//终端
  ROUTER			//路由			 	
}DEVMODE_TYPEDEF;		 

//通道号
typedef enum 
{																														
  CH_11=11,			//通道11
  CH_12,				//通道12
  CH_13,				//通道13
  CH_14,				//通道14
  CH_15,				//通道15	
  CH_16,				//通道16
  CH_17,				//通道17
  CH_18,				//通道18
  CH_19,				//通道19	
  CH_20,				//通道20
  CH_21,				//通道21
  CH_22,				//通道22
  CH_23,				//通道23	
  CH_24,				//通道24
  CH_25			    //通道25			 	
}DEVCH_TYPEDEF;					  

//发射功率
typedef enum 
{
  Power_32dBm=0,		//-32.0 dBm
  Power_20dBm,			//-20.5 dBm
  Power_9dBm,				//-9.0 dBm
  Power2dBm		  	  //2.5 dBm			 	
}POWERLEVEL_TYPEDEF;				 

//发送方式
typedef enum 
{
  P2P=0,						//单播
  BroadCast	  	  	//广播		 	
}DEVTXMODE_TYPEDEF;				 

//设备基本参数
#pragma pack(1)
typedef struct
{
  u8  DevName[16];
  u8  DevPwd[16];
  DEVMODE_TYPEDEF  DevMode;	 
  DEVCH_TYPEDEF DevCh;
  u16 PanID;
  u16 MyAddr;
  u8 MyIEEE[8];
  u16 DstAddr;
  u8 DstIEEE[8];	 //保留全为‘0’
  u8 Reserve;
  POWERLEVEL_TYPEDEF PowerLevel;
  u8 RetryNum;
  u8 TranTimeout;
  u8 Serial_Rate;
  u8 Serial_DataB;
  u8 Serial_StopB;
  u8 Serial_ParityB; 
  DEVTXMODE_TYPEDEF  DevTxMode;
}ZIG_DEV_INFO_TYPEDEF;		
#pragma pack()


//获取设备参数时返回的信息
#pragma pack(1)
typedef struct
{
  u8   frame_head[3];   	//协议帧头
  u8   function_code;  		//功能码				
  ZIG_DEV_INFO_TYPEDEF  zig_dev_info; 	//设备信息
  u8 work_state;					//运行状态
  u16 dev_type;						//设备类型
  u16 firmware;						//固件版本
}ZIG_GET_MYINFO_ACK_TYPEDEF; 
#pragma pack()

//ACK响应状态
typedef enum
{
  OK=0,   
  LENGTH_FAUSE,			
  ADDRESS_FAUSE, 	
  CHECK_FAUSE,				
  WRITE_FAUSE,				
  OTHER_FAUSE						
}ZIG_ACK_RESPOND_STATE_TYPEDEF; 

//绑定步骤状态
typedef enum
{
  OVERTIME_FAUSE=0,						//超时
  STEP_CFG_DEV_PANID_SEARCH=1,   //设置网络号为Search PanID(广播搜索) 
  STEP_RX_WORKPANID_DSTADDR,						//等待网关发来的work_panid和dstaddr					
  STEP_CFG_DEV_DSTADDR, 					//设置目的设备地址
  STEP_SEND_DEV_INFO,	 			//发送my devinfo	
  STEP_CFG_DEV_WORKPANID_DSTADDR,								//修改PANID和DstAddr
  STEP_BIND_OK
}ZIG_BIND_STEP_TYPEDEF; 

//修改设备配置返回的信息
typedef struct
{
  u8   frame_head[3];   //协议帧头
  u8   function_code;  	//功能码
  u16   MyAddr;
  ZIG_ACK_RESPOND_STATE_TYPEDEF	 respond_state;		//响应状态					
}ZIG_CFG_DEV_INFO_ACK_TYPEDEF; 	 

//配置备份
typedef struct
{
  u16 DstAddr;
  u16 PanID;		
}ZIG_BACKUP_DEV_INFO_TYPEDEF; 	 


//配置设备dstaddr返回的信息
typedef struct
{
  u8   frame_head[3];   //协议帧头
  u8   function_code;  	//功能码
  ZIG_ACK_RESPOND_STATE_TYPEDEF	 respond_state;		//响应状态					
}ZIG_MODIFY_DEV_DSTADDR_ACK_TYPEDEF; 	 		 


//接收网关发来的dstaddr和正常通信时的panid
typedef struct
{
  u8   frame_head[3];   	//协议帧头
  u8   function_code;  		//功能码
  u16  work_panid;				//正常通信网络号				
  u16  dst_addr;					//目的地址
  u8   sum_chk;						//校验和
}ZIG_RX_WORKPANID_DSTADDR_TYPEDEF; 

//查询子设备是否在线
typedef struct
{
  u8   frame_head[3];   //协议帧头
  u8   function_code;  	//功能码
  u8   sum_chk; 						
}ZIG_CHK_SUBDEVONLINE_STATE_ACK_TYPEDEF; 


typedef enum
{
  RET_OK=0,
  RET_ERR
}RESULT_TYPEDEF;  

typedef struct
{
  u16 Rx_Len;
  u16 Tx_Len;
  u8  Rx_Buf[UART_MAX_RX_LEN];   //DMA接收缓存
  u8  Tx_Buf[UART_MAX_TX_LEN];   //DMA发送缓存
}ZIG_UART_BUF_TYPEDEF; 

extern ZIG_UART_BUF_TYPEDEF  Zig_Uart_Buf;			
extern ZIG_DEV_INFO_TYPEDEF  zig_dev_info;
extern EEPROM_BACKUP_INFO_TYPEDEF eeprom_backup_info;

void Zigbee_Uart_Init(void);
void Zigbee_Init(void);
void Zigbee_DeInit(void);
void Zigbee_WakeUp(void);
void Zigbee_Sleep(void);
void Eeprom_Read_DEV_INFO(EEPROM_BACKUP_INFO_TYPEDEF *p_eeprom_backup_info);
void Zigbee_Set_BackUpInfo(void);
void Zig_Send_Data(u8* pbuf,u16 len);
ACK_TYPEDEF Zig_Send_Cmd(u8* pbuf,CMD_TYPEDEF cmd_type);
RESULT_TYPEDEF Zigbee_Get_Set_Info(void);
ZIG_BIND_STEP_TYPEDEF Zigbee_Bind_Handle(void);
void Zig_RxData_Analysis(ZIG_UART_BUF_TYPEDEF *p_zig_uart_buf);

#endif





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

/******Zigbee������Ϣ****************/
#define DEV_TYPE    			    DEV_SWITCH 
#define SEARCH_PANID 			    0x0FF0	
#define BIND_OVER_TIME			    30	

	  			
//��������
typedef enum 
{
  CMD_GET_DEV_INFO=0xD1,		//��ȡ�豸������Ϣ����
  CMD_MODIFY_DEV_DSTADDR=0xD2,	//�޸�Ŀ�ĵ�ַ����	 
  CMD_CFG_DEV_INFO=0xD6,			//�����豸������Ϣ����
  CMD_ENTER_SLEEP=0xD8,     //��������
  CMD_RESET_DEV_INFO=0xD9,			//��λ�豸����			
  SUBCMD_CFG_DEV_WORKPANID_DSTADDR=0x10,			 		//�Զ��������������panid
  SUBCMD_CFG_DEV_MYADDR=0x11,					//�Զ��������������myaddr
  SUBCMD_CFG_DEV_PANID_SEARCH=0x12,	 //�Զ��������������panidΪSearch PanID
  DATA_TX_MY_DEVINFO=0x13,
  SUBCMD_CFG_DEV_BACKUP_PANID_DSTADDR=0x14,	 //�Զ��������������panidΪSearch PanID		
}CMD_TYPEDEF;	

//ACK����
typedef enum 
{
  CMD_NONE_ACK=0,
  CMD_GET_DEV_INFO_ACK=0xD1,
  CMD_MODIFY_DEV_DSTADDR_ACK=0xD2,		 
  CMD_CFG_DEV_INFO_ACK=0xD6,
  DATA_RX_WORKPANID_DSTADDR_ACK=0x13,
  SUBCMD_CHK_SUBDEVONLINE_STATE_ACK=0x15,	   //�Զ������������ѯ���豸����״̬			
}ACK_TYPEDEF;	

																					
//zigbee��������
typedef enum 
{
  ENDPOINT=0,		//�ն�
  ROUTER			//·��			 	
}DEVMODE_TYPEDEF;		 

//ͨ����
typedef enum 
{																														
  CH_11=11,			//ͨ��11
  CH_12,				//ͨ��12
  CH_13,				//ͨ��13
  CH_14,				//ͨ��14
  CH_15,				//ͨ��15	
  CH_16,				//ͨ��16
  CH_17,				//ͨ��17
  CH_18,				//ͨ��18
  CH_19,				//ͨ��19	
  CH_20,				//ͨ��20
  CH_21,				//ͨ��21
  CH_22,				//ͨ��22
  CH_23,				//ͨ��23	
  CH_24,				//ͨ��24
  CH_25			    //ͨ��25			 	
}DEVCH_TYPEDEF;					  

//���书��
typedef enum 
{
  Power_32dBm=0,		//-32.0 dBm
  Power_20dBm,			//-20.5 dBm
  Power_9dBm,				//-9.0 dBm
  Power2dBm		  	  //2.5 dBm			 	
}POWERLEVEL_TYPEDEF;				 

//���ͷ�ʽ
typedef enum 
{
  P2P=0,						//����
  BroadCast	  	  	//�㲥		 	
}DEVTXMODE_TYPEDEF;				 

//�豸��������
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
  u8 DstIEEE[8];	 //����ȫΪ��0��
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


//��ȡ�豸����ʱ���ص���Ϣ
#pragma pack(1)
typedef struct
{
  u8   frame_head[3];   	//Э��֡ͷ
  u8   function_code;  		//������				
  ZIG_DEV_INFO_TYPEDEF  zig_dev_info; 	//�豸��Ϣ
  u8 work_state;					//����״̬
  u16 dev_type;						//�豸����
  u16 firmware;						//�̼��汾
}ZIG_GET_MYINFO_ACK_TYPEDEF; 
#pragma pack()

//ACK��Ӧ״̬
typedef enum
{
  OK=0,   
  LENGTH_FAUSE,			
  ADDRESS_FAUSE, 	
  CHECK_FAUSE,				
  WRITE_FAUSE,				
  OTHER_FAUSE						
}ZIG_ACK_RESPOND_STATE_TYPEDEF; 

//�󶨲���״̬
typedef enum
{
  OVERTIME_FAUSE=0,						//��ʱ
  STEP_CFG_DEV_PANID_SEARCH=1,   //���������ΪSearch PanID(�㲥����) 
  STEP_RX_WORKPANID_DSTADDR,						//�ȴ����ط�����work_panid��dstaddr					
  STEP_CFG_DEV_DSTADDR, 					//����Ŀ���豸��ַ
  STEP_SEND_DEV_INFO,	 			//����my devinfo	
  STEP_CFG_DEV_WORKPANID_DSTADDR,								//�޸�PANID��DstAddr
  STEP_BIND_OK
}ZIG_BIND_STEP_TYPEDEF; 

//�޸��豸���÷��ص���Ϣ
typedef struct
{
  u8   frame_head[3];   //Э��֡ͷ
  u8   function_code;  	//������
  u16   MyAddr;
  ZIG_ACK_RESPOND_STATE_TYPEDEF	 respond_state;		//��Ӧ״̬					
}ZIG_CFG_DEV_INFO_ACK_TYPEDEF; 	 

//���ñ���
typedef struct
{
  u16 DstAddr;
  u16 PanID;		
}ZIG_BACKUP_DEV_INFO_TYPEDEF; 	 


//�����豸dstaddr���ص���Ϣ
typedef struct
{
  u8   frame_head[3];   //Э��֡ͷ
  u8   function_code;  	//������
  ZIG_ACK_RESPOND_STATE_TYPEDEF	 respond_state;		//��Ӧ״̬					
}ZIG_MODIFY_DEV_DSTADDR_ACK_TYPEDEF; 	 		 


//�������ط�����dstaddr������ͨ��ʱ��panid
typedef struct
{
  u8   frame_head[3];   	//Э��֡ͷ
  u8   function_code;  		//������
  u16  work_panid;				//����ͨ�������				
  u16  dst_addr;					//Ŀ�ĵ�ַ
  u8   sum_chk;						//У���
}ZIG_RX_WORKPANID_DSTADDR_TYPEDEF; 

//��ѯ���豸�Ƿ�����
typedef struct
{
  u8   frame_head[3];   //Э��֡ͷ
  u8   function_code;  	//������
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
  u8  Rx_Buf[UART_MAX_RX_LEN];   //DMA���ջ���
  u8  Tx_Buf[UART_MAX_TX_LEN];   //DMA���ͻ���
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





/**
  ******************************************************************************
  * @file    Templates/Inc/main.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-June-2014
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"


#define     MAX_SWITCH_NUM   4
//子设备类型
typedef enum 
{
  DEV_AIR=0xD0,    					//空调
  DEV_FLOOR=0xD1,  					//地暖
  DEV_LIGHT_SWITCH=0xD2,   	//灯光开关型
  DEV_LIGHT_DIMMER=0xD3,  	//灯光调光型
  DEV_CURTAIN=0xD4,  				//窗帘
  DEV_SCENE=0xD5,  					//场景
  DEV_MUSIC=0xD6,  					//音乐
  DEV_TV=0xD7,  						//电视(含机顶盒)
  DEV_DOOR=0xD8,  					//门锁
  DEV_HANG_AIR=0xD9,  			//壁挂空调
  DEV_SWITCH=0xDA           //开关
}DEV_TYPE_TYPEDEF;		

//开关id
typedef enum 
{
  S0_ID=0,
  S1_ID,
  S2_ID,
  S3_ID
}SWITCH_ID_TYPEDEF;

//开关信息（个数/id）
typedef struct
{
  u8 head;
  DEV_TYPE_TYPEDEF dev_type;
  u8 dev_id[8];
  u8 switch_num;  
  SWITCH_ID_TYPEDEF switch_id[MAX_SWITCH_NUM];
  u8 tail;
}SWITCH_CHK_INFO_TYPEDEF;

//查询开关信息
typedef struct
{
  u8 head;
  DEV_TYPE_TYPEDEF dev_type;
  u8 dev_id[8];
  u8 tail;
}SWITCH_CHK_CMD_TYPEDEF;

//开关命令
typedef struct
{
  u8 head;
  DEV_TYPE_TYPEDEF dev_type;
  u8 dev_id[8];
  SWITCH_ID_TYPEDEF switch_id;
  u8 tail;
}SWITCH_CTRL_CMD_TYPEDEF;


//EEPROM备份信息(成员字节必须是4的倍数)
typedef struct
{
  u32 eeprom_switch_num;
  u32 eeprom_zigbee_panid_dstaddr;
}EEPROM_BACKUP_INFO_TYPEDEF;


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

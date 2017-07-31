#ifndef __AT25SF081_H__
#define __AT25SF081_H__	 
#include "stm32l0xx_hal.h"


////W25X16∂¡–¥
#define FLASH_ID 0X1F13

//÷∏¡Ó±Ì
#define AT25SFXX_WriteEnable		  0x06 
#define AT25SFXX_WriteDisable		  0x04 
#define AT25SFXX_ReadStatusReg		0x05 
#define AT25SFXX_WriteStatusReg		0x01 
#define AT25SFXX_ReadData			    0x03 
#define AT25SFXX_PageProgram		  0x02 

#define AT25SFXX_SectorErase		  0x20 
#define AT25SFXX_ChipErase			  0xC7 
#define AT25SFXX_PowerDown			  0xB9 
#define AT25SFXX_Rusume	          0xAB 

void AT25SFXX_Init(void); 
void AT25SFXX_DeInit(void);
void AT25SFXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead); 
void AT25SFXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
u16  AT25SFXX_ReadID(void);  	
#endif


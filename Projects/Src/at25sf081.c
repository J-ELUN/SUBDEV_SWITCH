#include "at25sf081.h"
#include "stm32l0xx_hal_msp.h" 

u8 SPI_FLASH_BUF[4096]={0};


void AT25SFXX_Init(void)
{
	SPI_Init();		   
} 

void AT25SFXX_DeInit(void)
{
	SPI_DeInit();		  
} 

static void SPI_WriteOneByte(u8 data)  
{
  HAL_SPI_Transmit(&SpiHandle, &data, 1,10);
}

static u8 SPI_ReadOneByte(void)
{
  u8 data=0;
  HAL_SPI_Receive(&SpiHandle, &data, 1,10);
  return data;
}

static void SPI_Read_nLen_Byte(u8 *pbuf,u16 len)
{
  HAL_SPI_Receive(&SpiHandle, pbuf, len,10);
}

static void SPI_Write_nLen_Byte(u8 *pbuf,u16 len)
{
  HAL_SPI_Transmit(&SpiHandle, pbuf, len,10);
}


//��ȡSPI_FLASH��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
static u8 SPI_Flash_ReadSR(void)   
{  
	u8 byte=0;   
	SPI_CS_RESET();	                            //ʹ������   
	SPI_WriteOneByte(AT25SFXX_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPI_ReadOneByte();             //��ȡһ���ֽ�  
	SPI_CS_SET();	                           //ȡ��Ƭѡ     
	return byte;   
} 


//дSPI_FLASH״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
//static void SPI_FLASH_Write_SR(u8 sr)   
//{   
//	SPI_CS_RESET();	                           //ʹ������   
//	SPI_WriteOneByte(AT25SFXX_WriteStatusReg);   //����дȡ״̬�Ĵ�������    
//	SPI_WriteOneByte(sr);               //д��һ���ֽ�  
//	SPI_CS_SET();	                            //ȡ��Ƭѡ     	      
//} 


//SPI_FLASHдʹ��	
//��WEL��λ   
static void SPI_FLASH_Write_Enable(void)   
{
	SPI_CS_RESET();                           //ʹ������   
  SPI_WriteOneByte(AT25SFXX_WriteEnable);      //����дʹ��  
	SPI_CS_SET();	                              //ȡ��Ƭѡ     	      
} 
//SPI_FLASHд��ֹ	
//��WEL����  
/*
static void SPI_FLASH_Write_Disable(void)   
{  
  SPI_CS_RESET();                           //ʹ������   
  SPI_WriteOneByte(AT25SFXX_WriteDisable);     //����д��ָֹ��    
  SPI_CS_SET();	                             //ȡ��Ƭѡ     	      
}*/

//�������ģʽ
static void SPI_Flash_PowerDown(void)   
{ 
  SPI_CS_RESET();                            //ʹ������   
  SPI_WriteOneByte(AT25SFXX_PowerDown);        //���͵�������  
  SPI_CS_SET();	                           //ȡ��Ƭѡ     	      
  HAL_Delay(1);                              //�ȴ�TPD  
}  

//����
static void SPI_Flash_WAKEUP(void)   
{  
  SPI_CS_RESET();                            //ʹ������   
  SPI_WriteOneByte(AT25SFXX_Rusume);         //  send AT25SFXX_PowerDown command 0xAB    
  SPI_CS_SET();	                            //ȡ��Ƭѡ     	      
  HAL_Delay(1);                               //�ȴ�TRES1
}  

//��ȡоƬID W25X16��ID:0XEF14
static u16 SPI_Flash_ReadID(void)
{
	u16 Temp = 0; 
	SPI_CS_RESET();				    
	SPI_WriteOneByte(0x90);//���Ͷ�ȡID����	    
	SPI_WriteOneByte(0x00); 	    
	SPI_WriteOneByte(0x00); 	    
	SPI_WriteOneByte(0x00); 	 			   
	Temp|=SPI_ReadOneByte()<<8;  
	Temp|=SPI_ReadOneByte();	 
	SPI_CS_SET();	
	return Temp;
}  

//�ȴ�����
static void SPI_Flash_Wait_Busy(void)   
{   
	while ((SPI_Flash_ReadSR()&0x01)==0x01);   // �ȴ�BUSYλ���
} 


//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
static void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{    												     
  SPI_CS_RESET();                           //ʹ������   
  SPI_WriteOneByte(AT25SFXX_ReadData);         //���Ͷ�ȡ����   
  SPI_WriteOneByte((u8)((ReadAddr)>>16));  //����24bit��ַ    
  SPI_WriteOneByte((u8)((ReadAddr)>>8));   
  SPI_WriteOneByte((u8)ReadAddr); 
  SPI_Read_nLen_Byte(pBuffer,NumByteToRead);
  SPI_CS_SET();                           //ȡ��Ƭѡ 

}


//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
static void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{ 
  SPI_FLASH_Write_Enable();                  //SET WEL 
  SPI_CS_RESET();                             //ʹ������   
  SPI_WriteOneByte(AT25SFXX_PageProgram);      //����дҳ����   
  SPI_WriteOneByte((u8)((WriteAddr)>>16)); //����24bit��ַ    
  SPI_WriteOneByte((u8)((WriteAddr)>>8));   
  SPI_WriteOneByte((u8)WriteAddr); 
  
  SPI_Write_nLen_Byte(pBuffer,NumByteToWrite); 
  SPI_CS_SET();                             //ȡ��Ƭѡ 
  SPI_Flash_Wait_Busy();					   //�ȴ�д�����
} 


//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
static void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 



//��������оƬ
//��Ƭ����ʱ��:
//W25X16:25s 
//W25X32:40s 
//W25X64:40s 
//�ȴ�ʱ�䳬��...
/*
static void SPI_Flash_Erase_Chip(void)   
{                                             
  SPI_FLASH_Write_Enable();                  //SET WEL 
  SPI_Flash_Wait_Busy();   
  SPI_CS_RESET();                                //ʹ������   
  SPI_WriteOneByte(AT25SFXX_ChipErase);        //����Ƭ��������  
  SPI_CS_SET();                            //ȡ��Ƭѡ     	      
  SPI_Flash_Wait_Busy();   				   //�ȴ�оƬ��������
} */ 


//����һ������(4kbyte)
//Dst_Addr:������ַ 0~511 for w25x16
//����һ������������ʱ��:150ms
static void SPI_Flash_Erase_Sector(u32 Dst_Addr)   
{   
  Dst_Addr*=4096;
  SPI_FLASH_Write_Enable();                  //SET WEL 	 
  SPI_Flash_Wait_Busy();   
  SPI_CS_RESET();                            //ʹ������   
  SPI_WriteOneByte(AT25SFXX_SectorErase);      //������������ָ�� 
  SPI_WriteOneByte((u8)((Dst_Addr)>>16));  //����24bit��ַ    
  SPI_WriteOneByte((u8)((Dst_Addr)>>8));   
  SPI_WriteOneByte((u8)Dst_Addr);  
  SPI_CS_SET();                            //ȡ��Ƭѡ     	      
  SPI_Flash_Wait_Busy();   				   //�ȴ��������
}  

//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)  		   
static void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    

  
	secpos=WriteAddr/4096;//������ַ 0~511 for w25x16
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   

	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
  
	while(1) 
	{	
		SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			SPI_Flash_Erase_Sector(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//д����������  

		}
    else 
      SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	}  
}


 

void AT25SFXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)
{
  SPI_Flash_WAKEUP();
  SPI_Flash_Read(pBuffer,ReadAddr,NumByteToRead);
  SPI_Flash_PowerDown();
}


void AT25SFXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
  SPI_Flash_WAKEUP(); 
  SPI_Flash_Write(pBuffer,WriteAddr, NumByteToWrite);
  SPI_Flash_PowerDown();
}


u16 AT25SFXX_ReadID(void)
{
  u16 id=0;
  SPI_Flash_WAKEUP();
  id=SPI_Flash_ReadID();
  SPI_Flash_PowerDown();
  return id;
}




















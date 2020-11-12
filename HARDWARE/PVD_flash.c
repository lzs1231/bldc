#if !defined  (_FLASH_H)
#define _FLASH_H
#define FLASH_ADR 0x08010000  //Ҫд�����ݵĵ�ַ
#define u8 INT8U
#define u16 INT16U
#define u32 INT32U
 
union union_temp16
{
	unsigned int un_temp16;
	unsigned char  un_temp8[2];// example 16: 0x0102  8:[0]2 [1]1
}my_unTemp16;
 
typedef struct 
{
	u8 apn[20];
	u8 useName[20];
	u8 password[20];
	u8 serverIP[16];
	u8 port[6];
	u8 useCall[3][15];
}configStruct;
 
configStruct myConf =
{
	 "cmnet",
	 "\0",
	 "\0",
	 "201.2.2.2",
	 "9002",
	 {
		"test123","\0","\0"
	 }
};
 
 
/******************************************************
flash �ַ���д��
ÿ�δ��������ֽ�
*******************************************************/
void FlashWriteStr( u32 flash_add, u16 len, u16* data )
{
	//char cp[12];
	//u8 s = 0;
	u16 byteN = 0;
	FLASH_Unlock();
	FLASH_ErasePage(flash_add);
	//sprintf( cp, "len:%d", len);
	//USART1_Puts(cp);
	while( len )
	{

		my_unTemp16.un_temp8[0] = *(data+byteN);
		my_unTemp16.un_temp8[1] = *(data+byteN+1);  
		FLASH_ProgramHalfWord( flash_add+byteN , my_unTemp16.un_temp16 );

		//sprintf( cp, "bye:%d\r\n", s);
		//USART1_Puts(cp);
		if( 1==len )
		{
			//������ݳ���������,Ϊ1��ʱ������
			break;               
		}
		else
		{
			byteN += 2;
			len -= 2;
		} 
	}
	FLASH_Lock();
}
 
/******************************************************
flash �ַ���������ָ��data�� 
��ַ��д��data��ַͬ �����ı�������Ҳ����һ��
*******************************************************/
void FlashReadStr( u32 flash_add, u16 len, u16* data )
{
	u16 byteN = 0;
	while( len )
	{
		my_unTemp16.un_temp16 = *(vu16*)(flash_add+byteN);
		if( 1==len )
		{
			*(data+byteN) = my_unTemp16.un_temp8[0];
			break;     
		}
		else
		{  
			*(data+byteN) = my_unTemp16.un_temp8[0];
			*(data+byteN+1) = my_unTemp16.un_temp8[1];
			byteN += 2;
			len -= 2;
		}
	}
}
 
#endif
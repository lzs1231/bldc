#include "modbus.h"
#include "usart.h"
#include "main.h"
//#include "stdlib.h"
#include "doublebuf.h"

#define RegData  64

MODBUS modbus;


extern  u8 RS485_RX_BUFF[];	//���ջ�����2048�ֽ�
u16 RS485_RX_CNT=0;			//���ռ�����
u8 RS485_RxFlag=0;			//����һ֡�������

extern  u8 RS485_TX_BUFF[];	//���ͻ�����
u16 RS485_TX_CNT=0;			//���ͼ�����
u8 RS485_TxFlag=0;			//����һ֡�������


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Slave�Ĵ����͵�Ƭ���Ĵ�����ӳ���ϵ
u16   Slave_InputIO[RegData];  //���뿪�����Ĵ���(����ʹ�õ���λ������)   ע�⣺ ���ﴢ��ӻ����ص����ݡ�    ������������ֻ����0��1 ���� Slave_InputIO[5]=0��Slave_InputIO[8]=1��
u16   Slave_OutputIO[RegData]; //����������Ĵ���(����ʹ�õ���λ������)    ������ 05 15

u16   Slave_ReadReg[40]  = {0};    //ֻ���Ĵ���----�洢���ݣ�����ֻ�ܶ�ȡ          ������ 03
u16   Slave_WriteReg[40] = {0};   //д�Ĵ���-------���������޸ĸüĴ���   ������ 06 16


/**************04�������ȡ����Ĵ���****************/
u16 *const pGainDead 	 = &Slave_ReadReg[0];
u16 *const pFineTune	 = &Slave_ReadReg[1];
u16 *const pDisPulseNum  = &Slave_ReadReg[2];
u16 *const pSensorRate   = &Slave_ReadReg[3];
u16 *const pSensorValue  = &Slave_ReadReg[4];
u16 *const pMode   	     = &Slave_ReadReg[5];
u16 *const pMatDis       = &Slave_ReadReg[6];
u16 *const pLimitFun1    = &Slave_ReadReg[7];
u16 *const pLimitFun2    = &Slave_ReadReg[8];
u16 *const pSPCFun1		 = &Slave_ReadReg[9];
u16 *const pSPCFun2		 = &Slave_ReadReg[0x0a];
u16 *const pNoMatFun1 	 = &Slave_ReadReg[0x0b];
u16 *const pNoMatFun2 	 = &Slave_ReadReg[0x0c];
u16 *const pAFlex 		 = &Slave_ReadReg[0x0d];
u16 *const pMFlex 		 = &Slave_ReadReg[0x0e];
u16 *const pLongIoMode   = &Slave_ReadReg[0x0f];

u16 *const pAutoSpeed 	 = &Slave_ReadReg[0x10];
u16 *const pManuSpeed	 = &Slave_ReadReg[0x11];
u16 *const pCentSpeed 	 = &Slave_ReadReg[0x12];
u16 *const pCurrentPara  = &Slave_ReadReg[0x13];
u16 *const pAdjustData	 = &Slave_ReadReg[0x14];
u16 *const pOtherPara	 = &Slave_ReadReg[0x15];

/**************06������д�뱣�ּĴ���****************/
u16 *const pHAdjustFlag  = &Slave_WriteReg[0];
u16 *const pHAlarmSwitch = &Slave_WriteReg[1];
u16 *const pHClickBut    = &Slave_WriteReg[5];
u16 *const pHAddSub      = &Slave_WriteReg[6];

u16 *const pHWorkMode    = &Slave_WriteReg[7];
u16 *const pHSensorMode  = &Slave_WriteReg[8];
u16 *const pHLimitMode   = &Slave_WriteReg[9];
u16 *const pHSPCMode     = &Slave_WriteReg[0x0a];
u16 *const pHNoWaitEN    = &Slave_WriteReg[0x0b];
u16 *const pHAutoPolar   = &Slave_WriteReg[0x0c];
u16 *const pHManuPolar   = &Slave_WriteReg[0x0d];
u16 *const pHMotorType   = &Slave_WriteReg[0x0e];
u16 *const pHPowerOnMode = &Slave_WriteReg[0x0f];

u16 *const pHPort0Fun  	 = &Slave_WriteReg[0x10];
u16 *const pHPort1Fun 	 = &Slave_WriteReg[0x11];
u16 *const pHPort2Fun 	 = &Slave_WriteReg[0x12];
u16 *const pHPort3Fun 	 = &Slave_WriteReg[0x13];

u16 *const pHParaConfirm = &Slave_WriteReg[0x14];
u16 *const pHMatNum 	 = &Slave_WriteReg[0x15];  // ��ǰ���ϱ��
u16 *const pHSCaliStep   = &Slave_WriteReg[0x16];  //������У׼����
u16 *const pHSensorNum   = &Slave_WriteReg[0x17];

u16 *const pHTCaliTorque = &Slave_WriteReg[0x18];    //�г�У׼��������
u16 *const pHTCaliFlag   = &Slave_WriteReg[0x19];    //�Ƿ������г�У׼
u16 *const pHTCaliCancel = &Slave_WriteReg[0x1a];    //�Ƿ�ȡ���г�У׼

u16 *const pHFlexSpeed	 = &Slave_WriteReg[0x1b];    //���ԼӼ���ֵ

u16 *const pHSysYes		 = &Slave_WriteReg[0x1f];    //ȷ��ִ��

u16 *const pHWarmFlag	 = &Slave_WriteReg[0x20];    //�����־

//CRCУ�� �Լ�������ӵ�
const u8 auchCRCHi[] = { 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40} ; 


const u8 auchCRCLo[] = { 
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,0x43, 0x83, 0x41, 0x81, 0x80, 0x40} ;


u16 CRC_Compute(u8 *puchMsg, u16 usDataLen) 
{ 
	u8 uchCRCHi = 0xFF ; 
	u8 uchCRCLo = 0xFF ; 
	u32 uIndex ; 
	while (usDataLen--) 
	{ 
		uIndex = uchCRCHi ^ *puchMsg++ ; 
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ; 
		uchCRCLo = auchCRCLo[uIndex] ; 
	} 
	return ((uchCRCHi<< 8)  | (uchCRCLo)) ; 
}

// Modbus��ʼ������
void Modbus_Init()
{
    modbus.myadd = 0x01;   //�ӻ��豸��ַΪ1
    modbus.timrun = 0;     //modbus��ʱ��ֹͣ����
	
//	modbus.rcbuf = (u8 *)malloc(128);   //����ռ��ֽ�����6+�Ĵ���
//	Modbus_Time_Init();
	
/*********��ʼ��modbus���ּĴ���***********/
	*pHAdjustFlag=255;
	*pHClickBut=0;
	*pHAddSub=0;
	
	*pHParaConfirm=0;   //�����޸�ȷ��  1��ʾȷ�ϣ�2��ʾ��ԭ��0��ʾ��ִ��
	
	*pHSCaliStep=0;     //������У׼����
	*pHSensorNum=0;     //������У׼Ĭ��ѡ��EPC1
	*pHTCaliTorque=0;   //У׼����
	*pHTCaliFlag=0;
	*pHTCaliCancel=0;
	
	
	*pHSensorMode = gSensorMode;		//0=��1=�ң�2=��+�ң�3=SPC
	*pHAutoPolar = gAutoPolar;			//�Զ����ԣ�0=������1=����;
	*pHManuPolar = gManuPolar;			//�ֶ����ԣ�0=������1=����;
	*pHMotorType = gMotorType;     		//�������   0��ˢ���4   1��ˢ���5   2��ˢ���
	*pHPowerOnMode = gPowerOnMode;   	//��������ģʽ��,0=�ֶ���1=�Զ���2=���ģ�3=�ϴ� 
	*pHMatNum = gCurMatNum;        	// ��ǰ���ϱ��
	
	*pHLimitMode = gLimitMode;    		//��λ��ʽ   0Ϊ�ڲ���λ��1Ϊ�ⲿ��λ������λ��2�ڲ����ⲿ
	*pHSPCMode   = gSPCMode;			//���ξ�ƫģʽ  0���ڲ�������  1���ⲿ������
	*pHNoWaitEN  = gNoWaitEN;     		//���ϵȴ����ܣ�0=ʹ��,1=��ֹ
	
	*pHPort0Fun = gLongIo0Mode;      	//�ĸ��˿�Ĭ�Ϲ���
	*pHPort1Fun = gLongIo1Mode;
	*pHPort2Fun = gLongIo2Mode;
	*pHPort3Fun = gLongIo3Mode;
	*pHFlexSpeed = 0;
	*pHSysYes = 255;
	*pHWarmFlag  = 0;
}


// Modbus 3�Ź����뺯��
// Modbus ������ȡ���ּĴ���ֵ
void Modbus_Func3()
{
    u16 Regadd,Reglen,crc;
	u8 i=0,j;	
	
	//�õ�Ҫ��ȡ�Ĵ������׵�ַ
	Regadd = (modbus.rcbuf[2]<<8)+modbus.rcbuf[3];
	//�õ�Ҫ��ȡ�Ĵ����ĸ���
	Reglen = (modbus.rcbuf[4]<<8)+modbus.rcbuf[5];
	
//	modbus.sendbuf = (u8 *)malloc((5+Reglen*2)*sizeof(char));   //����ռ��ֽ�����6+�Ĵ�������*2��ÿ���Ĵ��������ֽ����ݣ�
	
	//���ͻ�Ӧ���ݰ�
	modbus.sendbuf[i++] = modbus.myadd;      //���ͱ����豸��ַ
	modbus.sendbuf[i++] = 0x03;              //���͹�����
    modbus.sendbuf[i++] = ((Reglen*2)%256);  //�����ֽڸ���
	for(j=0;j<Reglen;j++)                    //��������
	{
		modbus.sendbuf[i++] = Slave_ReadReg[Regadd+j]/256;    //��8λ
		modbus.sendbuf[i++] = Slave_ReadReg[Regadd+j]%256;    //��8λ
	}
	crc = CRC_Compute(modbus.sendbuf,i);    //����Ҫ�������ݵ�CRC
	modbus.sendbuf[i++] = crc/256;
	modbus.sendbuf[i++] = crc%256;
	
	// ��ʼ����Modbus����
#ifdef USE_MAX_485
	RS485_SendData(&modbus.sendbuf[0],i);	
#elif defined USE_MAX_232
	RS232_SendData(&modbus.sendbuf[0],i);	
#endif	
//	free(modbus.sendbuf);
}

// Modbus 4�Ź����뺯��
// Modbus ������ȡ����Ĵ���ֵ
void Modbus_Func4()
{
    u16 Regadd,Reglen,crc;
	u8 i=0,j;	
	
	//�õ�Ҫ��ȡ�Ĵ������׵�ַ
	Regadd = (modbus.rcbuf[2]<<8)+modbus.rcbuf[3];
	//�õ�Ҫ��ȡ�Ĵ����ĸ���
	Reglen = (modbus.rcbuf[4]<<8)+modbus.rcbuf[5];
	
//	modbus.sendbuf = (u8 *)malloc((5+Reglen*2)*sizeof(char));   //����ռ��ֽ�����6+�Ĵ�������*2��ÿ���Ĵ��������ֽ����ݣ�
	
	//���ͻ�Ӧ���ݰ�
	modbus.sendbuf[i++] = modbus.myadd;      //���ͱ����豸��ַ
	modbus.sendbuf[i++] = 0x04;              //���͹�����
    modbus.sendbuf[i++] = ((Reglen*2)%256);  //�����ֽڸ���
	for(j=0;j<Reglen;j++)                    //��������
	{
		modbus.sendbuf[i++] = Slave_ReadReg[Regadd+j]/256;    //��8λ
		modbus.sendbuf[i++] = Slave_ReadReg[Regadd+j]%256;    //��8λ
	}
	crc = CRC_Compute(modbus.sendbuf,i);    //����Ҫ�������ݵ�CRC
	modbus.sendbuf[i++] = crc/256;
	modbus.sendbuf[i++] = crc%256;
	
	// ��ʼ����Modbus����
#ifdef USE_MAX_485
	RS485_SendData(&modbus.sendbuf[0],i);	
#elif defined USE_MAX_232
	RS232_SendData(&modbus.sendbuf[0],i);	
#endif
//	free(modbus.sendbuf);
}

// Modbus 6�Ź����뺯��
// Modbus ����д�뱣�ּĴ���ֵ
void Modbus_Func6()  
{
	u16 Regadd;
	u16 val;
	u16 i=0,crc;
	Regadd = (modbus.rcbuf[2]<<8)+modbus.rcbuf[3]; 	 //�õ�Ҫ�޸ĵĵ�ַ 
	val = (modbus.rcbuf[4]<<8)+modbus.rcbuf[5];    	 //�޸ĺ��ֵ
	Slave_WriteReg[Regadd]=val; 					 //�޸ı��豸��Ӧ�ļĴ���
	
//	modbus.sendbuf = (u8 *)malloc(8*sizeof(char));
	
	//����Ϊ��Ӧ����
	modbus.sendbuf[i++] = modbus.myadd;		//���豸��ַ
    modbus.sendbuf[i++] = 0x06;        		//������ 
    modbus.sendbuf[i++] = Regadd/256;
	modbus.sendbuf[i++] = Regadd%256;
	modbus.sendbuf[i++] = val/256;
	modbus.sendbuf[i++] = val%256;
	
	crc=CRC_Compute(modbus.sendbuf,i);  	//����У����
	modbus.sendbuf[i++] = crc/256;  
	modbus.sendbuf[i++] = crc%256;
	
	// ��ʼ����Modbus����
#ifdef USE_MAX_485
	RS485_SendData(&modbus.sendbuf[0],i);	
#elif defined USE_MAX_232
	RS232_SendData(&modbus.sendbuf[0],i);	
#endif	
//	free(modbus.sendbuf);
}

//Modbus�¼�������
void Modbus_Event()
{
	u16 crc,rccrc;
	
	if(modbus.reflag == 0)  				//û���յ����ݰ�
	{
	   return;
	}
	if(modbus.recount>2) 					//��Ҫ���ӣ����򱻸��ź��п��ܵ���while�޷��˳�
    {
		//�յ����ݰ���ͨ������������֡����CRC
		crc = CRC_Compute(&modbus.rcbuf[0],modbus.recount-2); 
		// ��ȡ����֡��CRC
		rccrc = (modbus.rcbuf[modbus.recount-2]<<8)+modbus.rcbuf[modbus.recount-1];
	}
	if(crc == rccrc) 						 	//CRC����ɹ� ��ʼ������
	{	
		if(modbus.rcbuf[0] == modbus.myadd)  	// ����ַ�Ƿ�ʱ�Լ��ĵ�ַ
		{
			switch(modbus.rcbuf[1])   		 	//����modbus������
			{
				case 0:             break;
				case 1:             break;
				case 2:             break;
				case 3:      Modbus_Func3();      break;        //�����������͵Ĺ����룬��������Ӧ��������ָ��
				case 4:      Modbus_Func4();      break;
				case 5:             break;
				case 6:      Modbus_Func6();      break;
				case 7:             break;
				case 8:             break;
				case 9:             break;			 
			}
		}
		else if(modbus.rcbuf[0] == 0) 		//�㲥��ַ�����Ӧ
		{

		}	 
	}	
	modbus.recount = 0;
	modbus.reflag = 0; 
}





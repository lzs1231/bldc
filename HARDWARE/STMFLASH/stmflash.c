#include "sys.h"
#include "stmflash.h"
#include "delay.h"
#include "main.h"
#include "modbus.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
 
u16  rest_para[53]={0};           //���ݲ���
u16  save_para[53]={0};           //�������洢����
u16  SensorValue[40]={0};

#define SIZE sizeof(save_para)	 	//���鳤��
//����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С+0X08000000) 
#define FLASH_SAVE_ADDR  0X08010000 	//0x1 0000 = 65536���ֽ�   ÿҳ1024�ֽ�  ����Ϊ64ҳ��0~63ҳ�� ����洢�ӵ�64ҳ��ʼд
#define FLASH_REST_ADDR  0X08010400 	//0x1 0400 = 65536���ֽ�   ÿҳ1024�ֽ�  ����Ϊ65ҳ��0~64ҳ�� ���ݴ洢�ӵ�65ҳ��ʼд 
#define FLASH_SV_ADDR    0X08010800 	//0x1 0800 = 65536���ֽ�   ÿҳ1024�ֽ�  ����Ϊ66ҳ��0~65ҳ�� ���ݴ洢�ӵ�66ҳ��ʼд 

//��ȡָ����ַ�İ���(16λ����)
//faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
//����ֵ:��Ӧ����.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}

#if STM32_FLASH_WREN	//���ʹ����д   
//������д��
//WriteAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//��ַ����2.
	}  
} 

#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //�ֽ�
#else 
#define STM_SECTOR_SIZE	2048  //��������Ʒÿ������2k,С����ÿ������1k  ��2048���ֽ�
#endif		 

u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//�����2K�ֽ�    1024��16λ����  ��flashһҳ��Ϊһ������


//��ָ����ַ��ʼд��ָ�����ȵ�����
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
//pBuffer:����ָ��
//NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���.)
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //������ַ
	u16 secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
	u16 secremain; //������ʣ���ַ(16λ�ּ���)	   
 	u16 i;    
	u32 offaddr;   //ȥ��0X0800 0000��ĵ�ַ
	
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*128)))	return;//�Ƿ���ַ      0x0800 0000~0X0801FFFF���������ַ
	
	FLASH_Unlock();						    //����
	
	offaddr = WriteAddr-STM32_FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.        ����WriteAddr=0X0801 0000      offaddr=0X0801 0000-0X0800 0000 =0x0001 0000=65536���ֽ�
	secpos = offaddr/STM_SECTOR_SIZE;			//������ַ��32ҳ  0~64 for STM32F105RBT6      ÿҳ2k=2048���ֽ�     ҳ��65536/2048=32ҳ��0~31ҳ��
	secoff = (offaddr%STM_SECTOR_SIZE)/2;		//��ĳһҳ�����ڵ�ƫ��(2���ֽ�Ϊ������λ.)   ����Ϊ0��������һҳ��ͷ��ʼд��û��ƫ��
	secremain = STM_SECTOR_SIZE/2-secoff;		//��ҳ����ʣ��ռ��С      
	
	if(NumToWrite<=secremain)   secremain=NumToWrite;//�����ڸ�ҳ������Χ
	
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE/2);//������������������
		
		for(i=0;i<secremain;i++)//У������
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)   break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//�����������
			for(i=0;i<secremain;i++)//����
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	    //��Ҫд�����ݱ��浽������STMFLASH_BUF[]��
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE/2);//д����������  
		}
		else STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 	
		
		if(NumToWrite==secremain)break; //д�������
		else//д��δ����
		{
			secpos++;					//������ַ��1������һҳд��
			secoff = 0;					//ƫ��λ��Ϊ0 	 
		   	pBuffer += secremain;  		//ָ��ƫ��
			WriteAddr += secremain;		//д��ַƫ��	   
		   	NumToWrite -= secremain;	//�ֽ�(16λ)���ݼ�
			
			if(NumToWrite > (STM_SECTOR_SIZE/2))   secremain = STM_SECTOR_SIZE/2; //��һ����������д����
			else                                   secremain = NumToWrite;        //��һ����������д����
		}	 
	};	
	FLASH_Lock();//����
}
#endif

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead/2;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//WriteAddr:��ʼ��ַ
//WriteData:Ҫд�������
void Test_Write(u32 WriteAddr,u16 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//д��һ���� 
}

//�������
void SavePara(void)
{
	OS_CPU_SR  cpu_sr;
	
	save_para[0]=gCheckFlag;
	save_para[1]=gIwdgFlag;
	save_para[2]=gBackupFlag;
	
	save_para[3]=gWorkMode;
	save_para[4]=gSensorMode;
	save_para[5]=gSensorSignal;
	save_para[6]=gAutoPolar;
	save_para[7]=gManuPolar;
	save_para[8]=gMotorType;
	save_para[9]=gPowerOnMode;
	save_para[10]=gCurMatNum;
	
	save_para[11]=gGainData;
	save_para[12]=gDeadZone;
	save_para[13]=gFineTune;
	
	save_para[14]=gAutoSpeed;	    
	save_para[15]=gManuSpeed;
	save_para[16]=gCentSpeed;
	
	save_para[17]=gCaliTorque;
	save_para[18]=gFuncTorque;
	save_para[19]=gCurrentPulseNum;
	save_para[20]=gMAXPulseNum;
	
	save_para[21]=gLimitMode;
	save_para[22]=gExtendLimit;
	save_para[23]=gCenterLimit;
	save_para[24]=gIndentLimit;

	save_para[25]=gSPCMode;
	save_para[26]=gSPCStopTime;
	save_para[27]=gSPCExtendLimit;
	save_para[28]=gSPCIndentLimit;	
	
	save_para[29]=gNoWaitEN;
	save_para[30]=gNoDetectValve;
	save_para[31]=gNoDetectTime;
	save_para[32]=gNoWaitTime;

	
	save_para[33]=gLongIo0Mode;
	save_para[34]=gLongIo1Mode;
	save_para[35]=gLongIo2Mode;
	save_para[36]=gLongIo3Mode;

	save_para[37]=gAFlexAcc;
	save_para[38]=gAFlexDec;
	save_para[39]=gMFlexAcc;
	save_para[40]=gMFlexDec;
	
	save_para[41]=gAlarmSwitch;
	save_para[42]=gRelay;
	save_para[43]=gKeepWait;
	
	OS_ENTER_CRITICAL();   //�ر��ж�
	STMFLASH_Write(FLASH_SAVE_ADDR, (u16*)save_para, SIZE);        //WriteAddr:��ʼ��ַ    pBuffer:����ָ��    NumToWrite:����(16λ)�� 
	OS_EXIT_CRITICAL();    //���ж�  
}

//��ȡ����
void ReadPara(void)
{
	OS_CPU_SR  cpu_sr;
	
	OS_ENTER_CRITICAL();   //�ر��ж�
	STMFLASH_Read(FLASH_SAVE_ADDR, (u16*)save_para, SIZE);  
	OS_EXIT_CRITICAL();    //���ж�
	
	gCheckFlag      	=save_para[0];
	gIwdgFlag  			=save_para[1];
	gBackupFlag			=save_para[2];
	
	gWorkMode			=save_para[3];
	gSensorMode			=save_para[4];
	gSensorSignal		=save_para[5];
	gAutoPolar			=save_para[6];
	gManuPolar			=save_para[7];
	gMotorType			=save_para[8];
	gPowerOnMode		=save_para[9];
	gCurMatNum			=save_para[10];
	
	gGainData			=save_para[11];
	gDeadZone			=save_para[12];
	gFineTune		    =save_para[13];
	
	gAutoSpeed  		=save_para[14];
	gManuSpeed      	=save_para[15];
	gCentSpeed			=save_para[16];
	
	gCaliTorque	        =save_para[17];
	gFuncTorque			=save_para[18];
	gCurrentPulseNum	=save_para[19];
	gMAXPulseNum		=save_para[20];
	
	gLimitMode			=save_para[21];
	gExtendLimit	    =save_para[22];
	gCenterLimit		=save_para[23];
	gIndentLimit		=save_para[24];
	
	gSPCMode			=save_para[25];
	gSPCStopTime		=save_para[26];
	gSPCExtendLimit	    =save_para[27];
	gSPCIndentLimit	    =save_para[28];
	
	gNoWaitEN			=save_para[29];
	gNoDetectValve    	=save_para[30];
	gNoDetectTime     	=save_para[31];
	gNoWaitTime	        =save_para[32];
	
	gLongIo0Mode		=save_para[33];
	gLongIo1Mode		=save_para[34];
	gLongIo2Mode		=save_para[35];
	gLongIo3Mode		=save_para[36];

	gAFlexAcc           =save_para[37];
	gAFlexDec           =save_para[38];
	gMFlexAcc           =save_para[39];
	gMFlexDec           =save_para[40];
	
	gAlarmSwitch        =save_para[41];
	gRelay        		=save_para[42];
	gKeepWait       	=save_para[43];
}

void Clearing()   //���㱸��
{
	OS_CPU_SR  cpu_sr;
	u8 i;
	gBackupFlag = 0;    //���ݱ�־��0
	for(i=0;i<53;i++)
	{
		rest_para[i]=0Xffff;
	}
	
	OS_ENTER_CRITICAL();   //�ر��ж�
	STMFLASH_Write(FLASH_REST_ADDR, (u16*)rest_para, SIZE);  //WriteAddr:��ʼ��ַ    pBuffer:����ָ��    NumToWrite:����(16λ)�� 
	OS_EXIT_CRITICAL();    //���ж�  
}

void Backup()   //ϵͳ����
{
	OS_CPU_SR  cpu_sr;
	gBackupFlag = 1;    //���ݱ�־��1
	
	rest_para[0]=gCheckFlag;
	rest_para[1]=gIwdgFlag;	    
//	rest_para[2]=gBackupFlag;

//	rest_para[3]=gWorkMode;
	rest_para[4]=gSensorMode;
	rest_para[5]=gSensorSignal;
	rest_para[6]=gAutoPolar;
	rest_para[7]=gManuPolar;
	rest_para[8]=gMotorType;
	rest_para[9]=gPowerOnMode;
	rest_para[10]=gCurMatNum;
	
	rest_para[11]=gGainData;
	rest_para[12]=gDeadZone;
	rest_para[13]=gFineTune;
	
	rest_para[14]=gAutoSpeed;
	rest_para[15]=gManuSpeed;
	rest_para[16]=gCentSpeed;
	
	rest_para[17]=gCaliTorque;
	rest_para[18]=gFuncTorque;
//	rest_para[19]=gCurrentPulseNum;
//	rest_para[20]=gMAXPulseNum;

	rest_para[21]=gLimitMode;
	rest_para[22]=gExtendLimit;
	rest_para[23]=gCenterLimit;
	rest_para[24]=gIndentLimit;

	rest_para[25]=gSPCMode;
	rest_para[26]=gSPCStopTime;
	rest_para[27]=gSPCExtendLimit;
	rest_para[28]=gSPCIndentLimit;
	
	rest_para[29]=gNoWaitEN;
	rest_para[30]=gNoDetectValve;
	rest_para[31]=gNoDetectTime;
	rest_para[32]=gNoWaitTime;
	
	rest_para[33]=gLongIo0Mode;
	rest_para[34]=gLongIo1Mode;
	rest_para[35]=gLongIo2Mode;
	rest_para[36]=gLongIo3Mode;

	rest_para[37]=gAFlexAcc;
	rest_para[38]=gAFlexDec;
	rest_para[39]=gMFlexAcc;
	rest_para[40]=gMFlexDec;
	
	rest_para[41]=gAlarmSwitch;
	rest_para[42]=gRelay;
	rest_para[43]=gKeepWait ;
	
	OS_ENTER_CRITICAL();   //�ر��ж�
	STMFLASH_Write(FLASH_REST_ADDR, (u16*)rest_para, SIZE);        //WriteAddr:��ʼ��ַ    pBuffer:����ָ��    NumToWrite:����(16λ)�� 
	OS_EXIT_CRITICAL();    //���ж�  
}


void Restore()    //ϵͳ��ԭ
{
	OS_CPU_SR  cpu_sr;
	
	OS_ENTER_CRITICAL();   //�ر��ж�
	STMFLASH_Read(FLASH_REST_ADDR, (u16*)rest_para, SIZE);  
	OS_EXIT_CRITICAL();    //���ж�
	
	gCheckFlag      	=rest_para[0];
	gIwdgFlag  		 	=rest_para[1];
//	gBackupFlag			=rest_para[2];
	
//	gWorkMode			=rest_para[3];
	gSensorMode			=rest_para[4];
	gSensorSignal		=rest_para[5];
	gAutoPolar			=rest_para[6];
	gManuPolar			=rest_para[7];
	gMotorType			=rest_para[8];
	gPowerOnMode		=rest_para[9];
	gCurMatNum			=rest_para[10];
	
	gGainData			=rest_para[11];
	gDeadZone			=rest_para[12];
	gFineTune		    =rest_para[13];
	
	gAutoSpeed			=rest_para[14];
	gManuSpeed          =rest_para[15];
	gCentSpeed          =rest_para[16];
	
	gCaliTorque	    	=rest_para[17];
	gFuncTorque		    =rest_para[18];
//	gCurrentPulseNum    =rest_para[19];
//	gMAXPulseNum	    =rest_para[20];
	
	gLimitMode			=rest_para[21];
	gExtendLimit		=rest_para[22];
	gCenterLimit		=rest_para[23];
	gIndentLimit	    =rest_para[24];
	
	gSPCMode		 	=rest_para[25];
	gSPCStopTime		=rest_para[26];
	gSPCExtendLimit	    =rest_para[27];
	gSPCIndentLimit	    =rest_para[28];
	
	gNoWaitEN			=rest_para[29];
	gNoDetectValve    	=rest_para[30];
	gNoDetectTime     	=rest_para[31];
	gNoWaitTime	        =rest_para[32];
	
	gLongIo0Mode		=rest_para[33];
	gLongIo1Mode		=rest_para[34];
	gLongIo2Mode		=rest_para[35];
	gLongIo3Mode		=rest_para[36];

	gAFlexAcc           =rest_para[37];
	gAFlexDec           =rest_para[38];
	gMFlexAcc           =rest_para[39];
	gMFlexDec           =rest_para[40];
	
	gAlarmSwitch		=rest_para[41];
	gRelay        		=rest_para[42];
	gKeepWait       	=rest_para[43];
	Modbus_Init();
}

void WriteSensor()   //������ֵ����
{
	OS_CPU_SR  cpu_sr;
	
	SensorValue[0]=g_Mat0_SensorL_H;
	SensorValue[1]=g_Mat0_SensorL_L;
	SensorValue[2]=g_Mat0_SensorR_H;
	SensorValue[3]=g_Mat0_SensorR_L;
	SensorValue[4]=Mat0EPC12;
	
	SensorValue[5]=g_Mat1_SensorL_H;
	SensorValue[6]=g_Mat1_SensorL_L;
	SensorValue[7]=g_Mat1_SensorR_H;
	SensorValue[8]=g_Mat1_SensorR_L;
	SensorValue[9]=Mat1EPC12;
	
	SensorValue[10]=g_Mat2_SensorL_H;
	SensorValue[11]=g_Mat2_SensorL_L;
	SensorValue[12]=g_Mat2_SensorR_H;	    
	SensorValue[13]=g_Mat2_SensorR_L;
	SensorValue[14]=Mat2EPC12;
	
	SensorValue[15]=g_Mat3_SensorL_H;
	SensorValue[16]=g_Mat3_SensorL_L;
	SensorValue[17]=g_Mat3_SensorR_H;
	SensorValue[18]=g_Mat3_SensorR_L;
	SensorValue[19]=Mat3EPC12;
	
	SensorValue[20]=g_Mat4_SensorL_H;
	SensorValue[21]=g_Mat4_SensorL_L;
	SensorValue[22]=g_Mat4_SensorR_H;
	SensorValue[23]=g_Mat4_SensorR_L;
	SensorValue[24]=Mat4EPC12;
	
	SensorValue[25]=g_Mat5_SensorL_H;
	SensorValue[26]=g_Mat5_SensorL_L;
	SensorValue[27]=g_Mat5_SensorR_H;
	SensorValue[28]=g_Mat5_SensorR_L;
	SensorValue[29]=Mat5EPC12;
	
	SensorValue[30]=g_Mat6_SensorL_H;
	SensorValue[31]=g_Mat6_SensorL_L;
	SensorValue[32]=g_Mat6_SensorR_H;
	SensorValue[33]=g_Mat6_SensorR_L;
	SensorValue[34]=Mat6EPC12;
	
	SensorValue[35]=g_Mat7_SensorL_H;
	SensorValue[36]=g_Mat7_SensorL_L;
	SensorValue[37]=g_Mat7_SensorR_H;
	SensorValue[38]=g_Mat7_SensorR_L;
	SensorValue[39]=Mat7EPC12;
	
	OS_ENTER_CRITICAL();   //�ر��ж�
	STMFLASH_Write(FLASH_SV_ADDR, (u16*)SensorValue, SIZE);        //WriteAddr:��ʼ��ַ    pBuffer:����ָ��    NumToWrite:����(16λ)�� 
	OS_EXIT_CRITICAL();    //���ж�  
}


void ReadSensor()    //������ֵ��ȡ
{
	OS_CPU_SR  cpu_sr;
	
	OS_ENTER_CRITICAL();   //�ر��ж�
	STMFLASH_Read(FLASH_SV_ADDR, (u16*)SensorValue, SIZE);  
	OS_EXIT_CRITICAL();    //���ж�
	
	g_Mat0_SensorL_H      	=SensorValue[0];
	g_Mat0_SensorL_L  		=SensorValue[1];
	g_Mat0_SensorR_H		=SensorValue[2];
	g_Mat0_SensorR_L		=SensorValue[3];
	Mat0EPC12			    =SensorValue[4];
	
	g_Mat1_SensorL_H		=SensorValue[5];
	g_Mat1_SensorL_L		=SensorValue[6];
	g_Mat1_SensorR_H		=SensorValue[7];
	g_Mat1_SensorR_L		=SensorValue[8];
	Mat1EPC12				=SensorValue[9];
	
	g_Mat2_SensorL_H		=SensorValue[10];
	g_Mat2_SensorL_L	 	=SensorValue[11];
	g_Mat2_SensorR_H  		=SensorValue[12];
	g_Mat2_SensorR_L 		=SensorValue[13];
	Mat2EPC12 				=SensorValue[14];
	
	g_Mat3_SensorL_H  		=SensorValue[15];
	g_Mat3_SensorL_L	    =SensorValue[16];
	g_Mat3_SensorR_H	    =SensorValue[17];
	g_Mat3_SensorR_L		=SensorValue[18];
	Mat3EPC12		        =SensorValue[19];
	
	g_Mat4_SensorL_H	    =SensorValue[20];
	g_Mat4_SensorL_L		=SensorValue[21];
	g_Mat4_SensorR_H		=SensorValue[22];
	g_Mat4_SensorR_L		=SensorValue[23];
	Mat4EPC12    	        =SensorValue[24];
	
	g_Mat5_SensorL_H     	=SensorValue[25];
	g_Mat5_SensorL_L	    =SensorValue[26];
	g_Mat5_SensorR_H	    =SensorValue[27];
	g_Mat5_SensorR_L		=SensorValue[28];
	Mat5EPC12	            =SensorValue[29];
	
	g_Mat6_SensorL_H	    =SensorValue[30];
	g_Mat6_SensorL_L		=SensorValue[31];
	g_Mat6_SensorR_H		=SensorValue[32];
	g_Mat6_SensorR_L     	=SensorValue[33];
	Mat6EPC12		        =SensorValue[34];
	
	g_Mat7_SensorL_H		=SensorValue[35];
	g_Mat7_SensorL_L		=SensorValue[36];
	g_Mat7_SensorR_H		=SensorValue[37];
	g_Mat7_SensorR_L	    =SensorValue[38];
	Mat7EPC12               =SensorValue[39];
}





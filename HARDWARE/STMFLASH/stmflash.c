#include "sys.h"
#include "stmflash.h"
#include "delay.h"
#include "main.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
 
u16  rest_para[53]={0};           //备份参数
u16  save_para[53]={0};           //保存掉电存储参数
u16  SensorValue[41]={0};

#define SIZE sizeof(save_para)	 	//数组长度
//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000) 
#define FLASH_SAVE_ADDR  0X08010000 	//0x1 0000 = 65536个字节   每页1024字节  所以为64页（0~63页） 掉电存储从第64页开始写
#define FLASH_REST_ADDR  0X08010400 	//0x1 0400 = 65536个字节   每页1024字节  所以为65页（0~64页） 备份存储从第65页开始写 
#define FLASH_SV_ADDR    0X08010800 	//0x1 0800 = 65536个字节   每页1024字节  所以为66页（0~65页） 备份存储从第66页开始写 

//读取指定地址的半字(16位数据)
//faddr:读地址(此地址必须为2的倍数!!)
//返回值:对应数据.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}

#if STM32_FLASH_WREN	//如果使能了写   
//不检查的写入
//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//地址增加2.
	}  
} 

#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else 
#define STM_SECTOR_SIZE	2048  //大容量产品每个扇区2k,小容量每个扇区1k  ，2048个字节
#endif		 

u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节    1024个16位数据  将flash一页分为一个数组


//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //扇区地址
	u16 secoff;	   //扇区内偏移地址(16位字计算)
	u16 secremain; //扇区内剩余地址(16位字计算)	   
 	u16 i;    
	u32 offaddr;   //去掉0X0800 0000后的地址
	
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*128)))	return;//非法地址      0x0800 0000~0X0801FFFF才是闪存地址
	
	FLASH_Unlock();						    //解锁
	
	offaddr = WriteAddr-STM32_FLASH_BASE;		//实际偏移地址.        假设WriteAddr=0X0801 0000      offaddr=0X0801 0000-0X0800 0000 =0x0001 0000=65536个字节
	secpos = offaddr/STM_SECTOR_SIZE;			//扇区地址第32页  0~64 for STM32F105RBT6      每页2k=2048个字节     页数65536/2048=32页（0~31页）
	secoff = (offaddr%STM_SECTOR_SIZE)/2;		//在某一页扇区内的偏移(2个字节为基本单位.)   余数为0，表明下一页从头开始写，没有偏移
	secremain = STM_SECTOR_SIZE/2-secoff;		//该页扇区剩余空间大小      
	
	if(NumToWrite<=secremain)   secremain=NumToWrite;//不大于该页扇区范围
	
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE/2);//读出整个扇区的内容
		
		for(i=0;i<secremain;i++)//校验数据
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)   break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
			for(i=0;i<secremain;i++)//复制
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	    //将要写的数据保存到缓冲区STMFLASH_BUF[]中
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE/2);//写入整个扇区  
		}
		else STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 	
		
		if(NumToWrite==secremain)break; //写入结束了
		else//写入未结束
		{
			secpos++;					//扇区地址增1，向下一页写入
			secoff = 0;					//偏移位置为0 	 
		   	pBuffer += secremain;  		//指针偏移
			WriteAddr += secremain;		//写地址偏移	   
		   	NumToWrite -= secremain;	//字节(16位)数递减
			
			if(NumToWrite > (STM_SECTOR_SIZE/2))   secremain = STM_SECTOR_SIZE/2; //下一个扇区还是写不完
			else                                   secremain = NumToWrite;        //下一个扇区可以写完了
		}	 
	};	
	FLASH_Lock();//上锁
}
#endif

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead/2;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//WriteAddr:起始地址
//WriteData:要写入的数据
void Test_Write(u32 WriteAddr,u16 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//写入一个字 
}

//保存参数
void SavePara(void)
{
	OS_CPU_SR  cpu_sr;
	
	save_para[0]=gCheckFlag;
	save_para[1]=gIwdgFlag;	    
	save_para[2]=gBackupFlag;
	
	
	save_para[3]=gWorkMode;
	save_para[4]=gSensorMode;
	
	save_para[5]=gAutoPolar;
	save_para[6]=gManuPolar;
	save_para[7]=gMotorType;
	save_para[8]=gPowerOnMode;	
	save_para[9]=gCurMatNum;
	
	save_para[10]=gGainData;
	save_para[11]=gDeadZone;
	save_para[12]=gFineTune;
	
	save_para[13]=gAutoSpeed;	    
	save_para[14]=gManuSpeed;
	save_para[15]=gCentSpeed;
	
	save_para[16]=gCaliTorque;
	save_para[17]=gFuncTorque;
	save_para[18]=gCurrentPulseNum;
	save_para[19]=gMAXPulseNum;
	
	save_para[20]=gLimitMode;
	save_para[21]=gBehindLimit;
	save_para[22]=gCenterLimit;
	save_para[23]=gFrontLimit;
	
	save_para[24]=gSPCMode;
	save_para[25]=gSPCStopTime;
	save_para[26]=gSPCBehindLimit;
	save_para[27]=gSPCFrontLimit;	
	
	save_para[28]=gNoWaitEN;
	save_para[29]=gNoDetectValve;
	save_para[30]=gNoDetectTime;
	save_para[31]=gNoWaitTime;

	
	save_para[32]=gLongIo0Mode;
	save_para[33]=gLongIo1Mode;
	save_para[34]=gLongIo2Mode;
	save_para[35]=gLongIo3Mode;

	save_para[36]=gAFlexAcc;
	save_para[37]=gAFlexDec;
	save_para[38]=gMFlexAcc;
	save_para[39]=gMFlexDec;
	
	save_para[40]=gAFlexSacc;
	save_para[41]=gAFlexSdec;
	save_para[42]=gMFlexSacc;
	save_para[43]=gMFlexSdec;
	
	save_para[44]=gLanguageChoice;
	save_para[45]=gLockTime;
	save_para[46]=gBuzzer ;
	save_para[47]=gBrightness ;
	save_para[48]=((Password[1]<<8)|Password[0]);
	save_para[49]=((Password[3]<<8)|Password[2]);
	save_para[50]=((Password[5]<<8)|Password[4]);
	OS_ENTER_CRITICAL();   //关闭中断
	STMFLASH_Write(FLASH_SAVE_ADDR, (u16*)save_para, SIZE);        //WriteAddr:起始地址    pBuffer:数据指针    NumToWrite:半字(16位)数 
	OS_EXIT_CRITICAL();    //打开中断  
}

//读取参数
void ReadPara(void)
{
	OS_CPU_SR  cpu_sr;
	
	OS_ENTER_CRITICAL();   //关闭中断
	STMFLASH_Read(FLASH_SAVE_ADDR, (u16*)save_para, SIZE);  
	OS_EXIT_CRITICAL();    //打开中断
	
	gCheckFlag      	=save_para[0];
	gIwdgFlag  			=save_para[1];
	gBackupFlag			=save_para[2];
	
	gWorkMode			=save_para[3];
	gSensorMode			=save_para[4];
	gAutoPolar			=save_para[5];
	gManuPolar			=save_para[6];
	gMotorType			=save_para[7];
	gPowerOnMode		=save_para[8];
	gCurMatNum			=save_para[9];
	
	gGainData			=save_para[10];
	gDeadZone			=save_para[11];
	gFineTune		    =save_para[12];
	
	gAutoSpeed  		=save_para[13];
	gManuSpeed      	=save_para[14];
	gCentSpeed			=save_para[15];
	
	gCaliTorque	        =save_para[16];
	gFuncTorque			=save_para[17];
	gCurrentPulseNum	=save_para[18];
	gMAXPulseNum		=save_para[19];
	
	gLimitMode			=save_para[20];
	gBehindLimit	    =save_para[21];
	gCenterLimit		=save_para[22];
	gFrontLimit			=save_para[23];
	
	gSPCMode			=save_para[24];
	gSPCStopTime		=save_para[25];
	gSPCBehindLimit	    =save_para[26];
	gSPCFrontLimit	    =save_para[27];
	
	gNoWaitEN			=save_para[28];
	gNoDetectValve    	=save_para[29];
	gNoDetectTime     	=save_para[30];
	gNoWaitTime	        =save_para[31];
	
	gLongIo0Mode		=save_para[32];
	gLongIo1Mode		=save_para[33];
	gLongIo2Mode		=save_para[34];
	gLongIo3Mode		=save_para[35];

	gAFlexAcc           =save_para[36];
	gAFlexDec           =save_para[37];
	gMFlexAcc           =save_para[38];
	gMFlexDec           =save_para[39];
	
	gAFlexSacc             =save_para[40];
	gAFlexSdec             =save_para[41];
	gMFlexSacc             =save_para[42];
	gMFlexSdec             =save_para[43];
	
	gLanguageChoice			=save_para[44];
	gLockTime            	=save_para[45];
	gBuzzer                 =save_para[46];
	gBrightness             =save_para[47] ;
	Password[0]				=save_para[48]&0xff;
	Password[1]				=save_para[48]>>8;
	Password[2]				=save_para[49]&0xFf;
	Password[3]				=save_para[49]>>8;
	Password[4]				=save_para[50]&0xff;
	Password[5]				=save_para[50]>>8;
	
}

void Clearing()   //清零备份
{
	OS_CPU_SR  cpu_sr;
	u8 i;
	gBackupFlag = 0;    //备份标志置0
	for(i=0;i<53;i++)
	{
		rest_para[i]=0Xffff;
	}
	
	OS_ENTER_CRITICAL();   //关闭中断
	STMFLASH_Write(FLASH_REST_ADDR, (u16*)rest_para, SIZE);  //WriteAddr:起始地址    pBuffer:数据指针    NumToWrite:半字(16位)数 
	OS_EXIT_CRITICAL();    //打开中断  
}

void Backup()   //系统备份
{
	OS_CPU_SR  cpu_sr;
	gBackupFlag = 1;    //备份标志置1
	
	rest_para[0]=gCheckFlag;
	rest_para[1]=gIwdgFlag;	    
//	rest_para[2]=BackupFlag;

//	rest_para[3]=gWorkMode;
	rest_para[4]=gSensorMode;
	rest_para[5]=gAutoPolar;
	rest_para[6]=gManuPolar;
	rest_para[7]=gMotorType;
	rest_para[8]=gPowerOnMode;
	rest_para[9]=gCurMatNum;
	
	rest_para[10]=gGainData;
	rest_para[11]=gDeadZone;
	rest_para[12]=gFineTune;
	
	rest_para[13]=gAutoSpeed;
	rest_para[14]=gManuSpeed;
	rest_para[15]=gCentSpeed;
	
	
	rest_para[16]=gFuncTorque;
	rest_para[17]=gCaliTorque;
//	rest_para[18]=gCurrentPulseNum;
//	rest_para[19]=gMAXPulseNum;

	rest_para[20]=gLimitMode;
	rest_para[21]=gBehindLimit;
	rest_para[22]=gCenterLimit;
	rest_para[23]=gFrontLimit;
	
	rest_para[24]=gSPCMode;
	rest_para[25]=gSPCStopTime;
	rest_para[26]=gSPCBehindLimit;
	rest_para[27]=gSPCFrontLimit;
	
	rest_para[28]=gNoWaitEN;
	rest_para[29]=gNoDetectValve;
	rest_para[30]=gNoDetectTime;
	rest_para[31]=gNoWaitTime;
	
	rest_para[32]=gLongIo0Mode;
	rest_para[33]=gLongIo1Mode;
	rest_para[34]=gLongIo2Mode;
	rest_para[35]=gLongIo3Mode;

	rest_para[36]=gAFlexAcc;
	rest_para[37]=gAFlexDec;
	rest_para[38]=gMFlexAcc;
	rest_para[39]=gMFlexDec;
	
	rest_para[40]=gAFlexSacc;
	rest_para[41]=gAFlexSdec;
	rest_para[42]=gMFlexSacc;
	rest_para[43]=gMFlexSdec;
	
	rest_para[44]=gLanguageChoice;
	rest_para[45]=gLockTime;
	rest_para[46]=gBuzzer;
	rest_para[47]=gBrightness ;
	rest_para[48]=((Password[1]<<8)|Password[0]);
	rest_para[49]=((Password[3]<<8)|Password[2]);
	rest_para[50]=((Password[5]<<8)|Password[4]);
	
	OS_ENTER_CRITICAL();   //关闭中断
	STMFLASH_Write(FLASH_REST_ADDR, (u16*)rest_para, SIZE);        //WriteAddr:起始地址    pBuffer:数据指针    NumToWrite:半字(16位)数 
	OS_EXIT_CRITICAL();    //打开中断  
}


void Restore()    //系统还原
{
	OS_CPU_SR  cpu_sr;
	
	OS_ENTER_CRITICAL();   //关闭中断
	STMFLASH_Read(FLASH_REST_ADDR, (u16*)rest_para, SIZE);  
	OS_EXIT_CRITICAL();    //打开中断
	
	gCheckFlag      	=rest_para[0];
	gIwdgFlag  		 	=rest_para[1];
//	gBackupFlag			=rest_para[2];
	
//	gWorkMode			=rest_para[3];
	gSensorMode			=rest_para[4];
	gAutoPolar			=rest_para[5];
	gManuPolar			=rest_para[6];
	gMotorType			=rest_para[7];
	gPowerOnMode		=rest_para[8];
	gCurMatNum			=rest_para[9];
	
	gGainData			=rest_para[10];
	gDeadZone			=rest_para[11];
	gFineTune		    =rest_para[12];
	
	gAutoSpeed			=rest_para[13];
	gManuSpeed          =rest_para[14];
	gCentSpeed          =rest_para[15];
	
	gCaliTorque	    	=rest_para[16];
	gFuncTorque		    =rest_para[17];
//	gCurrentPulseNum    =rest_para[18];
//	gMAXPulseNum	    =rest_para[19];
	
	gLimitMode			=rest_para[20];
	gBehindLimit		=rest_para[21];
	gCenterLimit		=rest_para[22];
	gFrontLimit	     	=rest_para[23];
	
	gSPCMode		 	=rest_para[24];
	gSPCStopTime		=rest_para[25];
	gSPCBehindLimit	    =rest_para[26];
	gSPCFrontLimit	    =rest_para[27];
	
	gNoWaitEN			=rest_para[28];
	gNoDetectValve    	=rest_para[29];
	gNoDetectTime     	=rest_para[30];
	gNoWaitTime	        =rest_para[31];
	
	gLongIo0Mode		=rest_para[32];
	gLongIo1Mode		=rest_para[33];
	gLongIo2Mode		=rest_para[34];
	gLongIo3Mode		=rest_para[35];

	gAFlexAcc           =rest_para[36];
	gAFlexDec           =rest_para[37];
	gMFlexAcc           =rest_para[38];
	gMFlexDec           =rest_para[39];
	
	gAFlexSacc            =rest_para[40];
	gAFlexSdec            =rest_para[41];
	gMFlexSacc            =rest_para[42];
	gMFlexSdec            =rest_para[43];
	
	gLanguageChoice		  =rest_para[44];
	gLockTime             =rest_para[45];
	gBuzzer                =rest_para[46];
	gBrightness            =rest_para[47];
	Password[0]				=rest_para[48]&0xff;
	Password[1]				=rest_para[48]>>8;
	Password[2]				=rest_para[49]&0xff;
	Password[3]				=rest_para[49]>>8;
	Password[4]				=rest_para[50]&0xff;
	Password[5]				=rest_para[50]>>8;
}

void WriteSensor()   //传感器值保存
{
	OS_CPU_SR  cpu_sr;
	
	SensorValue[0]=g_Mat0_Sensor1_H;
	SensorValue[1]=g_Mat0_Sensor1_L;
	SensorValue[2]=g_Mat0_Sensor2_H;
	SensorValue[3]=g_Mat0_Sensor2_L;
	SensorValue[4]=Mat0EPC12;
	
	SensorValue[5]=g_Mat1_Sensor1_H;
	SensorValue[6]=g_Mat1_Sensor1_L;
	SensorValue[7]=g_Mat1_Sensor2_H;
	SensorValue[8]=g_Mat1_Sensor2_L;
	SensorValue[9]=Mat1EPC12;
	
	SensorValue[10]=g_Mat2_Sensor1_H;
	SensorValue[11]=g_Mat2_Sensor1_L;
	SensorValue[12]=g_Mat2_Sensor2_H;	    
	SensorValue[13]=g_Mat2_Sensor2_L;
	SensorValue[14]=Mat2EPC12;
	
	SensorValue[15]=g_Mat3_Sensor1_H;
	SensorValue[16]=g_Mat3_Sensor1_L;
	SensorValue[17]=g_Mat3_Sensor2_H;
	SensorValue[18]=g_Mat3_Sensor2_L;
	SensorValue[19]=Mat3EPC12;
	
	SensorValue[20]=g_Mat4_Sensor1_H;
	SensorValue[21]=g_Mat4_Sensor1_L;
	SensorValue[22]=g_Mat4_Sensor2_H;
	SensorValue[23]=g_Mat4_Sensor2_L;
	SensorValue[24]=Mat4EPC12;
	
	SensorValue[25]=g_Mat5_Sensor1_H;
	SensorValue[26]=g_Mat5_Sensor1_L;
	SensorValue[27]=g_Mat5_Sensor2_H;
	SensorValue[28]=g_Mat5_Sensor2_L;
	SensorValue[29]=Mat5EPC12;
	
	SensorValue[30]=g_Mat6_Sensor1_H;
	SensorValue[31]=g_Mat6_Sensor1_L;
	SensorValue[32]=g_Mat6_Sensor2_H;
	SensorValue[33]=g_Mat6_Sensor2_L;
	SensorValue[34]=Mat6EPC12;
	
	SensorValue[35]=g_Mat7_Sensor1_H;
	SensorValue[36]=g_Mat7_Sensor1_L;
	SensorValue[37]=g_Mat7_Sensor2_H;
	SensorValue[38]=g_Mat7_Sensor2_L;
	SensorValue[39]=Mat7EPC12;

	OS_ENTER_CRITICAL();   //关闭中断
	STMFLASH_Write(FLASH_SV_ADDR, (u16*)SensorValue, SIZE);        //WriteAddr:起始地址    pBuffer:数据指针    NumToWrite:半字(16位)数 
	OS_EXIT_CRITICAL();    //打开中断  
}


void ReadSensor()    //传感器值读取
{
	OS_CPU_SR  cpu_sr;
	
	OS_ENTER_CRITICAL();   //关闭中断
	STMFLASH_Read(FLASH_SV_ADDR, (u16*)SensorValue, SIZE);  
	OS_EXIT_CRITICAL();    //打开中断
	
	g_Mat0_Sensor1_H      	=SensorValue[0];
	g_Mat0_Sensor1_L  		=SensorValue[1];
	g_Mat0_Sensor2_H		=SensorValue[2];
	g_Mat0_Sensor2_L		=SensorValue[3];
	Mat0EPC12			    =SensorValue[4];
	
	g_Mat1_Sensor1_H		=SensorValue[5];
	g_Mat1_Sensor1_L		=SensorValue[6];
	g_Mat1_Sensor2_H		=SensorValue[7];
	g_Mat1_Sensor2_L		=SensorValue[8];
	Mat1EPC12				=SensorValue[9];
	
	g_Mat2_Sensor1_H		=SensorValue[10];
	g_Mat2_Sensor1_L	 	=SensorValue[11];
	g_Mat2_Sensor2_H  		=SensorValue[12];
	g_Mat2_Sensor2_L 		=SensorValue[13];
	Mat2EPC12 				=SensorValue[14];
	
	g_Mat3_Sensor1_H  		=SensorValue[15];
	g_Mat3_Sensor1_L	    =SensorValue[16];
	g_Mat3_Sensor2_H	    =SensorValue[17];
	g_Mat3_Sensor2_L		=SensorValue[18];
	Mat3EPC12		        =SensorValue[19];
	
	g_Mat4_Sensor1_H	    =SensorValue[20];
	g_Mat4_Sensor1_L		=SensorValue[21];
	g_Mat4_Sensor2_H		=SensorValue[22];
	g_Mat4_Sensor2_L		=SensorValue[23];
	Mat4EPC12    	        =SensorValue[24];
	
	g_Mat5_Sensor1_H     	=SensorValue[25];
	g_Mat5_Sensor1_L	    =SensorValue[26];
	g_Mat5_Sensor2_H	    =SensorValue[27];
	g_Mat5_Sensor2_L		=SensorValue[28];
	Mat5EPC12	            =SensorValue[29];
	
	g_Mat6_Sensor1_H	    =SensorValue[30];
	g_Mat6_Sensor1_L		=SensorValue[31];
	g_Mat6_Sensor2_H		=SensorValue[32];
	g_Mat6_Sensor2_L     	=SensorValue[33];
	Mat6EPC12		        =SensorValue[34];
	
	g_Mat7_Sensor1_H		=SensorValue[35];
	g_Mat7_Sensor1_L		=SensorValue[36];
	g_Mat7_Sensor2_H		=SensorValue[37];
	g_Mat7_Sensor2_L	    =SensorValue[38];
	Mat7EPC12               =SensorValue[39];
	
}





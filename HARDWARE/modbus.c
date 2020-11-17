#include "modbus.h"
#include "usart.h"
#include "main.h"
//#include "stdlib.h"
#include "doublebuf.h"

#define RegData  64

MODBUS modbus;


extern  u8 RS485_RX_BUFF[];	//接收缓冲区2048字节
u16 RS485_RX_CNT=0;			//接收计数器
u8 RS485_RxFlag=0;			//接收一帧结束标记

extern  u8 RS485_TX_BUFF[];	//发送缓冲区
u16 RS485_TX_CNT=0;			//发送计数器
u8 RS485_TxFlag=0;			//发送一帧结束标记


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Slave寄存器和单片机寄存器的映射关系
u16   Slave_InputIO[RegData];  //输入开关量寄存器(这里使用的是位带操作)   注意： 这里储存从机返回的数据。    开关量的数据只能是0，1 例如 Slave_InputIO[5]=0；Slave_InputIO[8]=1；
u16   Slave_OutputIO[RegData]; //输出开关量寄存器(这里使用的是位带操作)    功能码 05 15

u16   Slave_ReadReg[40]  = {0};    //只读寄存器----存储数据，主机只能读取          功能码 03
u16   Slave_WriteReg[40] = {0};   //写寄存器-------主机可以修改该寄存器   功能码 06 16


/**************04功能码读取输入寄存器****************/
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

/**************06功能码写入保持寄存器****************/
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
u16 *const pHMatNum 	 = &Slave_WriteReg[0x15];  // 当前材料编号
u16 *const pHSCaliStep   = &Slave_WriteReg[0x16];  //传感器校准步骤
u16 *const pHSensorNum   = &Slave_WriteReg[0x17];

u16 *const pHTCaliTorque = &Slave_WriteReg[0x18];    //行程校准电流设置
u16 *const pHTCaliFlag   = &Slave_WriteReg[0x19];    //是否启动行程校准
u16 *const pHTCaliCancel = &Slave_WriteReg[0x1a];    //是否取消行程校准

u16 *const pHFlexSpeed	 = &Slave_WriteReg[0x1b];    //柔性加减速值

u16 *const pHSysYes		 = &Slave_WriteReg[0x1f];    //确定执行

u16 *const pHWarmFlag	 = &Slave_WriteReg[0x20];    //警告标志

//CRC校验 自己后面添加的
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

// Modbus初始化函数
void Modbus_Init()
{
    modbus.myadd = 0x01;   //从机设备地址为1
    modbus.timrun = 0;     //modbus定时器停止计算
	
//	modbus.rcbuf = (u8 *)malloc(128);   //分配空间字节数：6+寄存器
//	Modbus_Time_Init();
	
/*********初始化modbus保持寄存器***********/
	*pHAdjustFlag=255;
	*pHClickBut=0;
	*pHAddSub=0;
	
	*pHParaConfirm=0;   //参数修改确认  1表示确认，2表示还原，0表示不执行
	
	*pHSCaliStep=0;     //传感器校准步骤
	*pHSensorNum=0;     //传感器校准默认选中EPC1
	*pHTCaliTorque=0;   //校准电流
	*pHTCaliFlag=0;
	*pHTCaliCancel=0;
	
	
	*pHSensorMode = gSensorMode;		//0=左，1=右，2=左+右，3=SPC
	*pHAutoPolar = gAutoPolar;			//自动极性，0=负极，1=正极;
	*pHManuPolar = gManuPolar;			//手动极性，0=负极，1=正极;
	*pHMotorType = gMotorType;     		//电机类型   0无刷电机4   1无刷电机5   2有刷电机
	*pHPowerOnMode = gPowerOnMode;   	//开机工作模式，,0=手动，1=自动，2=中心，3=上次 
	*pHMatNum = gCurMatNum;        	// 当前材料编号
	
	*pHLimitMode = gLimitMode;    		//限位方式   0为内部限位、1为外部限位开关限位、2内部加外部
	*pHSPCMode   = gSPCMode;			//蛇形纠偏模式  0：内部编码器  1：外部传感器
	*pHNoWaitEN  = gNoWaitEN;     		//无料等待功能，0=使能,1=禁止
	
	*pHPort0Fun = gLongIo0Mode;      	//四个端口默认功能
	*pHPort1Fun = gLongIo1Mode;
	*pHPort2Fun = gLongIo2Mode;
	*pHPort3Fun = gLongIo3Mode;
	*pHFlexSpeed = 0;
	*pHSysYes = 255;
	*pHWarmFlag  = 0;
}


// Modbus 3号功能码函数
// Modbus 主机读取保持寄存器值
void Modbus_Func3()
{
    u16 Regadd,Reglen,crc;
	u8 i=0,j;	
	
	//得到要读取寄存器的首地址
	Regadd = (modbus.rcbuf[2]<<8)+modbus.rcbuf[3];
	//得到要读取寄存器的个数
	Reglen = (modbus.rcbuf[4]<<8)+modbus.rcbuf[5];
	
//	modbus.sendbuf = (u8 *)malloc((5+Reglen*2)*sizeof(char));   //分配空间字节数：6+寄存器数量*2（每个寄存器两个字节数据）
	
	//发送回应数据包
	modbus.sendbuf[i++] = modbus.myadd;      //发送本机设备地址
	modbus.sendbuf[i++] = 0x03;              //发送功能码
    modbus.sendbuf[i++] = ((Reglen*2)%256);  //返回字节个数
	for(j=0;j<Reglen;j++)                    //返回数据
	{
		modbus.sendbuf[i++] = Slave_ReadReg[Regadd+j]/256;    //高8位
		modbus.sendbuf[i++] = Slave_ReadReg[Regadd+j]%256;    //低8位
	}
	crc = CRC_Compute(modbus.sendbuf,i);    //计算要返回数据的CRC
	modbus.sendbuf[i++] = crc/256;
	modbus.sendbuf[i++] = crc%256;
	
	// 开始返回Modbus数据
#ifdef USE_MAX_485
	RS485_SendData(&modbus.sendbuf[0],i);	
#elif defined USE_MAX_232
	RS232_SendData(&modbus.sendbuf[0],i);	
#endif	
//	free(modbus.sendbuf);
}

// Modbus 4号功能码函数
// Modbus 主机读取输入寄存器值
void Modbus_Func4()
{
    u16 Regadd,Reglen,crc;
	u8 i=0,j;	
	
	//得到要读取寄存器的首地址
	Regadd = (modbus.rcbuf[2]<<8)+modbus.rcbuf[3];
	//得到要读取寄存器的个数
	Reglen = (modbus.rcbuf[4]<<8)+modbus.rcbuf[5];
	
//	modbus.sendbuf = (u8 *)malloc((5+Reglen*2)*sizeof(char));   //分配空间字节数：6+寄存器数量*2（每个寄存器两个字节数据）
	
	//发送回应数据包
	modbus.sendbuf[i++] = modbus.myadd;      //发送本机设备地址
	modbus.sendbuf[i++] = 0x04;              //发送功能码
    modbus.sendbuf[i++] = ((Reglen*2)%256);  //返回字节个数
	for(j=0;j<Reglen;j++)                    //返回数据
	{
		modbus.sendbuf[i++] = Slave_ReadReg[Regadd+j]/256;    //高8位
		modbus.sendbuf[i++] = Slave_ReadReg[Regadd+j]%256;    //低8位
	}
	crc = CRC_Compute(modbus.sendbuf,i);    //计算要返回数据的CRC
	modbus.sendbuf[i++] = crc/256;
	modbus.sendbuf[i++] = crc%256;
	
	// 开始返回Modbus数据
#ifdef USE_MAX_485
	RS485_SendData(&modbus.sendbuf[0],i);	
#elif defined USE_MAX_232
	RS232_SendData(&modbus.sendbuf[0],i);	
#endif
//	free(modbus.sendbuf);
}

// Modbus 6号功能码函数
// Modbus 主机写入保持寄存器值
void Modbus_Func6()  
{
	u16 Regadd;
	u16 val;
	u16 i=0,crc;
	Regadd = (modbus.rcbuf[2]<<8)+modbus.rcbuf[3]; 	 //得到要修改的地址 
	val = (modbus.rcbuf[4]<<8)+modbus.rcbuf[5];    	 //修改后的值
	Slave_WriteReg[Regadd]=val; 					 //修改本设备相应的寄存器
	
//	modbus.sendbuf = (u8 *)malloc(8*sizeof(char));
	
	//以下为回应主机
	modbus.sendbuf[i++] = modbus.myadd;		//本设备地址
    modbus.sendbuf[i++] = 0x06;        		//功能码 
    modbus.sendbuf[i++] = Regadd/256;
	modbus.sendbuf[i++] = Regadd%256;
	modbus.sendbuf[i++] = val/256;
	modbus.sendbuf[i++] = val%256;
	
	crc=CRC_Compute(modbus.sendbuf,i);  	//计算校验码
	modbus.sendbuf[i++] = crc/256;  
	modbus.sendbuf[i++] = crc%256;
	
	// 开始返回Modbus数据
#ifdef USE_MAX_485
	RS485_SendData(&modbus.sendbuf[0],i);	
#elif defined USE_MAX_232
	RS232_SendData(&modbus.sendbuf[0],i);	
#endif	
//	free(modbus.sendbuf);
}

//Modbus事件处理函数
void Modbus_Event()
{
	u16 crc,rccrc;
	
	if(modbus.reflag == 0)  				//没有收到数据包
	{
	   return;
	}
	if(modbus.recount>2) 					//需要增加，否则被干扰后，有可能导致while无法退出
    {
		//收到数据包，通过读到的数据帧计算CRC
		crc = CRC_Compute(&modbus.rcbuf[0],modbus.recount-2); 
		// 读取数据帧的CRC
		rccrc = (modbus.rcbuf[modbus.recount-2]<<8)+modbus.rcbuf[modbus.recount-1];
	}
	if(crc == rccrc) 						 	//CRC检验成功 开始分析包
	{	
		if(modbus.rcbuf[0] == modbus.myadd)  	// 检查地址是否时自己的地址
		{
			switch(modbus.rcbuf[1])   		 	//分析modbus功能码
			{
				case 0:             break;
				case 1:             break;
				case 2:             break;
				case 3:      Modbus_Func3();      break;        //解析主机发送的功能码，并调用相应函数返回指令
				case 4:      Modbus_Func4();      break;
				case 5:             break;
				case 6:      Modbus_Func6();      break;
				case 7:             break;
				case 8:             break;
				case 9:             break;			 
			}
		}
		else if(modbus.rcbuf[0] == 0) 		//广播地址不予回应
		{

		}	 
	}	
	modbus.recount = 0;
	modbus.reflag = 0; 
}





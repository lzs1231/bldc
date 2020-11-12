#ifndef __MODBUS_H__
#define __MODBUS_H__

#include "sys.h"


#define READ_COIL     01
#define READ_DI       02
#define READ_HLD_REG  03
#define READ_AI       04
#define SET_COIL      05
#define SET_HLD_REG   06
#define WRITE_COIL    15
#define WRITE_HLD_REG 16

typedef struct 
{
    u8  myadd;        //本设备从机地址
	u8  rcbuf[20];       //modbus接受缓冲区
	u8  timout;       //modbus数据持续时间
	u8  recount;      //modbus端口接收到的数据个数
	u8  timrun;       //modbus定时器是否计时标志
	u8  reflag;       //modbus一帧数据接受完成标志位
	u8  sendbuf[60];     //modbus发送缓冲区
	
}MODBUS;

extern MODBUS modbus;

extern u16   Slave_ReadReg[];    //只读寄存器----存储数据，主机只能读取          功能码 03
extern u16   Slave_WriteReg[];   //写寄存器-------主机可以修改该寄存器   功能码 06 16
void Modbus_Init(void);
void Modbus_Func3(void);
void Modbus_Event(void);

#define HI(n) ((n)>>8)
#define LOW(n) ((n)&0xff)


void Master_Service(u8 SlaverAddr,u8 Fuction,u16 StartAddr,u16 ValueOrLenth);        //发送数据，从机地址，功能码，起始地址，数据或长度
u8 RS485_RX_Service(void);
void Modbus_Init(void);


#endif


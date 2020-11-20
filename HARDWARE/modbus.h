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

typedef enum
{
	Error,
	MAX232,
	MAX485,
}Methods;

typedef struct 
{
    u8  myadd;        //本设备从机地址
	u8  rcbuf[20];    //modbus接受缓冲区
	u8  timout;       //modbus数据持续时间
	u8  recount;      //modbus端口接收到的数据个数
	u8  timrun;       //modbus定时器是否计时标志
	u8  reflag;       //modbus一帧数据接受完成标志位
	u8  sendbuf[60];  //modbus发送缓冲区
	Methods  methods;      //通讯方式
}MODBUS;

extern MODBUS modbus;

extern u16   Slave_ReadReg[];    //只读寄存器----存储数据，主机只能读取          功能码 03
extern u16   Slave_WriteReg[];   //写寄存器-------主机可以修改该寄存器   功能码 06 16


#define HI(n) ((n)>>8)
#define LOW(n) ((n)&0xff)


void Modbus_Init(void);
void Modbus_Event(void);

#endif


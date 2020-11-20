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
    u8  myadd;        //���豸�ӻ���ַ
	u8  rcbuf[20];    //modbus���ܻ�����
	u8  timout;       //modbus���ݳ���ʱ��
	u8  recount;      //modbus�˿ڽ��յ������ݸ���
	u8  timrun;       //modbus��ʱ���Ƿ��ʱ��־
	u8  reflag;       //modbusһ֡���ݽ�����ɱ�־λ
	u8  sendbuf[60];  //modbus���ͻ�����
	Methods  methods;      //ͨѶ��ʽ
}MODBUS;

extern MODBUS modbus;

extern u16   Slave_ReadReg[];    //ֻ���Ĵ���----�洢���ݣ�����ֻ�ܶ�ȡ          ������ 03
extern u16   Slave_WriteReg[];   //д�Ĵ���-------���������޸ĸüĴ���   ������ 06 16


#define HI(n) ((n)>>8)
#define LOW(n) ((n)&0xff)


void Modbus_Init(void);
void Modbus_Event(void);

#endif


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
    u8  myadd;        //���豸�ӻ���ַ
	u8  rcbuf[20];       //modbus���ܻ�����
	u8  timout;       //modbus���ݳ���ʱ��
	u8  recount;      //modbus�˿ڽ��յ������ݸ���
	u8  timrun;       //modbus��ʱ���Ƿ��ʱ��־
	u8  reflag;       //modbusһ֡���ݽ�����ɱ�־λ
	u8  sendbuf[60];     //modbus���ͻ�����
	
}MODBUS;

extern MODBUS modbus;

extern u16   Slave_ReadReg[];    //ֻ���Ĵ���----�洢���ݣ�����ֻ�ܶ�ȡ          ������ 03
extern u16   Slave_WriteReg[];   //д�Ĵ���-------���������޸ĸüĴ���   ������ 06 16
void Modbus_Init(void);
void Modbus_Func3(void);
void Modbus_Event(void);

#define HI(n) ((n)>>8)
#define LOW(n) ((n)&0xff)


void Master_Service(u8 SlaverAddr,u8 Fuction,u16 StartAddr,u16 ValueOrLenth);        //�������ݣ��ӻ���ַ�������룬��ʼ��ַ�����ݻ򳤶�
u8 RS485_RX_Service(void);
void Modbus_Init(void);


#endif


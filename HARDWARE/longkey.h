#ifndef  __LONGKEY_H__
#define  __LONGKEY_H__

#include "sys.h"

#define	LKEY_GPIO	  GPIOC     
#define	LKEY1_PIN	  GPIO_Pin_12
#define	LKEY2_PIN	  GPIO_Pin_13
#define	LKEY3_PIN	  GPIO_Pin_14
#define	LKEY4_PIN	  GPIO_Pin_15



/********�˿ڶ���*******/
enum PortFunType
{
	Nodef       = 0,
	PusherLeft,
	PusherRight,
	PusherCenter,
	ManuDef,
	AutoDef,
	OperPrepar,
	Limit1,
	Limit2,
	Switch1_EPC,
	Switch2_EPC,
	FunNum,
};

typedef  struct  MyPortFun{
	u16 *PPort;                 //ָ��Զ�̶˿ڹ���
	GPIO_TypeDef *LKEY_PORT;
	u16 LKEY_IO;
	u8  LKEY_Buf;               //�����жϰ���ʱ��
	u8  IOState;                //IO��״̬
	u8  LastIOState;            //IO���ϴ�״̬
	u16  DisNum;                //��ʾ��Ӧ��ͼ��ؼ����
}PortFunDef;


extern PortFunDef PortFun[4];

void KeyHandle(PortFunDef *Port,u16 *FuncFlag);
void ReadLongIO(PortFunDef *PPort,u16 *FuncFlag);
void ReadLong(u16 *LongPortFun);






#endif


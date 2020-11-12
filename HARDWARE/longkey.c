#include "main.h"
#include "longkey.h"


PortFunDef PortFun[4] = {
	{&gLongIo0Mode,GPIOC,GPIO_Pin_11,0,0,0,33},
	{&gLongIo1Mode,GPIOC,GPIO_Pin_12,0,0,0,34},
	{&gLongIo2Mode,GPIOB,GPIO_Pin_8,0,0,0,35},
	{&gLongIo3Mode,GPIOB,GPIO_Pin_9,0,0,0,36}
};

void KeyHandle(PortFunDef *PPort,u16 *pLongPortFun)
{
	switch(*(*PPort).PPort)
	{
		case Nodef:                    break;
	
		case PusherLeft: 
			if(PPort->IOState == 1)       {pLongPortFun[PusherLeft]=2;      pLongPortFun[PusherRight]=2;   }
			else if(PPort->IOState == 0)  {pLongPortFun[PusherLeft]=0;   	pLongPortFun[PusherRight]=0; 	}
		break;
		case PusherRight:  
			if(PPort->IOState == 1)       {pLongPortFun[PusherLeft]=1; 		pLongPortFun[PusherRight]=1;    }
			else if(PPort->IOState == 0)  {pLongPortFun[PusherLeft]=0;		pLongPortFun[PusherRight]=0; }
		break;
		case PusherCenter: 
			if(PPort->IOState == 1)       pLongPortFun[PusherCenter]=5;   //=1��ʾִ�����Ĺ�λ
			else if(PPort->IOState == 0)  pLongPortFun[PusherCenter]=0;	
		break;
		case ManuDef:   
			if(PPort->IOState == 1)       pLongPortFun[ManuDef]=4;    //�̽��ֶ�����������Ϊ�ֶ�
			else if(PPort->IOState == 0)  pLongPortFun[ManuDef]=2;	
		break;
		case AutoDef:   
			if(PPort->IOState == 1)       pLongPortFun[AutoDef]=2;    //�̽��Զ�����������Ϊ�Զ�
			else if(PPort->IOState == 0)  pLongPortFun[AutoDef]=4;
		break;
		case OperPrepar:  
			if(PPort->IOState == 1)       pLongPortFun[OperPrepar]=1;     
			else if(PPort->IOState == 0)  pLongPortFun[OperPrepar]=0; 
		break;
		case Limit1:  
			if(PPort->IOState == 1)       pLongPortFun[Limit1]=1;        //=1��ʾ����������λ����
			else if(PPort->IOState == 0)  pLongPortFun[Limit1]=0;
		break;
		case Limit2:  
			if(PPort->IOState == 1)       pLongPortFun[Limit2]=1;         //=1��ʾ���������λ����
			else if(PPort->IOState == 0)  pLongPortFun[Limit2]=0;            
		break;
		case Switch1_EPC: 
			if(gSensorMode==0||gSensorMode==1)
			{
				if(PPort->IOState == 1)       pLongPortFun[Switch1_EPC]=1;         //
				else if(PPort->IOState == 0)  pLongPortFun[Switch1_EPC]=0; 
			}          	
		break;
		case Switch2_EPC:  
			if(gSensorMode==0||gSensorMode==1)
			{
				if(PPort->IOState == 1)       pLongPortFun[Switch2_EPC]=1;        //����ΪEPC2
				else if(PPort->IOState == 0)  pLongPortFun[Switch2_EPC]=0;        //�ɿ�ΪEPC1
			}
		break;
		default: break;                                         //=1��ʾ�ص����ĵ���
	}
} 

void ReadLongIO(PortFunDef *PPort,u16 *pLongPortFun)
{
	PPort->LKEY_Buf<<=1;    //ÿ��10ms����һ��

	if(GPIO_ReadInputDataBit(PPort->LKEY_PORT, (*PPort).LKEY_IO)==0)   PPort->LKEY_Buf+=1;//��ȡλ�͵�ƽʱ��1

	if((PPort->LKEY_Buf&0x07) ==0x07)      (*PPort).IOState=1;    //��������
	else if((PPort->LKEY_Buf&0x07) ==0x00) (*PPort).IOState=0;    //ָʾbutton �Ƿ��¡������ɿ�  
	
	if(PPort->LastIOState != PPort->IOState)          //�жϰ���״̬�Ƿ�仯,ֻ��״̬��ʽ�仯��
	{
		KeyHandle(PPort,pLongPortFun);
	}
	PPort->LastIOState = PPort->IOState;	
}

void ReadLong(u16 *pLongPortFun)
{
	ReadLongIO(&PortFun[0],pLongPortFun);
	ReadLongIO(&PortFun[1],pLongPortFun);
	ReadLongIO(&PortFun[2],pLongPortFun);
	ReadLongIO(&PortFun[3],pLongPortFun);
}




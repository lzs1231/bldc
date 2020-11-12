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
			if(PPort->IOState == 1)       pLongPortFun[PusherCenter]=5;   //=1表示执行中心归位
			else if(PPort->IOState == 0)  pLongPortFun[PusherCenter]=0;	
		break;
		case ManuDef:   
			if(PPort->IOState == 1)       pLongPortFun[ManuDef]=4;    //短接手动，按键按下为手动
			else if(PPort->IOState == 0)  pLongPortFun[ManuDef]=2;	
		break;
		case AutoDef:   
			if(PPort->IOState == 1)       pLongPortFun[AutoDef]=2;    //短接自动，按键按下为自动
			else if(PPort->IOState == 0)  pLongPortFun[AutoDef]=4;
		break;
		case OperPrepar:  
			if(PPort->IOState == 1)       pLongPortFun[OperPrepar]=1;     
			else if(PPort->IOState == 0)  pLongPortFun[OperPrepar]=0; 
		break;
		case Limit1:  
			if(PPort->IOState == 1)       pLongPortFun[Limit1]=1;        //=1表示到达缩进限位点了
			else if(PPort->IOState == 0)  pLongPortFun[Limit1]=0;
		break;
		case Limit2:  
			if(PPort->IOState == 1)       pLongPortFun[Limit2]=1;         //=1表示到达伸出限位点了
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
				if(PPort->IOState == 1)       pLongPortFun[Switch2_EPC]=1;        //按下为EPC2
				else if(PPort->IOState == 0)  pLongPortFun[Switch2_EPC]=0;        //松开为EPC1
			}
		break;
		default: break;                                         //=1表示回到中心点了
	}
} 

void ReadLongIO(PortFunDef *PPort,u16 *pLongPortFun)
{
	PPort->LKEY_Buf<<=1;    //每隔10ms左移一次

	if(GPIO_ReadInputDataBit(PPort->LKEY_PORT, (*PPort).LKEY_IO)==0)   PPort->LKEY_Buf+=1;//读取位低电平时加1

	if((PPort->LKEY_Buf&0x07) ==0x07)      (*PPort).IOState=1;    //按键按下
	else if((PPort->LKEY_Buf&0x07) ==0x00) (*PPort).IOState=0;    //指示button 是否按下。按键松开  
	
	if(PPort->LastIOState != PPort->IOState)          //判断按键状态是否变化,只有状态方式变化才
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




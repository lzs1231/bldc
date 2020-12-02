#include <string.h>
#include "delay.h"
#include "lcdtest.h"
#include "main.h"
#include "stmflash.h"
#include "modbus.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

#define getstate(a,b)	(a==b?1:0)

extern  u8 OcclusionL_rate,OcclusionR_rate;             //�������ڵ���


ParaStorageDef ParaStorage={SRestore,{Backup,Restore,ParameterInit}};

TravelCalDef TravelCal = {0,0,0};						


MatCalDef MatCal = {            //����У׼ 
	{
		{&g_Mat0_SensorL_H,&g_Mat0_SensorL_L,&g_Mat0_SensorR_H,&g_Mat0_SensorR_L,&Mat0EPC12},
		{&g_Mat1_SensorL_H,&g_Mat1_SensorL_L,&g_Mat1_SensorR_H,&g_Mat1_SensorR_L,&Mat1EPC12},
		{&g_Mat2_SensorL_H,&g_Mat2_SensorL_L,&g_Mat2_SensorR_H,&g_Mat2_SensorR_L,&Mat2EPC12},
		{&g_Mat3_SensorL_H,&g_Mat3_SensorL_L,&g_Mat3_SensorR_H,&g_Mat3_SensorR_L,&Mat3EPC12},
		{&g_Mat4_SensorL_H,&g_Mat4_SensorL_L,&g_Mat4_SensorR_H,&g_Mat4_SensorR_L,&Mat4EPC12},
		{&g_Mat5_SensorL_H,&g_Mat5_SensorL_L,&g_Mat5_SensorR_H,&g_Mat5_SensorR_L,&Mat5EPC12},
		{&g_Mat6_SensorL_H,&g_Mat6_SensorL_L,&g_Mat6_SensorR_H,&g_Mat6_SensorR_L,&Mat6EPC12},
		{&g_Mat7_SensorL_H,&g_Mat7_SensorL_L,&g_Mat7_SensorR_H,&g_Mat7_SensorR_L,&Mat7EPC12},
	},
	&gCurMatNum,       //���ϱ��Ĭ��0
	0,0,0,0,0,NoShelterDis,ShelterDis,RealTimeDis
};

ConfirmDataDef ConfirmData = {
	1,0,
	{
		{0,160,&gFineTune,1},            //ƫ����
		{1,200,&gGainData,1},            //����
		{0,80,&gDeadZone,1},             //ä��
		{5,95,&gCenterLimit,1},          //������λ
		{1,200,&gGainData,1},            //����
		{1,200,&gGainData,1},            //����
		{0,80,&gDeadZone,1},             //ä��
		{100,3000,&gManuSpeed,100},    	 //�ֶ��ٶ�
		{100,3000,&gAutoSpeed,100},      //�Զ��ٶ�
		{100,3000,&gCentSpeed,100},   	 //�����ٶ�
		
		{20,100,&gAFlexAcc,1},			 //�Զ�����
		{20,100,&gAFlexDec,1},			 //�Զ�����
		{20,100,&gMFlexAcc,1},			 //�ֶ�����
		{20,100,&gMFlexDec,1},			 //�ֶ�����
		
		{2,10,&gFuncTorque,1},			 //���е���
		
		{5,95,&gExtendLimit,1},          //�����λ
		{5,95,&gCenterLimit,1},          //������λ
		{5,95,&gIndentLimit,1},			 //������λ
		
		{2,250,&gSPCStopTime,1},		 //SPCֹͣʱ��
		{10,90,&gSPCExtendLimit,1},		 //SPC�����λ
		{10,90,&gSPCIndentLimit,1},		 //SPC������λ
		
		{5,95,&gNoDetectValve,1},		 //������ֵ
		{1,100,&gNoDetectTime,1},		 //���ϼ��ʱ��
		{1,100,&gNoWaitTime,1},			 //���ϵȴ�ʱ��

		{0,160,&gFineTune,1},            //ƫ����
	},
};


/**********�����־����***************/
u8 WarmOut(void)
{
	u8 icnt,HallData;
	HallData  = (u8)((GPIOC->IDR&0x000001c0)>>6);        //��ת��λ��    �ϳ�GPIOC8|GPIOC7|GPIOC6 
	
	if(HallData>6||HallData<1)   	Warm[NotConFlag] = NotConFlag;
	else 						   	Warm[NotConFlag] = NoErrFlag; 
	
	if(*pHWarmFlag == 1)     		Warm[LockFlag] = LockFlag;
	else if(*pHWarmFlag == 3)  		Warm[ProhibitFlag] = ProhibitFlag;
	else 					   	   {Warm[LockFlag] = NoErrFlag;     Warm[ProhibitFlag] = NoErrFlag;}
	
	/**************�򿪼̵���**********************/
	switch(*pHAlarmSwitch)
	{								
		case 0:		PDout(2) = gRelay==0?getstate(Warm[LimitFlag],LimitFlag):~getstate(Warm[LimitFlag],LimitFlag); 	break;
		case 1:		PDout(2) = gRelay==0?getstate(Warm[BreakAlarmFlag],BreakAlarmFlag):~getstate(Warm[BreakAlarmFlag],BreakAlarmFlag);	break;
		case 2:		PDout(2) = gRelay==0?(getstate(Warm[LimitFlag],LimitFlag)|getstate(Warm[BreakAlarmFlag],BreakAlarmFlag)):~(getstate(Warm[LimitFlag],LimitFlag)|getstate(Warm[BreakAlarmFlag],BreakAlarmFlag)); break;
		default:	break;
	}
	
	for(icnt=WarmNum-1;icnt>0;icnt--)
	{
		if(Warm[icnt] != 0)   return Warm[icnt];           
	}
	return 0;  
}	  


/***********������ģʽ��Ӧ����ʾ����***************/
void SensorDis(void)               
{
	*pSensorRate = (OcclusionL_rate<<8&0xff00)|(OcclusionR_rate&0x00ff);          //��8λ��ʾ��  ��8λ��ʾ�� 
}

/**************������У׼������ʾ����*******************/
u8 NoShelterDis(void)
{
	switch(*pHSensorNum)
	{
		case EPC1:    
			MatCal.t_SensorL_valueH = SensorL_value;
			*pSensorValue	=	(SensorL_value|(*pHMatNum<<12))&0x7fff;  //���λΪ0����ʾEPC1
		break;          
		case EPC2:    
			MatCal.t_SensorR_valueH = SensorR_value;
			*pSensorValue	=	(SensorR_value|(*pHMatNum<<12))|0x8000;  //���λΪ1����ʾEPC2
		break;          
		default :                                break;
	}
	return 0 ;
}

u8 ShelterDis(void)
{
	switch(*pHSensorNum)
	{
		case EPC1:
			MatCal.t_SensorL_valueL = SensorL_value;
			*pSensorValue	=	(SensorL_value|(*pHMatNum<<12))&0x7fff;  //���λΪ0����ʾEPC1
			MatCal.CaliFlag = 1;  
		break;          
		case EPC2:    
			MatCal.t_SensorR_valueL = SensorR_value;
			*pSensorValue	=	(SensorR_value|(*pHMatNum<<12))|0x8000;  //���λΪ1����ʾEPC2
			MatCal.CaliFlag = 1;     //��ʾУ׼���
		break;          
		default :                                break;
	}
	return 0 ;
}

u8 RealTimeDis(void)
{
	switch(*pHSensorNum)
	{
		case EPC1: 
			*pSensorValue	=	(SensorL_value|(*pHMatNum<<12))&0x7fff;  //���λΪ0����ʾEPC1
			if((MatCal.t_SensorL_valueH-MatCal.t_SensorL_valueL > 125))   
			{
				if(MatCal.CaliFlag == 1)   
				{
					*MatCal.MatValue[*pHMatNum].PSensorL_H = MatCal.t_SensorL_valueH;
					*MatCal.MatValue[*pHMatNum].PSensorL_L = MatCal.t_SensorL_valueL;
					*MatCal.MatValue[*pHMatNum].PEPC12 = *MatCal.MatValue[*pHMatNum].PEPC12|(1<<1);
					WriteSensor();   
					MatCal.CaliFlag = 0;         //У׼��ɱ�־
					return 0 ;
				}
			}else if((MatCal.t_SensorL_valueH-MatCal.t_SensorL_valueL < -125)){ 
				if(MatCal.CaliFlag == 1)   
				{
					*MatCal.MatValue[*pHMatNum].PSensorL_H = MatCal.t_SensorL_valueL;
					*MatCal.MatValue[*pHMatNum].PSensorL_L = MatCal.t_SensorL_valueH;	
					*MatCal.MatValue[*pHMatNum].PEPC12 = *MatCal.MatValue[*pHMatNum].PEPC12|(1<<1);
					WriteSensor(); 	
					MatCal.CaliFlag = 0;        //У׼��ɱ�־
					return 0 ;     				//������ʾУ׼�ɹ�
				}
			}else{
				return 1;         				//������ʾУ׼ʧ��
			}
		break;          						//���洫����1����ߣ���Сֵ
		case EPC2: 
			*pSensorValue	=	(SensorR_value|(*pHMatNum<<12))|0x8000;  //���λΪ1����ʾEPC2
			if((MatCal.t_SensorR_valueH-MatCal.t_SensorR_valueL>125))   
			{
				if(MatCal.CaliFlag == 1)   
				{
					*MatCal.MatValue[*pHMatNum].PSensorR_H = MatCal.t_SensorR_valueH;
					*MatCal.MatValue[*pHMatNum].PSensorR_L = MatCal.t_SensorR_valueL;
					*MatCal.MatValue[*pHMatNum].PEPC12 = *MatCal.MatValue[*pHMatNum].PEPC12|1;
					WriteSensor(); 
					MatCal.CaliFlag = 0;        //У׼��ɱ�־
					return 0 ;
				}
			}else if((MatCal.t_SensorR_valueH-MatCal.t_SensorR_valueL<-125)) {
				if(MatCal.CaliFlag == 1)   
				{
					*MatCal.MatValue[*pHMatNum].PSensorR_H = MatCal.t_SensorR_valueL;
					*MatCal.MatValue[*pHMatNum].PSensorR_L = MatCal.t_SensorR_valueH;
					*MatCal.MatValue[*pHMatNum].PEPC12 = *MatCal.MatValue[*pHMatNum].PEPC12|1;
					WriteSensor(); 	
					MatCal.CaliFlag = 0;        //У׼��ɱ�־
					return 0 ;
				}
			}else{
				return 1 ;
			}			    
		break;          //���洫����2���ұߣ���Сֵ
		default :         return 1 ;             
	}
}

/*****************���ݻ�ԭ********************/
void BackupRestore(void)        //ÿ��50ms����һ��
{
	if(*pHSysYes == SBackup||*pHSysYes == SRestore||*pHSysYes == SFactorySet)   		//ϵͳ����
	{
		gWorkMode = 0;
		delay_ms(1000);
		
		ParaStorage.fun[*pHSysYes]();	
		*pHSysYes = 255;
	}
}

/*****************��������********************/
void AdjustmentInterface(u16 control_id,u16 *data,u8 StepData,u8 state)
{
	static u8 Key_time_number=0;
	static u8 key_sun_bit=0;
	
	if(state == 1)      //����״̬����ʼ��ʱ
	{
		switch(control_id)
		{
			case 1:  
				if(key_sun_bit==0)      //�̰�ִֻ��һ��
				{
					key_sun_bit=1;
					(*data) += StepData;
				}else{
					Key_time_number<7?++Key_time_number:(*data+=StepData);    //����700ms������ټӼ�
				}
			break;
				
			case 2:  
				if(key_sun_bit==0)
				{
					key_sun_bit=1;
					(*data)-=StepData;
				}else{
					Key_time_number<7?++Key_time_number:(*data-=StepData);
				}
			break;
			default:                break;
		}
	}else if(state == 0){
		key_sun_bit=0;
		Key_time_number=0;
	}
}

void AddSub(void)  //�ṹ��ָ��ָ�򴫵ݹ����Ľṹ�������Ա
{
	static u8 TimeCnt;
	u8 state;
	ConfirmDataDef *PCData;
	
	state = *pHAddSub==0?0:1;   //*pHAddSub==0��ʾ�Ӽ���ťû�а���
	PCData = &ConfirmData;
	
	if(PCData->ExitFlag == 1)  //��ֹ�ϵ�ʱ�ݴ棬��*pHAdjustFlag��ʼ��Ϊ255
	{
		PCData->TempData = *PCData->AdjustData[*pHAdjustFlag].tData;    
		PCData->ExitFlag=0;
	}   //�ݴ�һ��ԭʼ����	
	
	if(TimeCnt>10)
	{
		if((*pHAddSub==1 && (*PCData->AdjustData[*pHAdjustFlag].tData+PCData->AdjustData[*pHAdjustFlag].StepData <= PCData->AdjustData[*pHAdjustFlag].UpperLimit)) || state==0)
		{
			AdjustmentInterface(*pHAddSub,PCData->AdjustData[*pHAdjustFlag].tData,PCData->AdjustData[*pHAdjustFlag].StepData,state);
		}
		else if((*pHAddSub==2 && (*PCData->AdjustData[*pHAdjustFlag].tData-PCData->AdjustData[*pHAdjustFlag].StepData >= PCData->AdjustData[*pHAdjustFlag].LowerLimit)) || state==0)
		{
			AdjustmentInterface(*pHAddSub,PCData->AdjustData[*pHAdjustFlag].tData,PCData->AdjustData[*pHAdjustFlag].StepData,state);
		}
		TimeCnt=0;
	}	
	
	if(*pHAdjustFlag==0||*pHAdjustFlag==24)
	{
		*pAdjustData = (*PCData->AdjustData[*pHAdjustFlag].tData)*10;
	}
	else if(*pHAdjustFlag==18||*pHAdjustFlag==22||*pHAdjustFlag==23)
	{
		*pAdjustData = *PCData->AdjustData[*pHAdjustFlag].tData+800;
	}
	else
	{
		*pAdjustData = (*PCData->AdjustData[*pHAdjustFlag].tData)*10+800;
	}
	
	if(*pHParaConfirm==2)   //ȡ������
	{
		*ConfirmData.AdjustData[*pHAdjustFlag].tData = ConfirmData.TempData;     //��ԭԭʼ����
		ConfirmData.ExitFlag = 1;
		*pHAdjustFlag = 255;
	}else if(*pHParaConfirm==1){
		ConfirmData.ExitFlag = 1;
		*pHAdjustFlag = 255;
	}
	TimeCnt++;
}




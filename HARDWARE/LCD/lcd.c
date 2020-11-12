#include <string.h>
#include "cmd_process.h"
#include "delay.h"
#include "lcdtest.h"
#include "main.h"
#include "stmflash.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

extern  s16 OcclusionL_rate,OcclusionR_rate;             //�������ڵ���
extern  u8  cmd_buffer[CMD_MAX_SIZE];  


u16     current_screen_id = 0;          //��ǰ����ID
u16     last_screen_id;             //��һ����ID����Ҫ���ڷ���
u16     BrightnessDis=100;              //��Ļ���ȵ��ڷ�Χ0~100
u16     Screen_brightness;      //��Ļ����
u16     t_Work_Mode;                //�ݴ湤��ģʽ
u8      PortNum;



LockTimeDef LockTime 		= {&gBrightness,{0xcc,0x99,0x66,0x33,0x00},&gLockTime,{5,60,300,50000}};
ParaStorageDef ParaStorage	= {SRestore,{Backup,Restore,ParameterInit}};
SensorModeDef SensorMode 	= {&gSensorMode,{"EPC1","EPC2","CPC","SPC"},{"������1/EPC1","������2/EPC2","������1+2/CPC","���ξ�ƫ/SPC"},SensorEPC1Dis,SensorEPC2Dis,SensorCPCDis,SensorSPCDis};
TravelCalDef TravelCal 		= {0,0,0,0,&gCaliTorque};						
PolarDef Polar         		= {&gAutoPolar,&gManuPolar,{"����","����","Forward","Reverse"},{"+","-"}};
PasswordSetDef PasswordSet  = {0,0,Password,{0},{InputPassword,ChangePassword,RepeatInput}};

MatCalDef MatCal = {            //����У׼
	Manu_ID,                    //Ĭ�Ϸ��ؽ���Ϊ�ֶ�����
	EPC1,   
	{
	{&g_Mat0_Sensor1_H,&g_Mat0_Sensor1_L,&g_Mat0_Sensor2_H,&g_Mat0_Sensor2_L,&Mat0EPC12},
	{&g_Mat1_Sensor1_H,&g_Mat1_Sensor1_L,&g_Mat1_Sensor2_H,&g_Mat1_Sensor2_L,&Mat1EPC12},
	{&g_Mat2_Sensor1_H,&g_Mat2_Sensor1_L,&g_Mat2_Sensor2_H,&g_Mat2_Sensor2_L,&Mat2EPC12},
	{&g_Mat3_Sensor1_H,&g_Mat3_Sensor1_L,&g_Mat3_Sensor2_H,&g_Mat3_Sensor2_L,&Mat3EPC12},
	{&g_Mat4_Sensor1_H,&g_Mat4_Sensor1_L,&g_Mat4_Sensor2_H,&g_Mat4_Sensor2_L,&Mat4EPC12},
	{&g_Mat5_Sensor1_H,&g_Mat5_Sensor1_L,&g_Mat5_Sensor2_H,&g_Mat5_Sensor2_L,&Mat5EPC12},
	{&g_Mat6_Sensor1_H,&g_Mat6_Sensor1_L,&g_Mat6_Sensor2_H,&g_Mat6_Sensor2_L,&Mat6EPC12},
	{&g_Mat7_Sensor1_H,&g_Mat7_Sensor1_L,&g_Mat7_Sensor2_H,&g_Mat7_Sensor2_L,&Mat7EPC12},
	},
	&gCurMatNum,       //���ϱ��Ĭ��0
	0,0,0,0,0
};

DelayInfaceGroupDef DelayInfaceGroup = {
	0,
	{{0,0,Shoutcut_ID},
	{0,0,Shoutcut_ID},
	{0,0,Material_SwitchID},
	{0,0,Travel_SetID},
	{0,0,Shoutcut_ID}}
};

ConfirmDataDef ConfirmData = {
	d_Gain_Data,1,0,
	{
	{1,100,&gGainData,1},
	{0,80,&gDeadZone,1},
	{100,3000,&gAutoSpeed,100},
	{100,3000,&gManuSpeed,100},
	{100,3000,&gCentSpeed,100},
	{0,160,&gFineTune,1},
	{5,95,&gBehindLimit,1},
	{5,95,&gCenterLimit,1},
	{5,95,&gFrontLimit,1},
	{5,95,&gNoDetectValve,1},
	{1,100,&gNoDetectTime,1},
	{1,100,&gNoWaitTime,1},
	{2,250,&gSPCStopTime,1},
	{10,90,&gSPCBehindLimit,1},
	{10,90,&gSPCFrontLimit,1},
	{5,100,&gAFlexAcc,1},
	{5,100,&gAFlexDec,1},
	{5,100,&gMFlexAcc,1},
	{5,100,&gMFlexDec,1},
	{1,100,&BrightnessDis,1},
	{2,10,&gFuncTorque,1}
	},
	NULL
};

void Adjustment_interface(u16 screen_id,u16 control_id,u16 *data,u8 StepData,u8 state);


/****************��ʼ����ʾ*******************/
void InitLDis()
{
	/****��ʾ�Ӽ����ı�������****/
	BatchBegin(AccDec_Rec_P_SetID);    //��ʼ��������
	BatchSetFrame(7, gAFlexSacc+gLanguageChoice*2);
	BatchSetFrame(11,gAFlexSdec+gLanguageChoice*2);
	BatchSetFrame(21,gMFlexSacc+gLanguageChoice*2);
	BatchSetFrame(28,gMFlexSdec+gLanguageChoice*2);
	BatchEnd();
	
	/****��ʾ��λģʽ�ı���****/
	AnimationPlayFrame(Limit_Rec_P_SetID,17,gLimitMode+gLanguageChoice*3);  
	
	/****��ʾSPCģʽ�ı�������****/
	AnimationPlayFrame(SPC_Rec_P_SetID,16,gSPCMode+gLanguageChoice*2); 
	
	/****��ʾ����ʹ���ı�������****/
	AnimationPlayFrame(NO_Rec_P_SetID,9,gNoWaitEN+gLanguageChoice*2); 
	
	/****��ʾ�����ı�������****/
	AnimationPlayFrame(PolarityID,3,(*Polar.PPolarM)+gLanguageChoice*2); 
	AnimationPlayFrame(PolarityID,4,(*Polar.PPolarA)+gLanguageChoice*2); 
	
	/****��ʾ��������ı�������****/
	AnimationPlayFrame(Motor_P_SetID,6,gMotorType+gLanguageChoice*2); 
	
	/****��ʾ����״̬�ı�������****/
	AnimationPlayFrame(Power_on_StateID,2,gPowerOnMode+gLanguageChoice*4); 
	
	/****��ʾ�˿ڶ����ı�������****/
	BatchBegin(Port_Sys_P_SetID);    //��ʼ��������
	BatchSetFrame(PortFun[0].DisNum,*PortFun[0].PPort+gLanguageChoice*11);
	BatchSetFrame(PortFun[1].DisNum,*PortFun[1].PPort+gLanguageChoice*11);
	BatchSetFrame(PortFun[2].DisNum,*PortFun[2].PPort+gLanguageChoice*11);
	BatchSetFrame(PortFun[3].DisNum,*PortFun[3].PPort+gLanguageChoice*11);
	BatchEnd();
}

/****************��ʼ��LCD*******************/
void InitDIS(void)
{
	int cnt;
	/********����Ĭ�ϣ��Ӽ��ٲ��ܵ���********/
	SetControlVisiable(AccDec_Rec_P_SetID,8, gAFlexSacc);
	SetControlVisiable(AccDec_Rec_P_SetID,5, gAFlexSdec);
	SetControlVisiable(AccDec_Rec_P_SetID,19,gMFlexSacc);
	SetControlVisiable(AccDec_Rec_P_SetID,26,gMFlexSdec);

//	SetControlVisiable(Auto_ID,23,0);
	AnimationPlayFrame(RestoreBackupID,7,gBackupFlag);
	
	//��ʼ����ʾ�г�����Ӧ�������
	SetTextFloat(Travel_SetID,3,*TravelCal.CalCurrent/10.0,1,1); 
	
	//��Ļ���ȳ�ʼ��
	SetPowerSaving(1,0xff-(u8)LockTime.BrightnessArray[*LockTime.PBrightness],0x32,LockTime.LockTimeArray[*LockTime.PLockTime]);
	SetButtonValue(Brightness_Sys_P_SetID,*LockTime.PLockTime+12,1);
	SetBackLight(LockTime.BrightnessArray[*LockTime.PBrightness]);
	SetButtonValue(Brightness_Sys_P_SetID,*LockTime.PBrightness+3,1);
	
	//��������ʾ��ʼ��
	AnimationPlayFrame(Mode_Rec_P_SetID,2,*SensorMode.PSensorMode); 
	
	/****����ѡ����ʾ****/
	for(cnt=0;cnt<8;cnt++)
	{
		AnimationPlayFrame(MatValue_SwitchID,cnt+11,*MatCal.MatValue[cnt].PEPC12); 
	}
	SetTextInt32(MatValue_SwitchID,10,*MatCal.CurMatNum,0,1);
	SetTextInt32(Material_SwitchID,10,*MatCal.CurMatNum,0,1);
	AnimationPlayFrame(BuzzerSetID,4,1-gBuzzer);
	AnimationPlayFrame(BuzzerSetID,5,gBuzzer);
	
	if(gLanguageChoice!=0&&gLanguageChoice!=1)  gLanguageChoice=0;
	SetLanguage(gLanguageChoice,gLanguageChoice);	
	delay_ms(200);
	AnimationPlayFrame(LanguageSetID,4,gLanguageChoice);
	AnimationPlayFrame(LanguageSetID,5,1-gLanguageChoice);
	
	InitLDis();
}

void InitLCD(void)
{
	//�ϵ����س�������
	SetControlVisiable(Auto_ID,50,0);
	SetControlVisiable(Auto_ID,51,0);
	SetControlVisiable(Auto_ID,52,0);
	SetControlVisiable(Manu_ID,50,0);
	SetControlVisiable(Manu_ID,51,0);
	SetControlVisiable(Manu_ID,52,0);
	SetControlVisiable(Center_ID,50,0);
	SetControlVisiable(Center_ID,51,0);
	SetControlVisiable(Center_ID,52,0);
	InitDIS();
}

/***********�Զ����У��ֶ����У����Ķ�Ӧ����ʾ����**************/
void AutoDis()
{
	float Dis_Pulse_Num;
	Dis_Pulse_Num = (100.0*gCurrentPulseNum/gMAXPulseNum);
	gWorkMode = 1;
	SensorMode.fun[*SensorMode.PSensorMode]();
	SetProgressValue(Auto_ID,29,Dis_Pulse_Num);        //���ý�������ֵ1000*(1.0*gCurrentPulseNum/gMAXPulseNum)
	
	SetTextInt32(Auto_ID,12,gGainData,0,1); 
	SetTextInt32(Auto_ID,14,gCurMatNum,0,1); 
	SetTextInt32(Auto_ID,16,(s8)(gFineTune-80),1,1);
	SetTextValue(Auto_ID,18,SensorMode.DisStr[*SensorMode.PSensorMode]);
	SetTextValue(Auto_ID,20,Polar.DisStrSymbol[*Polar.PPolarA]);
	SetTextFloat(Auto_ID,30,Dis_Pulse_Num,1,1); 
	
#ifdef  _TEST_
	SetTextInt32(current_screen_id,60,g_ADC_Buf[0]*8,0,1);              //������ʾ
	SetTextInt32(current_screen_id,61,gCurrentPulseNum,0,1);         //��ʾ������
#endif
}

void ManuDis()
{
	float Dis_Pulse_Num;
	Dis_Pulse_Num = (100.0*gCurrentPulseNum/gMAXPulseNum);
				
	gWorkMode = 0;
	MatCal.BackInf = Manu_ID;
	TravelCal.BackInf = Manu_ID;
	SensorMode.fun[*SensorMode.PSensorMode]();
	SetProgressValue(Manu_ID,29,Dis_Pulse_Num);        //���ý�������ֵ1000*(1.0*gCurrentPulseNum/gMAXPulseNum)

	SetTextInt32(Manu_ID,12,gGainData,0,1); 
	SetTextInt32(Manu_ID,14,gCurMatNum,0,1); 
	SetTextInt32(Manu_ID,16,(s8)(gFineTune-80),1,1);
	SetTextValue(Manu_ID,18,SensorMode.DisStr[*SensorMode.PSensorMode]);
	SetTextValue(Manu_ID,20,Polar.DisStrSymbol[*Polar.PPolarA]);
	SetTextFloat(Manu_ID,30,Dis_Pulse_Num,1,1); 
	
#ifdef  _TEST_
SetTextInt32(current_screen_id,60,g_ADC_Buf[0]*8,0,1);              //������ʾ
SetTextInt32(current_screen_id,61,gCurrentPulseNum,0,1);         //��ʾ������
#endif
}

void CenterDis()
{
	float Dis_Pulse_Num;
	Dis_Pulse_Num = (100.0*gCurrentPulseNum/gMAXPulseNum);
	gWorkMode = 2;
	SetProgressValue(Center_ID,29,Dis_Pulse_Num);        //���ý�������ֵ1000*(1.0*gCurrentPulseNum/gMAXPulseNum)
	
	SetTextInt32(Center_ID,12,gGainData,0,1); 
	SetTextInt32(Center_ID,14,gCurMatNum,0,1); 
	SetTextInt32(Center_ID,16,(s8)(gFineTune-80),1,1);
	SetTextValue(Center_ID,18,SensorMode.DisStr[*SensorMode.PSensorMode]);
	SetTextValue(Center_ID,20,Polar.DisStrSymbol[*Polar.PPolarA]);
	SetTextInt32(Center_ID,22,gCenterLimit,0,1);
	SetTextFloat(Center_ID,30,Dis_Pulse_Num,1,1); 
#ifdef  _TEST_
SetTextInt32(current_screen_id,60,g_ADC_Buf[0]*8,0,1);              //������ʾ
SetTextInt32(current_screen_id,61,gCurrentPulseNum,0,1);         //��ʾ������
#endif
}

/************4�ִ�����ģʽ��Ӧ����ʾ����***************/
void SensorEPC1Dis()    //EPC1
{
	if(OcclusionL_rate>=0)
	{
		SetProgressValue(current_screen_id,22,OcclusionL_rate);  
		SetProgressValue(current_screen_id,21,0); 
	}
	else
	{
		SetProgressValue(current_screen_id,22,0);  
		SetProgressValue(current_screen_id,21,OcclusionL_rate);  
	}
	SetTextInt32(current_screen_id,24,OcclusionL_rate,1,1); 
}

void SensorEPC2Dis()    //EPC2
{
	if(OcclusionR_rate>=0)
	{
		SetProgressValue(current_screen_id,22,OcclusionR_rate);  
		SetProgressValue(current_screen_id,21,0); 
	}
	else
	{
		SetProgressValue(current_screen_id,22,0);  
		SetProgressValue(current_screen_id,21,OcclusionR_rate);  
	}
	SetTextInt32(current_screen_id,24,OcclusionR_rate,1,1); 
}

void SensorCPCDis()   //CPC
{
	s16 DisRate;
	DisRate = (OcclusionR_rate-OcclusionL_rate)>>1;
	if(OcclusionR_rate-OcclusionL_rate>=0)
	{
		SetProgressValue(current_screen_id,22,DisRate);  
		SetProgressValue(current_screen_id,21,0); 
	}
	else
	{
		SetProgressValue(current_screen_id,22,0);  
		SetProgressValue(current_screen_id,21,DisRate);  
	}
	SetTextInt32(current_screen_id,24,DisRate,1,1); 
}

void SensorSPCDis()   //SPC
{
	if(gSPCMode == 0)    //0���ڲ�������  1���ⲿ������
	{
		SetProgressValue(current_screen_id,21,-OcclusionL_rate/2-50);  
		SetProgressValue(current_screen_id,22,OcclusionR_rate/2+50);  
		OcclusionL_rate>OcclusionR_rate?SetTextInt32(current_screen_id,24,OcclusionL_rate/2+50,1,1):SetTextInt32(current_screen_id,24,OcclusionR_rate/2+50,1,1); 
	}
	else
	{
		SetProgressValue(current_screen_id,21,-OcclusionL_rate/2-50);  
		SetProgressValue(current_screen_id,22,OcclusionR_rate/2+50);  
		OcclusionL_rate>OcclusionR_rate?SetTextInt32(current_screen_id,24,OcclusionL_rate/2+50,1,1):SetTextInt32(current_screen_id,24,OcclusionR_rate/2+50,1,1); 
	}

}

/**************������У׼������ʾ����*******************/
void NoShelterDis()
{
	switch(MatCal.EPCNum)
	{
		case EPC1:    
			MatCal.t_Sensor1_valueH = SensorL_value;
//			*MatCal.MatValue[*MatCal.CurMatNum].PSensor1_H = SensorL_value;                     //���洫����1����ߣ����ֵ
			SetTextInt32(current_screen_id,1,SensorL_value,0,1);        //���µ����������ı���
			SetTextValue(current_screen_id,3,"EPC1");                    //���µ����������ı���
		break;          
		case EPC2:    
			MatCal.t_Sensor2_valueH = SensorR_value;
//			*MatCal.MatValue[*MatCal.CurMatNum].PSensor2_H = SensorR_value;       //���洫����2���ұߣ����ֵ
			SetTextInt32(current_screen_id,1,SensorR_value,0,1);        //���µ����������ı���
			SetTextValue(current_screen_id,3,"EPC2");                    //���µ����������ı���
		break;          
		default :                                break;
	}
}

void ShelterDis()
{
	switch(MatCal.EPCNum)
	{
		case EPC1:
			MatCal.t_Sensor1_valueL = SensorL_value;			
//			*MatCal.MatValue[*MatCal.CurMatNum].PSensor1_L = SensorL_value;       //���洫����1����ߣ���Сֵ
			SetTextInt32(current_screen_id,1,SensorL_value,0,1);        //���µ����������ı���
			SetTextValue(current_screen_id,3,"EPC1");                    //���µ����������ı���
		break;          
		case EPC2:    
			MatCal.t_Sensor2_valueL = SensorR_value;
//			*MatCal.MatValue[*MatCal.CurMatNum].PSensor2_L = SensorR_value;       //���洫����2���ұߣ���Сֵ
			SetTextInt32(current_screen_id,1,SensorR_value,0,1);        //���µ����������ı���
			SetTextValue(current_screen_id,3,"EPC2");                    //���µ����������ı���
		break;          
		default :                                break;
	}
}

void RealTimeDis()
{
	switch(MatCal.EPCNum)
	{
		case EPC1: 
			SetTextValue(current_screen_id,5,"EPC1");
			SetTextInt32(current_screen_id,4,OcclusionL_rate,1,0);        //���µ����������ı���
			SetProgressValue(current_screen_id,2,OcclusionL_rate);                       //���ý�������ֵ
			if((MatCal.t_Sensor1_valueH-MatCal.t_Sensor1_valueL>125))   
			{
				*MatCal.MatValue[*MatCal.CurMatNum].PSensor1_H = MatCal.t_Sensor1_valueH;
				*MatCal.MatValue[*MatCal.CurMatNum].PSensor1_L = MatCal.t_Sensor1_valueL;
				AnimationPlayFrame(Sensor_Calibr4ID,8,0+gLanguageChoice*2);  //��ʾ������ʾ��Ӣ��
				WriteSensor();
			}
			else if((MatCal.t_Sensor1_valueH-MatCal.t_Sensor1_valueL<-125)) 
			{
				*MatCal.MatValue[*MatCal.CurMatNum].PSensor1_H = MatCal.t_Sensor1_valueL;
				*MatCal.MatValue[*MatCal.CurMatNum].PSensor1_L = MatCal.t_Sensor1_valueH;
				AnimationPlayFrame(Sensor_Calibr4ID,8,0+gLanguageChoice*2);  //��ʾ������ʾ��Ӣ��
				WriteSensor();
			}		
			else
			{
				AnimationPlayFrame(Sensor_Calibr4ID,8,1+gLanguageChoice*2);  //��ʾ������ʾ��Ӣ��
			}
		break;          //���洫����1����ߣ���Сֵ
		case EPC2: 
			SetTextValue(current_screen_id,5,"EPC2");				
			SetTextInt32(current_screen_id,4,OcclusionR_rate,1,0);        //���µ����������ı���
			SetProgressValue(current_screen_id,2,OcclusionR_rate); 
			if((MatCal.t_Sensor2_valueH-MatCal.t_Sensor2_valueL>125))   
			{
				*MatCal.MatValue[*MatCal.CurMatNum].PSensor2_H = MatCal.t_Sensor2_valueH;
				*MatCal.MatValue[*MatCal.CurMatNum].PSensor2_L = MatCal.t_Sensor2_valueL;
				AnimationPlayFrame(Sensor_Calibr4ID,8,0+gLanguageChoice*2);  //��ʾУ׼�ɹ���Ӣ��
				WriteSensor();
			}
			else if((MatCal.t_Sensor2_valueH-MatCal.t_Sensor2_valueL<-125)) 
			{
				*MatCal.MatValue[*MatCal.CurMatNum].PSensor2_H = MatCal.t_Sensor2_valueL;
				*MatCal.MatValue[*MatCal.CurMatNum].PSensor2_L = MatCal.t_Sensor2_valueH;
				AnimationPlayFrame(Sensor_Calibr4ID,8,0+gLanguageChoice*2);  //��ʾУ׼�ɹ���Ӣ��
				WriteSensor();
			}		
			else
			{
				AnimationPlayFrame(Sensor_Calibr4ID,8,1+gLanguageChoice*2);  //��ʾУ׼ʧ����Ӣ��
			}			    
		break;          //���洫����2���ұߣ���Сֵ
		default :                                break;
	}
}

/*******����������غ���*****/
void InputPassword(u8 *str)
{	
	if((memcmp(Password,str,6)==0)||(memcmp(SysPassword,str,6)==0))
	{
		SetScreen(Parm_Set_ID);
		AnimationPlayFrame(Password_ProtectionID,18,0);    //���ش�����ʾ
	}
	else
	{
		AnimationPlayFrame(Password_ProtectionID,18,PasswordSet.CallID+1+gLanguageChoice*2);  //��ʾ������ʾ��Ӣ��
	}
}

void ChangePassword(u8 *str)
{
	PasswordSet.ChangeFlag = 1;
	strcpy((char *)PasswordSet.t_Password,(const char *)str); //�ݴ�����,����ָ��ָ��ͬһ�������ռ�
	SetScreen(Password_SetID);
}

void RepeatInput(u8 *str)
{
	if((memcmp(PasswordSet.t_Password ,str,6)==0)&& PasswordSet.ChangeFlag==1)//�ж��ٴ�����������Ƿ���ͬ
	{
		PasswordSet.ChangeFlag = 2;
		SetScreen(Password_SetID);
	}
	else
	{
		AnimationPlayFrame(Password_ProtectionID,18,PasswordSet.CallID+gLanguageChoice*2); //��ʾ�������벻ͬ
	}
}

void anjiancl(u16 screen_id, u16 control_id, u8  state, ConfirmDataDef *PCData, enum AdjustDataType AData, u16 BackInf)  //�ṹ��ָ��ָ�򴫵ݹ����Ľṹ�������Ա
{
	PCData->MenuCount    = AData;
	PCData->BackInface   = BackInf;
	if(control_id==2&&((*PCData->AdjustData[AData].tData+PCData->AdjustData[AData].StepData <= PCData->AdjustData[AData].UpperLimit) || state==0))
	{
		Adjustment_interface(screen_id,control_id,PCData->AdjustData[AData].tData,PCData->AdjustData[AData].StepData,state);
	}
	else if(control_id==3&&((*PCData->AdjustData[AData].tData-PCData->AdjustData[AData].StepData >= PCData->AdjustData[AData].LowerLimit) || state==0))
	{
		Adjustment_interface(screen_id,control_id,PCData->AdjustData[AData].tData,PCData->AdjustData[AData].StepData,state);
	}
}

void Confirm(u16 control_id, ConfirmDataDef *PCData)
{
	if(control_id==4)   //ȡ������
	{
		*PCData->AdjustData[PCData->MenuCount].tData=ConfirmData.TempData;     //��ԭԭʼ����
	}
	ConfirmData.ExitFlag=1;
	SetScreen(PCData->BackInface);
}


/*! 
*  \brief  ��Ϣ��������
*  \param msg ��������Ϣ
*  \param size ��Ϣ����
*/
void ProcessMessage(PCTRL_MSG msg, u16 size )
{
    u8 cmd_type = msg->cmd_type;                                                  //ָ������
    u8 ctrl_msg = msg->ctrl_msg;                                                  //��Ϣ������
    u8 control_type = msg->control_type;                                          //�ؼ�����
    u16 screen_id = PTR2U16(&msg->screen_id);                                     //����ID   16λ����
    u16 control_id = PTR2U16(&msg->control_id);                                   //�ؼ�ID   16λ����
    u32 value = PTR2U32(msg->param);                                              //��ֵ

    switch(cmd_type)
    {  
		case NOTIFY_TOUCH_PRESS:                                                        //����������
		case NOTIFY_TOUCH_RELEASE:                                                      //�������ɿ�
			NotifyTouchXY(cmd_buffer[1],PTR2U16(cmd_buffer+2),PTR2U16(cmd_buffer+4)); 
			break;                                                                    
		case NOTIFY_WRITE_FLASH_OK:                                                     //дFLASH�ɹ�
			NotifyWriteFlash(1);                                                      
			break;                                                                    
		case NOTIFY_WRITE_FLASH_FAILD:                                                  //дFLASHʧ��
			NotifyWriteFlash(0);                                                      
			break;                                                                    
		case NOTIFY_READ_FLASH_OK:                                                      //��ȡFLASH�ɹ�
			NotifyReadFlash(1,cmd_buffer+2,size-6);                                     //ȥ��֡ͷ֡β
			break;                                                                    
		case NOTIFY_READ_FLASH_FAILD:                                                   //��ȡFLASHʧ��
			NotifyReadFlash(0,0,0);                                                   
			break;                                                                    
		case NOTIFY_READ_RTC:                                                           //��ȡRTCʱ��
			NotifyReadRTC(cmd_buffer[2],cmd_buffer[3],cmd_buffer[4],cmd_buffer[5],cmd_buffer[6],cmd_buffer[7],cmd_buffer[8]);
			break;
		case NOTIFY_CONTROL:                                                            //�ؼ�����֪ͨ
		{
			if(ctrl_msg==MSG_GET_CURRENT_SCREEN)                                    //����ID�仯֪ͨ
			{
				NotifyScreen(screen_id);                                            //�����л������ĺ���
			}
			else 
			{
				switch(control_type)
				{
					case kCtrlButton:                                                   //��ť�ؼ�
						NotifyButton(screen_id,control_id,msg->param[1]);                 
						break;                                                             
					case kCtrlText:                                                     //�ı��ؼ�
						NotifyText(screen_id,control_id,msg->param);                       
						break;                                                             
					case kCtrlProgress:                                                 //�������ؼ�
						NotifyProgress(screen_id,control_id,value);                     //������Ļ�������ؼ��ϴ���value�������ı����ֵ
						break;                                                             
					case kCtrlSlider:                                                   //�������ؼ�
						NotifySlider(screen_id,control_id,value);                          
						break;                                                             
					case kCtrlMeter:                                                    //�Ǳ�ؼ�
						NotifyMeter(screen_id,control_id,value);                           
						break;                                                             
					case kCtrlMenu:                                                     //�˵��ؼ�
						NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);      
						break;                                                              
					case kCtrlSelector:                                                 //ѡ��ؼ�
						NotifySelector(screen_id,control_id,msg->param[0]);                
						break;                                                              
					case kCtrlRTC:                                                      //����ʱ�ؼ�
						NotifyTimer(screen_id,control_id);
						break;
					default:
						break;
				}
			} 
			break;  
		} 
		case NOTIFY_HandShake:                                                          //����֪ͨ                                                     
			NOTIFYHandShake();
			break;
		default:
			break;
    }
}
/*! 
*  \brief  ����֪ͨ
*/
void NOTIFYHandShake()
{
   SetButtonValue(3,2,1);
}

/*! 
*  \brief  �����л�֪ͨ
*  \details  ��ǰ����ı�ʱ(�����GetScreen)��ִ�д˺���
*  \param screen_id ��ǰ����ID
*/
void NotifyScreen(u16 screen_id)                                     //�����л������ĺ���
{
    //TODO: ����û�����
    current_screen_id = screen_id;    //�ڹ��������п��������л�֪ͨ����¼��ǰ����ID
    //�����Զ����棬����ID=0
	switch(current_screen_id)
	{
		case Boot_InterfaceID:
			switch(gWorkMode)
			{
				case 0:    SetScreen(Manu_ID);     break;
				case 1:    SetScreen(Auto_ID);     break;
				case 2:    SetScreen(Center_ID);   break;
				default :  break;
			}
			InitLCD();
		break;
		case Company_Information:               break;
			
		/*�Զ��˵�*/
		case Auto_ID:  			    	  AutoDis();         break;
		case Polar_A_AdjustID:      	    		         break;
		case Gain_A_AdjustID:  			 
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gGainData;      ConfirmData.ExitFlag=0;}      //�ݴ�һ��ԭʼ����
			SetTextInt32(current_screen_id,5,gGainData,0,1);              
		break;
		case Offset_A_AdjustID:  		 
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gFineTune;     ConfirmData.ExitFlag=0;}     //�ݴ�һ��ԭʼ����
			SetTextInt32(current_screen_id,5,(s8)(gFineTune-80),1,1);   
		break;
		
		/*�ֶ��˵�*/
		case Manu_ID:               	 ManuDis();          break;

		/*���Ĳ˵�*/
		case Center_ID:               	 CenterDis();        break;
		case Center_AdjustID:
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gCenterLimit;     ConfirmData.ExitFlag=0;}  //�ݴ�һ��ԭʼ����				
			SetTextInt32(current_screen_id,5,gCenterLimit,0,1);   
		break;
		/*��ݲ˵�*/
		case Shoutcut_ID:  			         break;
		/*��ݲ˵�    ��*/
		case Wizard_ID:  			     TravelCal.BackInf = Wizard_ID; break;
		case Manu_W_ID:  			     gWorkMode = 0;   break;
		case Sensor_W_SetID:  			 MatCal.BackInf = Sensor_W_SetID;        break;
		case Test_Manu_W_ID:  			 gWorkMode = 0;   break;
		case Test_Auto_W_ID:  			 gWorkMode = 1;   break;
		case Polar_W_AdjustID:  		                    break;
		case Gain_W_AdjustID:    
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gGainData;  ConfirmData.ExitFlag=0;}     //�ݴ�һ��ԭʼ����
			SetTextInt32(current_screen_id,5,gGainData,0,1);    
		break;
		
		/*��ݲ˵�    ��������*/
		case Parm_Set_ID:  			         break;
		/*��ݲ˵�    ��������    ��ƫ��������*/
		case Rec_P_SetID:  			         break;
		
		case Mode_Rec_P_SetID:           AnimationPlayFrame(Mode_Rec_P_SetID,2,*SensorMode.PSensorMode);     break;                //��ƫģʽ
		
		case Gain_Rec_P_SetID:  		 SetTextInt32(current_screen_id,4,gGainData,0,1);//��ƫ����
										 SetTextInt32(current_screen_id,5,gDeadZone,0,0);               break;   
										 
		case Gain_Rec_P_AdjustID:  	
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gGainData;    ConfirmData.ExitFlag=0;}   //�ݴ�һ��ԭʼ����
			SetTextInt32(current_screen_id,5,gGainData,0,1);               
		break;
		case Dead_Rec_P_AdjustID:
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gDeadZone;  ConfirmData.ExitFlag=0;}     //�ݴ�һ��ԭʼ����			
			SetTextInt32(current_screen_id,5,gDeadZone,0,0);               
		break;
		
		case Speed_Rec_P_SetID:          SetTextInt32(current_screen_id,4,gManuSpeed,0,1);     //��ƫ�ٶ� 
										 SetTextInt32(current_screen_id,5,gAutoSpeed,0,1);       
										 SetTextInt32(current_screen_id,6,gCentSpeed,0,1);            break;
		case MSpeed_Rec_P_AdjustID:  
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gManuSpeed;  ConfirmData.ExitFlag=0;}     //�ݴ�һ��ԭʼ����			
			SetTextInt32(current_screen_id,5,gManuSpeed,0,0);            
		break;
		case ASpeed_Rec_P_AdjustID: 
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gAutoSpeed;     ConfirmData.ExitFlag=0;}    //�ݴ�һ��ԭʼ����			
			SetTextInt32(current_screen_id,5,gAutoSpeed,0,0);            
		break;
		case CSpeed_Rec_P_AdjustID: 
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gCentSpeed;     ConfirmData.ExitFlag=0;}   //�ݴ�һ��ԭʼ����			
			SetTextInt32(current_screen_id,5,gCentSpeed,0,0);            
		break;
		
		case AccDec_Rec_P_SetID:  		 SetTextInt32(current_screen_id,3,gAFlexAcc,0,1);        //���µ����������ı���
										 SetTextInt32(current_screen_id,4,gAFlexDec,0,1);        //��ƫ�Ӽ���
										 SetTextInt32(current_screen_id,18,gMFlexAcc,0,1);        //���µ����������ı���
										 SetTextInt32(current_screen_id,25,gMFlexDec,0,1);    break;      //��ƫ�Ӽ���
		case AAccSpeed_Rec_P_AdjustID:  
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gAFlexAcc;      ConfirmData.ExitFlag=0;}    //�ݴ�һ��ԭʼ����	
			SetTextInt32(current_screen_id,5,gAFlexAcc,0,0);     
		break;
		case ADecSpeed_Rec_P_AdjustID: 
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gAFlexDec;      ConfirmData.ExitFlag=0;}   //�ݴ�һ��ԭʼ����				
			SetTextInt32(current_screen_id,5,gAFlexDec,0,0);     
		break;
		case MAccSpeed_Rec_P_AdjustID:  
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gMFlexAcc;      ConfirmData.ExitFlag=0;}  //�ݴ�һ��ԭʼ����				
			SetTextInt32(current_screen_id,5,gMFlexAcc,0,0);     
		break;
		case MDecSpeed_Rec_P_AdjustID:   
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gMFlexDec;      ConfirmData.ExitFlag=0;} //�ݴ�һ��ԭʼ����	
			SetTextInt32(current_screen_id,5,gMFlexDec,0,0);     
		break;
		
		case Current_Rec_P_SetID:
			SetTextInt32(current_screen_id,2,gFuncTorque,0,0); 			
		break;      //��ƫ����
		case Current_Rec_P_AdjustID:     
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gFuncTorque;     ConfirmData.ExitFlag=0;}  //�ݴ�һ��ԭʼ����				
			SetTextInt32(current_screen_id,5,gFuncTorque,0,0);
		break;      //��ƫ�������ڽ���
		case Limit_Rec_P_SetID:  		 SetTextInt32(current_screen_id,5,gBehindLimit,0,1);        //���µ����������ı���
										 SetTextInt32(current_screen_id,6,gCenterLimit,0,1);        //���µ����������ı���
										 SetTextInt32(current_screen_id,9,gFrontLimit,0,1);       break;      //��ƫSPC
		case Behind_Rec_P_SetID: 
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gBehindLimit;     ConfirmData.ExitFlag=0;}  //�ݴ�һ��ԭʼ����				
			SetTextInt32(current_screen_id,5,gBehindLimit,0,1);      
		break;
		case Center_Rec_P_SetID: 
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gCenterLimit;     ConfirmData.ExitFlag=0;}  //�ݴ�һ��ԭʼ����				
			SetTextInt32(current_screen_id,5,gCenterLimit,0,1);      
		break;
		case Front_Rec_P_SetID:
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gFrontLimit;      ConfirmData.ExitFlag=0;} //�ݴ�һ��ԭʼ����				
			SetTextInt32(current_screen_id,5,gFrontLimit,0,1);       
		break;
		
		case SPC_Rec_P_SetID:  			 SetTextFloat(current_screen_id,4,gSPCStopTime/10.0,1,1);        //���µ����������ı���
										 SetTextInt32(current_screen_id,5,gSPCBehindLimit,0,1);        //���µ����������ı���
										 SetTextInt32(current_screen_id,6,gSPCFrontLimit,0,1);       break;      //��ƫSPC
		case SPCStop_Rec_P_AdjustID:  
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gSPCStopTime;    ConfirmData.ExitFlag=0;}   //�ݴ�һ��ԭʼ����					
			SetTextFloat(current_screen_id,5,gSPCStopTime/10.0,1,1);    
		break;
		case SPCBehind_Rec_P_AdjustID:  
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gSPCBehindLimit;    ConfirmData.ExitFlag=0;}   //�ݴ�һ��ԭʼ����					
			SetTextInt32(current_screen_id,5,gSPCBehindLimit,0,1);      
		break;
		case SPCFront_Rec_P_AdjustID:  
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gSPCFrontLimit;      ConfirmData.ExitFlag=0;} //�ݴ�һ��ԭʼ����					
			SetTextInt32(current_screen_id,5,gSPCFrontLimit,0,1);       
		break;
		
		case NO_Rec_P_SetID:  			 SetTextInt32(current_screen_id,12,gNoDetectValve,0,1);        //���µ����������ı���
										 SetTextFloat(current_screen_id,13,gNoDetectTime/10.0,1,1);        //���µ����������ı���
										 SetTextFloat(current_screen_id,14,gNoWaitTime/10.0,1,1);    break;      //��ƫ����
		case NOValue_Rec_P_AdjustID: 
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gNoDetectValve;     ConfirmData.ExitFlag=0;}  //�ݴ�һ��ԭʼ����			
			SetTextInt32(current_screen_id,5,gNoDetectValve,0,1);       
		break;
		case NODetect_Rec_P_AdjustID:  
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gNoDetectTime;      ConfirmData.ExitFlag=0;} //�ݴ�һ��ԭʼ����			
			SetTextFloat(current_screen_id,5,gNoDetectTime/10.0,1,1);   
		break;
		case NOWait_Rec_P_AdjustID: 
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gNoWaitTime;      ConfirmData.ExitFlag=0;} //�ݴ�һ��ԭʼ����			
			SetTextFloat(current_screen_id,5,gNoWaitTime/10.0,1,1);     
		break;
		
		case PolarityID:
			AnimationPlayFrame(PolarityID,4,(*Polar.PPolarA)+gLanguageChoice*2); 
		break;
		/*��ݲ˵�    ��������    ��������������*/
		case Sensor_P_SetID:  			   SetTextInt32(current_screen_id,8,(s8)(gFineTune-80),1,1);  MatCal.BackInf = Sensor_P_SetID;  break;
		case Offset_Sensor_P_AdjustID:   
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gFineTune;      ConfirmData.ExitFlag=0;} //�ݴ�һ��ԭʼ����	
			SetTextInt32(current_screen_id,5,(s8)(gFineTune-80),1,1);  
		break;
		/*��ݲ˵�    ��������    �ƶ�����������*/
		case Motor_P_SetID:  		TravelCal.BackInf = Motor_P_SetID;     break;
		/*��ݲ˵�    ��������    ϵͳ��������*/
		case Sys_P_SetID:  			         break;
			
		case Brightness_Sys_P_SetID:  	 SetTextInt32(current_screen_id,5,BrightnessDis,0,1);    break;	
		case BuzzerSetID:  		                              break;
		case Password_SetID:  		                              break;
		case LanguageSetID:  		                              break;
		case Power_on_StateID:                              break;
		case About_NativeID:  		                              break;	
		case RestoreBackupID:            					break;
		case SYSInitAffirmID:            					break;
		case SYSInitDisID:                                  break;
		/*��ݲ˵�    �˿ڶ���*/
		case Port_Sys_P_SetID:  		                              break;	
			
/********************�������***********************/		
		case Password_ProtectionID:  			         break;	
			
/*****************************ȷ�ϲ����޸�****************************/			
		case Confirm_ID:  		                              break;	
			
/*****************************������У׼4��****************************/
		case Material_SwitchID:		 SetTextInt32(current_screen_id,2,*MatCal.CurMatNum,0,0);  		break;
		case MatValue_SwitchID:		 
			
		break;
		case Sensor_Calibr1ID:      	               break;
		case Ask_to_SwitchID:				break;
		case Sensor_Calibr2ID:     	 NoShelterDis();   	    break;
		case Sensor_Calibr3ID:       ShelterDis();          break;//����������У׼���棬�ƶ����ϣ��鿴ƫ�����仯        ʵʱ��ȡ����������ֵ		
		case Sensor_Calibr4ID:       RealTimeDis(); 	    break;
			
/*****************************�г�У׼****************************/
		case Travel_SetID:  			
			switch(gWorkMode)
			{
				case 0: AnimationPlayFrame(current_screen_id,11,1);   AnimationPlayFrame(current_screen_id,12,0);  AnimationPlayFrame(current_screen_id,13,0);  break;
				case 1: AnimationPlayFrame(current_screen_id,11,0);   AnimationPlayFrame(current_screen_id,12,1);  AnimationPlayFrame(current_screen_id,13,0);  break;
				case 2: AnimationPlayFrame(current_screen_id,11,0);   AnimationPlayFrame(current_screen_id,12,0);  AnimationPlayFrame(current_screen_id,13,1);  break;
				default:   break;
			}
		break;
		case Travel_AdaptationID:
			SetTextFloat(current_screen_id,6,g_ADC_Buf[0]*0.008,1,1);
			if(TravelCal.CaliFlag==0)  	SetScreen(Travel_SetID);
		break;
		default:                                            break;
	}
	
}

/*! 
*  \brief  ���������¼���Ӧ
*  \param press 1���´�������3�ɿ�������
*  \param x x����
*  \param y y����
*/
void NotifyTouchXY(u8 press,u16 x,u16 y)
{ 
    //TODO: ����û�����
}

/*! 
*  \brief  ��������
*/ 
void UpdateUI()
{	
	u8 WarmFlag = 0,i;
	u8 HallData;
	static u8 Dis_Cnt=0;
	
	if(LongPortFun[PusherCenter]==5)		{SetScreen(Center_ID);gWorkMode=2;}
	if(LongPortFun[ManuDef]==4)				{SetScreen(Manu_ID);  gWorkMode=1;LongPortFun[ManuDef]=0;}
	else if(LongPortFun[ManuDef]==2) 		{SetScreen(Auto_ID);  gWorkMode=0;LongPortFun[ManuDef]=0;}
	if(LongPortFun[AutoDef]==2)				{SetScreen(Auto_ID);  gWorkMode=0;LongPortFun[AutoDef]=0;}
	else if(LongPortFun[AutoDef]==4) 		{SetScreen(Manu_ID);  gWorkMode=1;LongPortFun[AutoDef]=0;}
	if(LongPortFun[Switch1_EPC]==1)			{gSensorMode=1; LongPortFun[Switch1_EPC]=0xff;}
	else if(LongPortFun[Switch1_EPC]==0) 	{gSensorMode=0; LongPortFun[Switch1_EPC]=0xff;}
	if(LongPortFun[Switch2_EPC]==1)			{gSensorMode=1;  gAutoPolar=1; LongPortFun[Switch2_EPC]=0xff;}
	else if(LongPortFun[Switch2_EPC]==0) 	{gSensorMode=0;  gAutoPolar=0; LongPortFun[Switch2_EPC]=0xff;}
	
	if(Dis_Cnt%7 == 0)          //140ms
	{
		if(current_screen_id==Travel_AdaptationID) 
		{
			AnimationPlayFrame(Travel_AdaptationID,4,TravelCal.CaliStep+gLanguageChoice*4);  
		}
		
		/**********��ʾ����***************/
		HallData  = (u8)((GPIOC->IDR&0x000001c0)>>6);        //��ת��λ��    �ϳ�GPIOC8|GPIOC7|GPIOC6 
		if(HallData>6||HallData<1)   	Warm[NotConFlag] = NotConFlag;
		else 						   	Warm[NotConFlag] = NoErrFlag; 
		
		PDout(2) = (Warm[LimitFlag]==LimitFlag?1:0);        //��λ��򿪼̵���

		for(i=WarmNum-1;i>0;i--)
		{
			if(Warm[i] != 0)   {WarmFlag = Warm[i];  break;}
			else               {WarmFlag = 0;              }	
		}
		AnimationPlayFrame(current_screen_id,40,WarmFlag+gLanguageChoice*11);  

		/**********����״̬�򰴼����¼���ÿ��200ms��1***************/
		DelayInfaceGroup.DelayInface[DelayInfaceGroup.Num].DelayCnt = (DelayInfaceGroup.DelayInface[DelayInfaceGroup.Num].KeyDwon==1?++DelayInfaceGroup.DelayInface[DelayInfaceGroup.Num].DelayCnt:0);
		SetProgressValue(current_screen_id,51,DelayInfaceGroup.DelayInface[DelayInfaceGroup.Num].DelayCnt);  
		if(DelayInfaceGroup.DelayInface[DelayInfaceGroup.Num].DelayCnt == 10)  
		{
			SetScreen(DelayInfaceGroup.DelayInface[DelayInfaceGroup.Num].BackInface);      //����3s����ת��ָ������
		}
		
		/**********��ȡ��ǰ��ĻID***************/
		GetScreen();
		
		if(Dis_Cnt%21==0)
		{
			if(WarmFlag>2&&gBuzzer==1)  SetBuzzer(20);       //��λ����
		}
		if(Dis_Cnt==84)    //1.68s
		{
			Warm[ProhibitFlag] = 0;
			Dis_Cnt=0;
		}
	}	
	Dis_Cnt++;
}

/*! 
*  \brief  ��ť�ؼ�֪ͨ
*  \details  ����ť״̬�ı�(�����GetControlValue)ʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param state ��ť״̬��0����1����
*/
void NotifyButton(u16 screen_id, u16 control_id, u8  state)
{ 
	OS_CPU_SR  cpu_sr;
	static u16 GainBackInf=0;
	switch(screen_id)
	{	
		case Boot_InterfaceID:
			
		break;
		case Company_Information:
			if(control_id==2 && state==1)
			{
				gWorkMode==0?SetScreen(Manu_ID):(gWorkMode==1?SetScreen(Auto_ID):SetScreen(Center_ID));
			}
		break;
		/*�Զ��˵�*/
		case Auto_ID:                //�Զ�����	
			switch(control_id)
			{
				case 4:
					 Warm[ProhibitFlag] = ProhibitFlag;
				break;
				case 5: 
					SetTextValue(Polar_A_AdjustID,8,Polar.DisStr[(*Polar.PPolarA)+gLanguageChoice*2]);
				break;
				case 6: 
					GainBackInf = 0;
					SetTextInt32(Gain_Rec_P_SetID,4,gGainData,0,1);
					SetTextInt32(Gain_Rec_P_SetID,5,gDeadZone,0,0);
				break;
				case 7:
					SetTextInt32(Offset_A_AdjustID,5,(s8)(gFineTune-80),1,1);
				break;
				case 8:
					DelayInfaceGroup.Num = 0;
					DelayInfaceGroup.DelayInface[DelayInfaceGroup.Num].KeyDwon = (state==1?1:0);  
				break;

				default:                     break;
			}
		break;     
		case Polar_A_AdjustID:                //��������
			if(control_id == 2&&state == 1)
			{
				switch(*Polar.PPolarA)
				{
					case 0: (*Polar.PPolarA) = Reverse;    break;
					case 1: (*Polar.PPolarA) = Forward;    break;
					default :                  break;
				}	
				SetTextValue(Polar_A_AdjustID,8,Polar.DisStr[(*Polar.PPolarA)+gLanguageChoice*2]); 
				SetTextValue(Auto_ID,20,Polar.DisStrSymbol[*Polar.PPolarA]);   
				SetTextValue(Manu_ID,20,Polar.DisStrSymbol[*Polar.PPolarA]);
				AnimationPlayFrame(PolarityID,4,(*Polar.PPolarA)+gLanguageChoice*2); 
			}		
		break;       
			
		case Gain_A_AdjustID:              //�������
			anjiancl(screen_id, control_id, state,&ConfirmData,d_Gain_Data,Auto_ID);
		break;     
		case Offset_A_AdjustID:              //ƫ��������
			anjiancl(screen_id, control_id, state,&ConfirmData,d_Fine_Tuning,Auto_ID);
		break; 
		/*�ֶ��˵�*/
		case Manu_ID:             //�ֶ�����
			switch(control_id)
			{
				case 5:
					if(state == 1) ClickButton=2;      //�㶯��־��Ϊ2		
					if(state == 0) ClickButton=0; 
				break;
				case 7:
					if(state == 1) ClickButton=1;      //�㶯��־��Ϊ1  
					if(state == 0) ClickButton=0;                          
				break;
				case 8:
					DelayInfaceGroup.Num = 1;
					DelayInfaceGroup.DelayInface[DelayInfaceGroup.Num].KeyDwon = (state==1?1:0);  
				break;
				
				case 9:
					DelayInfaceGroup.Num = 2;
					DelayInfaceGroup.DelayInface[DelayInfaceGroup.Num].KeyDwon = (state==1?1:0);  
				break;
				case 10:
					DelayInfaceGroup.Num = 3;
					DelayInfaceGroup.DelayInface[DelayInfaceGroup.Num].KeyDwon = (state==1?1:0);                     
				break;
				default :        break;
			}
		break;				
		/*���Ĳ˵�*/
		case Center_ID:             //���Ľ���
			switch(control_id)
			{
				case 5:
					if(state == 1) ClickButton=2;      //�㶯��־��Ϊ2		
					if(state == 0) ClickButton=0; 
				break;
				case 7:
					if(state == 1) ClickButton=1;      //�㶯��־��Ϊ1  
					if(state == 0) ClickButton=0;                          
				break;
				case 8:
					DelayInfaceGroup.Num = 4;
					DelayInfaceGroup.DelayInface[DelayInfaceGroup.Num].KeyDwon = (state==1?1:0);  
				break;
				case 24:
					SetTextFloat(Center_AdjustID,5,gCenterLimit,0,1);
				break;
				default :        break;
			}
		break;	
		case Center_AdjustID:
			anjiancl(screen_id, control_id, state,&ConfirmData,d_Center_limit,Center_ID);
		break;
		
		/*��ݲ˵�*/
		case Shoutcut_ID:
			if(control_id == 2)   
			{
				PasswordSet.CallID = InputPasswordID;
				AnimationPlayFrame(Password_ProtectionID,17,PasswordSet.CallID+gLanguageChoice*3);
				AnimationPlayFrame(Password_ProtectionID,18,0); //��ʾ�޸�������Ӣ��
			}				
			else if(control_id==4)
			{
				if(state==1) {Warm[LockFlag] = LockFlag; }
				else          Warm[LockFlag] = NoErrFlag;
			}
			else if(control_id==9 && state==1)    
			{
				switch(gWorkMode)
				{
					case 0: gWorkMode=0;  SetScreen(Manu_ID);     break;       //�л����ֶ�����
					case 1: gWorkMode=1;  SetScreen(Auto_ID);     break;       //�л����Զ�����
					case 2: gWorkMode=2;  SetScreen(Center_ID);   break;       //�л������Ľ���
					default :                break;
				}	
			}			
		break;
		/*��ݲ˵�    ��*/
		case Wizard_ID:
		break;
	
		case Manu_W_ID:         //�ֶ�����  
			switch(control_id)
			{
				case 1:
					if(state == 1) ClickButton=2;      //�㶯��־��Ϊ2		
					if(state == 0) ClickButton=0; 
				break;
				case 2:
					if(state == 1) ClickButton=1;      //�㶯��־��Ϊ1  
					if(state == 0) ClickButton=0;                          
				break;
				default :         break;
			}
		break;
		case Sensor_W_SetID:       //����������
		break;
		case Test_Manu_W_ID:         
			switch(control_id)
			{
				case 3:
					if(state == 1) ClickButton=2;      //�㶯��־��Ϊ2		
					if(state == 0) ClickButton=0; 
				break;
				case 4:
					if(state == 1) ClickButton=1;      //�㶯��־��Ϊ1  
					if(state == 0) ClickButton=0;                          
				break;
				default :         break;
			}
		break;
		case Test_Auto_W_ID:  
			if(control_id == 4&&state==1)
			{
				SetTextValue(Polar_W_AdjustID,8,Polar.DisStr[(*Polar.PPolarA)+gLanguageChoice*2]); 
			}				
		break;
		case Polar_W_AdjustID:         
			if(control_id == 2&&state==1)
			{
				switch(*Polar.PPolarA)
				{
					case 0: (*Polar.PPolarA)=Reverse;   break;
					case 1: (*Polar.PPolarA)=Forward;   break;
					default :                 break;
				}	
				SetTextValue(Polar_W_AdjustID,8,Polar.DisStr[(*Polar.PPolarA)+gLanguageChoice*2]); 
				AnimationPlayFrame(PolarityID,4,(*Polar.PPolarA)+gLanguageChoice*2); 
			}
		break;
		case Gain_W_AdjustID: 
			anjiancl(screen_id, control_id, state,&ConfirmData,d_Gain_Data,Test_Auto_W_ID);
		break;
		/*��ݲ˵�    ��������*/
		case Parm_Set_ID:         
		
		break;
		/*��ݲ˵�    ��������    ��ƫ��������*/
		case Rec_P_SetID:         
			if(control_id == 3)		   GainBackInf = 1;
		break;
		case Mode_Rec_P_SetID:          //��ƫģʽ 
			
		break;
		case Gain_Rec_P_SetID:          //��ƫ����
			if(control_id == 6)		   SetTextInt32(Gain_Rec_P_AdjustID,5,gGainData,0,1);           //���µ��ڽ����ֵ
			else if(control_id == 7)   SetTextInt32(Dead_Rec_P_AdjustID,5,gDeadZone,0,1);
			else if(control_id == 9)   GainBackInf==0?SetScreen(Auto_ID):SetScreen(Rec_P_SetID);
		break;
		case Gain_Rec_P_AdjustID:  
			anjiancl(screen_id, control_id, state,&ConfirmData,d_Gain_Data,Gain_Rec_P_SetID);
		break;
		case Dead_Rec_P_AdjustID:  
			anjiancl(screen_id, control_id, state,&ConfirmData,d_Dead_Zone,Gain_Rec_P_SetID);			
		break;
		
		case Speed_Rec_P_SetID:          //�ٶ�����  
			if(control_id == 11)	   SetTextInt32(MSpeed_Rec_P_AdjustID,5,gManuSpeed,0,1);
			else if(control_id == 7)   SetTextInt32(ASpeed_Rec_P_AdjustID,5,gAutoSpeed,0,1);
			else if(control_id == 8)   SetTextInt32(CSpeed_Rec_P_AdjustID,5,gCentSpeed,0,1);
		break;
		case MSpeed_Rec_P_AdjustID:  
			anjiancl(screen_id, control_id, state,&ConfirmData,d_Manu_Speed,Speed_Rec_P_SetID);
		break;
		case ASpeed_Rec_P_AdjustID: 
			anjiancl(screen_id, control_id, state,&ConfirmData,d_Auto_Speed,Speed_Rec_P_SetID);		
		break;
		case CSpeed_Rec_P_AdjustID: 
			anjiancl(screen_id, control_id, state,&ConfirmData,d_Cent_Speed,Speed_Rec_P_SetID);	
		break;
		
		case AccDec_Rec_P_SetID:          //���ԼӼ��� 
			if(control_id == 8)	       SetTextInt32(AAccSpeed_Rec_P_AdjustID,5,gAFlexAcc,0,1);
			else if(control_id == 5)   SetTextInt32(ADecSpeed_Rec_P_AdjustID,5,gAFlexDec,0,1);
			else if(control_id == 19)  SetTextInt32(MAccSpeed_Rec_P_AdjustID,5,gMFlexAcc,0,1);
			else if(control_id == 26)  SetTextInt32(MDecSpeed_Rec_P_AdjustID,5,gMFlexDec,0,1);
		break;
		case AAccSpeed_Rec_P_AdjustID:  
			anjiancl(screen_id, control_id, state,&ConfirmData,d_AFlex_acc,AccDec_Rec_P_SetID);				
		break;
		case ADecSpeed_Rec_P_AdjustID: 
			anjiancl(screen_id, control_id, state,&ConfirmData,d_AFlex_dec,AccDec_Rec_P_SetID);				
		break;
		case MAccSpeed_Rec_P_AdjustID:  
			anjiancl(screen_id, control_id, state,&ConfirmData,d_MFlex_acc,AccDec_Rec_P_SetID);					
		break;
		case MDecSpeed_Rec_P_AdjustID:  
			anjiancl(screen_id, control_id, state,&ConfirmData,d_MFlex_dec,AccDec_Rec_P_SetID);				
		break;
		
		case Current_Rec_P_SetID:         
			
		break;
		
		case Current_Rec_P_AdjustID:         
			anjiancl(screen_id, control_id, state,&ConfirmData,d_Function_Torque,Current_Rec_P_SetID);	
		break;
		
		case Limit_Rec_P_SetID:         
			if(control_id == 3)		    SetTextFloat(Behind_Rec_P_SetID,5,gBehindLimit,0,1);
			else if(control_id == 4)    SetTextFloat(Center_Rec_P_SetID,5,gCenterLimit,0,1);
			else if(control_id == 8)    SetTextFloat(Front_Rec_P_SetID ,5,gFrontLimit ,0,1);
		break;
		case Behind_Rec_P_SetID: 
			anjiancl(screen_id, control_id, state,&ConfirmData,d_Behind_limit,Limit_Rec_P_SetID);							
		break;
		case Center_Rec_P_SetID: 
			anjiancl(screen_id, control_id, state,&ConfirmData,d_Center_limit,Limit_Rec_P_SetID);			
		break;
		case Front_Rec_P_SetID: 
			anjiancl(screen_id, control_id, state,&ConfirmData,d_Front_limit,Limit_Rec_P_SetID);				
		break;
		
		case SPC_Rec_P_SetID:         
			if(control_id == 7)	       SetTextFloat(SPCStop_Rec_P_AdjustID,5,(float)gSPCStopTime/10.0,1,1);
			else if(control_id == 8)   SetTextInt32(SPCBehind_Rec_P_AdjustID,5,gSPCBehindLimit,0,1);
			else if(control_id == 9)   SetTextInt32(SPCFront_Rec_P_AdjustID,5,gSPCFrontLimit,0,1);
		break;
		case SPCStop_Rec_P_AdjustID:  
			anjiancl(screen_id, control_id, state,&ConfirmData,d_SPC_stop_time,SPC_Rec_P_SetID);						
		break;
		case SPCBehind_Rec_P_AdjustID: 
			anjiancl(screen_id, control_id, state,&ConfirmData,d_SPC_Behind_limit,SPC_Rec_P_SetID);				
		break;
		case SPCFront_Rec_P_AdjustID: 
			anjiancl(screen_id, control_id, state,&ConfirmData,d_SPC_Front_limit,SPC_Rec_P_SetID);				
		break;
		
		case NO_Rec_P_SetID:         
			if(control_id == 6)	       SetTextInt32(NOValue_Rec_P_AdjustID,5,gNoDetectValve,0,1);
			else if(control_id == 7)   SetTextFloat(NODetect_Rec_P_AdjustID,5,(float)gNoDetectTime/10.0,1,1);
			else if(control_id == 8)   SetTextFloat(NOWait_Rec_P_AdjustID,5,(float)gNoWaitTime/10.0,1,1);
		break;
		case NOValue_Rec_P_AdjustID:  
			anjiancl(screen_id, control_id, state,&ConfirmData,d_No_Detect_Valve,NO_Rec_P_SetID);	
		break;
		case NODetect_Rec_P_AdjustID:   
			anjiancl(screen_id, control_id, state,&ConfirmData,d_No_Detect_Time,NO_Rec_P_SetID);		
		break;
		case NOWait_Rec_P_AdjustID: 
			anjiancl(screen_id, control_id, state,&ConfirmData,d_No_Wait_Time,NO_Rec_P_SetID);			
		break;
		case PolarityID:
			
		break;
		
		/*��ݲ˵�    ��������    ��������������*/
		case Sensor_P_SetID:   		
			if(control_id == 13)	   SetTextInt32(Offset_Sensor_P_AdjustID,5,(s8)(gFineTune-80),1,1);			
		break;
		case Offset_Sensor_P_AdjustID: 
		    anjiancl(screen_id, control_id, state,&ConfirmData,d_Fine_Tuning,Sensor_P_SetID);	
		break;
		
		/*��ݲ˵�    ��������    �ƶ�����������*/
		case Motor_P_SetID:       
			
		break;
			
		/*��ݲ˵�    ��������    ϵͳ��������*/	
		case Sys_P_SetID:   
				
		break;
		case Brightness_Sys_P_SetID:         
			switch(control_id)
			{
				case 3:    *LockTime.PBrightness = Brightness20;             break;
				case 4:    *LockTime.PBrightness = Brightness40;             break;
				case 5:    *LockTime.PBrightness = Brightness60;             break;
				case 6:    *LockTime.PBrightness = Brightness80;             break;
				case 7:    *LockTime.PBrightness = Brightness100;            break;
				case 12:   *LockTime.PLockTime = Second_30;      			 break;
				case 13:   *LockTime.PLockTime = Minute_1;    				 break;
				case 14:   *LockTime.PLockTime = Minute_5;    				 break;
				case 15:   *LockTime.PLockTime = Not_lock;	 				 break;
				default: break;
			}
			SetPowerSaving(1,0xff-LockTime.BrightnessArray[*LockTime.PBrightness],0x32,LockTime.LockTimeArray[*LockTime.PLockTime]);
		break;
		case BuzzerSetID:
			if(control_id==3 && state==1)             {gBuzzer=1;}
			else if(control_id==6 && state==1)        {gBuzzer=0;}
		break;
		case Password_SetID:
			if(control_id == 4&&state==1)      
			{		
				PasswordSet.CallID = ChangePasswordID;
				AnimationPlayFrame(Password_ProtectionID,17,PasswordSet.CallID+gLanguageChoice*3); //��ʾ�޸�������Ӣ��
				AnimationPlayFrame(Password_ProtectionID,18,0); //��ʾ�޸�������Ӣ��
			}
			else if(control_id == 6&&state==1) 
			{
				PasswordSet.CallID = RepeatInputID;
				AnimationPlayFrame(Password_ProtectionID,17,PasswordSet.CallID+gLanguageChoice*3);  //��ʾ�ظ�������Ӣ��
				AnimationPlayFrame(Password_ProtectionID,18,0); //��ʾ�޸�������Ӣ��
			}
			else if(control_id == 7&&state==1&&PasswordSet.ChangeFlag == 2)              //ȷ�ϸ�������
			{
				PasswordSet.ChangeFlag = 0;
				strcpy(PasswordSet.PPassword,PasswordSet.t_Password);
			}
		break;
		case LanguageSetID:
			if(control_id==3 && state==1)             {gLanguageChoice = Chinese;  InitLDis();}
			else if(control_id==6 && state==1)        {gLanguageChoice = English;  InitLDis();}
		break;
		case Port_Sys_P_SetID:    
			switch(control_id)
			{
				case 5:				PortNum = 0;						        break;
				case 6:				PortNum = 1;							    break;
				case 7:				PortNum = 2;								break;
				case 8:				PortNum = 3;								break;
				case 9:				*PortFun[PortNum].PPort = Nodef;			break;
				case 11:			*PortFun[PortNum].PPort = PusherLeft;		break;
				case 15:			*PortFun[PortNum].PPort = PusherRight;		break;
				case 17:			*PortFun[PortNum].PPort = PusherCenter;		break;
				case 19:			*PortFun[PortNum].PPort = ManuDef;			break;
				case 21:			*PortFun[PortNum].PPort = AutoDef;			break;
				case 23:			*PortFun[PortNum].PPort = OperPrepar;		break;
				case 25:			*PortFun[PortNum].PPort = Limit1;			break;
				case 27:			*PortFun[PortNum].PPort = Limit2;			break;
				case 29:			*PortFun[PortNum].PPort = Switch1_EPC;		break;
				case 31:			*PortFun[PortNum].PPort = Switch2_EPC;		break;
				default:														break;
			}  
			AnimationPlayFrame(current_screen_id,PortFun[PortNum].DisNum,*PortFun[PortNum].PPort+gLanguageChoice*11);  
		break;
		
		/*��ݲ˵�    */
		case RestoreBackupID:
			if(control_id==4 && state==1)		{ParaStorage.MenuCount=SBackup;}
			else if(control_id==5 && state==1)	{ParaStorage.MenuCount=SRestore; if(gBackupFlag==1) SetScreen(SYSInitAffirmID);}
			else if(control_id==6 && state==1)	{ParaStorage.MenuCount=SFactorySet;}
			AnimationPlayFrame(SYSInitAffirmID,2,ParaStorage.MenuCount+gLanguageChoice*3);
		break;
		
		case SYSInitAffirmID:
			if(control_id==3 && state==1)
			{
				t_Work_Mode=gWorkMode;
				gWorkMode=0;         //�л�Ϊ�ֶ�ģʽ����ֹ����ʱ������������Ϊ����ʱ��Ƭ������ֹͣ����
				AnimationPlayFrame(SYSInitDisID,2,ParaStorage.MenuCount+gLanguageChoice*3);
				delay_ms(1000);
				ParaStorage.fun[ParaStorage.MenuCount]();      //ִ�ж�Ӧ�ĺ�����ϵͳ���ݣ�ϵͳ��ԭ��ϵͳ��ʼ��
				SetScreen(RestoreBackupID);
				if(ParaStorage.MenuCount==SBackup || ParaStorage.MenuCount==SRestore)   gWorkMode = t_Work_Mode;
				
				InitDIS();   //ϵͳ��ԭ�ͻָ����������ö���Ҫ��ʼ����ʾ
			}
		break;
			
/*****************************�������********************************/				
		case Password_ProtectionID:
			if(control_id == 14)	
			{
				
			}
			else if(control_id == 16)  
			{
				PasswordSet.CallID == 0? SetScreen(Shoutcut_ID): SetScreen(Password_SetID);
			}
		break;	
			
/*****************************ȷ�ϲ����޸�****************************/			
		case Confirm_ID:
			Confirm(control_id,&ConfirmData);
		break;
	
/*****************************������У׼4��****************************/
		case Material_SwitchID:
			switch(control_id)
			{
				case 3:			break;
				case 4:			break;
				case 21:	SetScreen(MatCal.BackInf);		break;
				default:        break;
			}
		break;
		case MatValue_SwitchID:
			switch(control_id)
			{
				case 1:     *MatCal.CurMatNum = 0;     break;
				case 2:     *MatCal.CurMatNum = 1;     break;
				case 3:     *MatCal.CurMatNum = 2;     break;
				case 4:     *MatCal.CurMatNum = 3;     break;
				case 5:     *MatCal.CurMatNum = 4;     break;
				case 6:     *MatCal.CurMatNum = 5;     break;
				case 7:     *MatCal.CurMatNum = 6;     break;
				case 8:     *MatCal.CurMatNum = 7;     break;
				default:    break;
			}
			SetTextInt32(MatValue_SwitchID,10,*MatCal.CurMatNum,0,1);
			SetTextInt32(Material_SwitchID,10,*MatCal.CurMatNum,0,1);
		break;
	
		case Sensor_Calibr1ID:      
			switch(control_id)
			{
				case 1: MatCal.EPCNum = EPC1;         break;   //ѡ��У׼������EPC1
				case 2: gWorkMode ==0?SetScreen(Sensor_Calibr2ID):SetScreen(Ask_to_SwitchID);        break;
				case 3: MatCal.EPCNum = EPC2;         break;   //ѡ��У׼������EPC2
				default:                              break;
			}
		break;

		case Ask_to_SwitchID:
			if(control_id ==3)      {gWorkMode=0; }
		break;
		case Sensor_Calibr2ID:         	break;
		case Sensor_Calibr3ID:          break;
		case Sensor_Calibr4ID:     
			if(control_id ==9)
			{
				*MatCal.MatValue[*MatCal.CurMatNum].PEPC12=MatCal.EPCNum==EPC1?(*MatCal.MatValue[*MatCal.CurMatNum].PEPC12|(1<<1)):(*MatCal.MatValue[*MatCal.CurMatNum].PEPC12|1);
				AnimationPlayFrame(MatValue_SwitchID,*MatCal.CurMatNum+11,*MatCal.MatValue[*MatCal.CurMatNum].PEPC12); 
			}
		break;
			
/*****************************�г�У׼****************************/		
		case Travel_SetID:            //�����г�����Ӧ����ȡת��
			if(control_id == 1&&gWorkMode==0)     {TravelCal.CaliFlag =1;	TravelCal.CaliStep=0;  SetScreen(Travel_AdaptationID);}
			else if(control_id == 9)   SetScreen(TravelCal.BackInf);
			else if(control_id == 17)  gWorkMode = 0;
		break;
		case Travel_AdaptationID:
			if(control_id == 2) 
			{
				TravelCal.CaliFlag = 0;
				TravelCal.CaliStep = 0;
				TravelCal.StallDir = 0;
			}
		break;
		default:           break;	
	}
}

/*! 
*  \brief  �ı��ؼ�֪ͨ
*  \details  ���ı�ͨ�����̸���(�����GetControlValue)ʱ��ִ�д˺���
*  \details  �ı��ؼ����������ַ�����ʽ�·���MCU������ı��ؼ������Ǹ���ֵ��
*  \details  ����Ҫ�ڴ˺����н��·��ַ�������ת�ظ���ֵ��
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param str �ı��ؼ�����
*/
void NotifyText(u16 screen_id, u16 control_id, u8 *str)
{
	if(screen_id == Password_ProtectionID)	
	{
//		sscanf(str,"%ld",&value);      //���ַ���ת��Ϊ���� 
		PasswordSet.fun[PasswordSet.CallID](str);     //���ݵ��ý���CallID�ж�ִ���ĸ����ܺ���
	}
}                

/*!                                                                              
*  \brief  �������ؼ�֪ͨ ,�϶�������ʱ��ִ��                                                      
*  \details  ����GetControlValueʱ��ִ�д˺���                                  
*  \param screen_id ����ID                                                      
*  \param control_id �ؼ�ID                                                     
*  \param value ֵ                                                              
*/                                                                              
void NotifyProgress(u16 screen_id, u16 control_id, u32 value)           
{  
    if(screen_id == 3)
    {  
    }    
}                                                                                

/*!                                                                              
*  \brief  �������ؼ�֪ͨ                                                       
*  \details  ���������ı�(�����GetControlValue)ʱ��ִ�д˺���                  
*  \param screen_id ����ID                                                      
*  \param control_id �ؼ�ID                                                     
*  \param value ֵ                                                              
*/                                                                              
void NotifySlider(u16 screen_id, u16 control_id, u32 value)             
{                                                             
	if(screen_id == 10)
    {  
	}
}

/*! 
*  \brief  �Ǳ�ؼ�֪ͨ
*  \details  ����GetControlValueʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param value ֵ
*/
void NotifyMeter(u16 screen_id, u16 control_id, u32 value)
{
    //TODO: ����û�����
}

/*! 
*  \brief  �˵��ؼ�֪ͨ
*  \details  ���˵���»��ɿ�ʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param item �˵�������
*  \param state ��ť״̬��0�ɿ���1����
*/
void NotifyMenu(u16 screen_id, u16 control_id, u8 item, u8 state)
{
    //TODO: ����û�����
	if(screen_id == Travel_SetID&&control_id==4 && state==1)         //���У׼ת��ѡ��
	{
		switch(item)
		{
			case 0:    *TravelCal.CalCurrent = 10;      break;
			case 1:    *TravelCal.CalCurrent = 15;      break;
			case 2:    *TravelCal.CalCurrent = 20;      break;
			case 3:    *TravelCal.CalCurrent = 30;      break;
			default :            break;
		}
	}
	else if(screen_id == Mode_Rec_P_SetID && control_id == 3 && state==1)
	{
		switch(item)
		{
			case 0:    *SensorMode.PSensorMode = 0;      break;
			case 1:    *SensorMode.PSensorMode = 1;      break;
			case 2:    *SensorMode.PSensorMode = 2;      break;
			case 3:    *SensorMode.PSensorMode = 3;      break;
			default :            break;
		}
		AnimationPlayFrame(Mode_Rec_P_SetID,2,*SensorMode.PSensorMode); 
//		SetTextValue(Auto_ID,18,SensorMode.DisStr[(*SensorMode.PSensorMode)]);
//		SetTextValue(Manu_ID,18,SensorMode.DisStr[(*SensorMode.PSensorMode)]);
//		SetTextValue(Center_ID,18,SensorMode.DisStr[(*SensorMode.PSensorMode)]);
	}
	else if(screen_id == SPC_Rec_P_SetID&&control_id==18 && state==1)
	{
		switch(item)
		{
			case 0:    gSPCMode = 0;       break;
			case 1:    gSPCMode = 1;       break;
			default :            break;
		}
		AnimationPlayFrame(SPC_Rec_P_SetID,16,gSPCMode+gLanguageChoice*2);  
	}
	else if(screen_id == Limit_Rec_P_SetID&&control_id==18 && state==1)
	{
		switch(item)
		{
			case 0:    gLimitMode = 0;       break;
			case 1:    gLimitMode = 1;       break;
			case 2:    gLimitMode = 2;       break;
			default :            break;
		}
		AnimationPlayFrame(Limit_Rec_P_SetID,17,gLimitMode+gLanguageChoice*3); 
	}
	else if(screen_id == NO_Rec_P_SetID && control_id==15 && state==1)
	{
		switch(item)
		{
			case 0:    gNoWaitEN = 0;       break;     //ʹ��
			case 1:    gNoWaitEN = 1;       break;    
			default :            break;
		}
		AnimationPlayFrame(NO_Rec_P_SetID,9,gNoWaitEN+gLanguageChoice*2); 
	}
	else if(screen_id == PolarityID && control_id==7 && state==1)
	{
		switch(item)
		{
			case 0:    *Polar.PPolarM = Forward;       break;
			case 1:    *Polar.PPolarM = Reverse;       break;    //ʹ��
			default :            break;
		}
		AnimationPlayFrame(PolarityID,3,(*Polar.PPolarM)+gLanguageChoice*2); 
	}
	else if(screen_id == PolarityID && control_id==8 && state==1)
	{
		switch(item)
		{
			case 0:    *Polar.PPolarA = Forward;       break;
			case 1:    *Polar.PPolarA = Reverse;       break;    //ʹ��
			default :            break;
		}
		AnimationPlayFrame(PolarityID,4,(*Polar.PPolarA)+gLanguageChoice*2); 
	}
	else if(screen_id == AccDec_Rec_P_SetID && control_id==15 && state==1)
	{
		switch(item)
		{
			case 0:    gAFlexSacc = DefaultDef;     gAFlexAcc = AFlexAcc; break;
			case 1:    gAFlexSacc = Custom;         break;
			default :            break;
		}
		SetControlVisiable(AccDec_Rec_P_SetID,8,gAFlexSacc);
		AnimationPlayFrame(AccDec_Rec_P_SetID,7,gAFlexSacc+gLanguageChoice*2);
	}
	else if(screen_id == AccDec_Rec_P_SetID && control_id==16 && state==1)
	{
		switch(item)
		{
			case 0:    gAFlexSdec = DefaultDef;     gAFlexDec = AFlexDec; break;
			case 1:    gAFlexSdec = Custom;        break;
			default :            break;
		}
		SetControlVisiable(AccDec_Rec_P_SetID,5, gAFlexSdec);
		AnimationPlayFrame(AccDec_Rec_P_SetID,11,gAFlexSdec+gLanguageChoice*2);
	}
	else if(screen_id == AccDec_Rec_P_SetID && control_id==23 && state==1)
	{
		switch(item)
		{
			case 0:    gMFlexSacc = DefaultDef;     gMFlexAcc = MFlexAcc; break;
			case 1:    gMFlexSacc = Custom;           break;
			default :            break;
		}
		SetControlVisiable(AccDec_Rec_P_SetID,19,gMFlexSacc);
		AnimationPlayFrame(AccDec_Rec_P_SetID,21,gMFlexSacc+gLanguageChoice*2);
	}
	else if(screen_id == AccDec_Rec_P_SetID && control_id==30 && state==1)
	{
		switch(item)
		{
			case 0:    gMFlexSdec = DefaultDef;     gMFlexDec = MFlexDec; break;
			case 1:    gMFlexSdec = Custom;           break;
			default :            break;
		}
		SetControlVisiable(AccDec_Rec_P_SetID,26,gMFlexSdec);
		AnimationPlayFrame(AccDec_Rec_P_SetID,28,gMFlexSdec+gLanguageChoice*2);
	}
	else if(screen_id == Motor_P_SetID&&control_id==9 && state==1)        //�ƶ�������
	{
		switch(item)
		{
			case 0:   gMotorType = BrushlessMotor4;  break;
			case 1:   gMotorType = BrushlessMotor5;  break;
			default:                                      break;   
		}
		AnimationPlayFrame(Motor_P_SetID,6,gMotorType+gLanguageChoice*2); 
		switch(gMotorType)                                           //��ѯ�������
		{
			case 0:                        //���ѡ�����ˢ����Ϳ�CAP,���ⲿ�ж���Ϊ�������    4�Լ�
				NUMBERTURN = 7500000;
				SERIES     =   24;          
			break;                   
			case 1:                        //      5�Լ�
				NUMBERTURN = 6000000;
				SERIES     =   30;
			break; 
			default: break;                   //ϵͳ��ʼ��  
		}
	}
	else if(screen_id == Power_on_StateID && control_id==4 && state==1)
	{
		switch(item)
		{
			case 0:  gPowerOnMode =  ManuInterface;  break;
			case 1:  gPowerOnMode =  AutoInterface;  break;
			case 2:  gPowerOnMode =  CentInterface;  break;
			case 3:  gPowerOnMode =  LastInterface;  break;
			default :                                          break;
		}
		AnimationPlayFrame(Power_on_StateID,2,gPowerOnMode+gLanguageChoice*4); 
	}
}

/*! 
*  \brief  ѡ��ؼ�֪ͨ
*  \details  ��ѡ��ؼ��仯ʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param item ��ǰѡ��
*/
void NotifySelector(u16 screen_id, u16 control_id, u8  item)
{

}

/*! 
*  \brief  ��ʱ����ʱ֪ͨ����
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*/
void NotifyTimer(u16 screen_id, u16 control_id)
{
    if(screen_id==8&&control_id == 7)
    {
        SetBuzzer(100);
    } 
}

/*! 
*  \brief  ��ȡ�û�FLASH״̬����
*  \param status 0ʧ�ܣ�1�ɹ�
*  \param _data ��������
*  \param length ���ݳ���
*/
void NotifyReadFlash(u8 status,u8 *_data,u16 length)
{
    //TODO: ����û�����
}

/*! 
*  \brief  д�û�FLASH״̬����
*  \param status 0ʧ�ܣ�1�ɹ�
*/
void NotifyWriteFlash(u8 status)
{
    //TODO: ����û�����
}

/*! 
*  \brief  ��ȡRTCʱ�䣬ע�ⷵ�ص���BCD��
*  \param year �꣨BCD��
*  \param month �£�BCD��
*  \param week ���ڣ�BCD��
*  \param day �գ�BCD��
*  \param hour ʱ��BCD��
*  \param minute �֣�BCD��
*  \param second �루BCD��
*/
void NotifyReadRTC(u8 year,u8 month,u8 week,u8 day,u8 hour,u8 minute,u8 second)
{

}

void Adjustment_interface(u16 screen_id,u16 control_id,u16 *data,u8 StepData,u8 state)
{
	static u8 Key_time_number=0;
	static u8 key_sun_bit=0;
	
	if(state == 1||state == 2)      //����״̬����ʼ��ʱ
	{
		switch(control_id)
		{
			case 2:  
				if(key_sun_bit==0)      //�̰�ִֻ��һ��
				{
					key_sun_bit=1;
					(*data)+=StepData;
				}
				else
				{
					Key_time_number<7?++Key_time_number:(*data+=StepData);    //����700ms������ټӼ�
				}
			break;
				
			case 3:  
				if(key_sun_bit==0)
				{
					key_sun_bit=1;
					(*data)-=StepData;
				}
				else
				{
					Key_time_number<7?++Key_time_number:(*data-=StepData);
				}
			break;
			default:                break;
		}
	}
	else if(state == 0)
	{
		key_sun_bit=0;
		Key_time_number=0;
	}
}


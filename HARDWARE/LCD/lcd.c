#include <string.h>
#include "cmd_process.h"
#include "delay.h"
#include "lcdtest.h"
#include "main.h"
#include "stmflash.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

extern  s16 OcclusionL_rate,OcclusionR_rate;             //传感器遮挡率
extern  u8  cmd_buffer[CMD_MAX_SIZE];  


u16     current_screen_id = 0;          //当前画面ID
u16     last_screen_id;             //上一画面ID，主要用于返回
u16     BrightnessDis=100;              //屏幕亮度调节范围0~100
u16     Screen_brightness;      //屏幕亮度
u16     t_Work_Mode;                //暂存工作模式
u8      PortNum;



LockTimeDef LockTime 		= {&gBrightness,{0xcc,0x99,0x66,0x33,0x00},&gLockTime,{5,60,300,50000}};
ParaStorageDef ParaStorage	= {SRestore,{Backup,Restore,ParameterInit}};
SensorModeDef SensorMode 	= {&gSensorMode,{"EPC1","EPC2","CPC","SPC"},{"传感器1/EPC1","传感器2/EPC2","传感器1+2/CPC","蛇形纠偏/SPC"},SensorEPC1Dis,SensorEPC2Dis,SensorCPCDis,SensorSPCDis};
TravelCalDef TravelCal 		= {0,0,0,0,&gCaliTorque};						
PolarDef Polar         		= {&gAutoPolar,&gManuPolar,{"正向","反向","Forward","Reverse"},{"+","-"}};
PasswordSetDef PasswordSet  = {0,0,Password,{0},{InputPassword,ChangePassword,RepeatInput}};

MatCalDef MatCal = {            //材料校准
	Manu_ID,                    //默认返回界面为手动界面
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
	&gCurMatNum,       //材料编号默认0
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


/****************初始化显示*******************/
void InitLDis()
{
	/****显示加减速文本框内容****/
	BatchBegin(AccDec_Rec_P_SetID);    //开始批量更新
	BatchSetFrame(7, gAFlexSacc+gLanguageChoice*2);
	BatchSetFrame(11,gAFlexSdec+gLanguageChoice*2);
	BatchSetFrame(21,gMFlexSacc+gLanguageChoice*2);
	BatchSetFrame(28,gMFlexSdec+gLanguageChoice*2);
	BatchEnd();
	
	/****显示限位模式文本框****/
	AnimationPlayFrame(Limit_Rec_P_SetID,17,gLimitMode+gLanguageChoice*3);  
	
	/****显示SPC模式文本框内容****/
	AnimationPlayFrame(SPC_Rec_P_SetID,16,gSPCMode+gLanguageChoice*2); 
	
	/****显示无料使能文本框内容****/
	AnimationPlayFrame(NO_Rec_P_SetID,9,gNoWaitEN+gLanguageChoice*2); 
	
	/****显示极性文本框内容****/
	AnimationPlayFrame(PolarityID,3,(*Polar.PPolarM)+gLanguageChoice*2); 
	AnimationPlayFrame(PolarityID,4,(*Polar.PPolarA)+gLanguageChoice*2); 
	
	/****显示电机类型文本框内容****/
	AnimationPlayFrame(Motor_P_SetID,6,gMotorType+gLanguageChoice*2); 
	
	/****显示开机状态文本框内容****/
	AnimationPlayFrame(Power_on_StateID,2,gPowerOnMode+gLanguageChoice*4); 
	
	/****显示端口定义文本框内容****/
	BatchBegin(Port_Sys_P_SetID);    //开始批量更新
	BatchSetFrame(PortFun[0].DisNum,*PortFun[0].PPort+gLanguageChoice*11);
	BatchSetFrame(PortFun[1].DisNum,*PortFun[1].PPort+gLanguageChoice*11);
	BatchSetFrame(PortFun[2].DisNum,*PortFun[2].PPort+gLanguageChoice*11);
	BatchSetFrame(PortFun[3].DisNum,*PortFun[3].PPort+gLanguageChoice*11);
	BatchEnd();
}

/****************初始化LCD*******************/
void InitDIS(void)
{
	int cnt;
	/********启动默认，加减速不能调节********/
	SetControlVisiable(AccDec_Rec_P_SetID,8, gAFlexSacc);
	SetControlVisiable(AccDec_Rec_P_SetID,5, gAFlexSdec);
	SetControlVisiable(AccDec_Rec_P_SetID,19,gMFlexSacc);
	SetControlVisiable(AccDec_Rec_P_SetID,26,gMFlexSdec);

//	SetControlVisiable(Auto_ID,23,0);
	AnimationPlayFrame(RestoreBackupID,7,gBackupFlag);
	
	//初始化显示行程自适应界面电流
	SetTextFloat(Travel_SetID,3,*TravelCal.CalCurrent/10.0,1,1); 
	
	//屏幕亮度初始化
	SetPowerSaving(1,0xff-(u8)LockTime.BrightnessArray[*LockTime.PBrightness],0x32,LockTime.LockTimeArray[*LockTime.PLockTime]);
	SetButtonValue(Brightness_Sys_P_SetID,*LockTime.PLockTime+12,1);
	SetBackLight(LockTime.BrightnessArray[*LockTime.PBrightness]);
	SetButtonValue(Brightness_Sys_P_SetID,*LockTime.PBrightness+3,1);
	
	//传感器显示初始化
	AnimationPlayFrame(Mode_Rec_P_SetID,2,*SensorMode.PSensorMode); 
	
	/****材料选择显示****/
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
	//上电隐藏长按计数
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

/***********自动运行，手动运行，中心对应的显示程序**************/
void AutoDis()
{
	float Dis_Pulse_Num;
	Dis_Pulse_Num = (100.0*gCurrentPulseNum/gMAXPulseNum);
	gWorkMode = 1;
	SensorMode.fun[*SensorMode.PSensorMode]();
	SetProgressValue(Auto_ID,29,Dis_Pulse_Num);        //设置进度条的值1000*(1.0*gCurrentPulseNum/gMAXPulseNum)
	
	SetTextInt32(Auto_ID,12,gGainData,0,1); 
	SetTextInt32(Auto_ID,14,gCurMatNum,0,1); 
	SetTextInt32(Auto_ID,16,(s8)(gFineTune-80),1,1);
	SetTextValue(Auto_ID,18,SensorMode.DisStr[*SensorMode.PSensorMode]);
	SetTextValue(Auto_ID,20,Polar.DisStrSymbol[*Polar.PPolarA]);
	SetTextFloat(Auto_ID,30,Dis_Pulse_Num,1,1); 
	
#ifdef  _TEST_
	SetTextInt32(current_screen_id,60,g_ADC_Buf[0]*8,0,1);              //电流显示
	SetTextInt32(current_screen_id,61,gCurrentPulseNum,0,1);         //显示脉冲数
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
	SetProgressValue(Manu_ID,29,Dis_Pulse_Num);        //设置进度条的值1000*(1.0*gCurrentPulseNum/gMAXPulseNum)

	SetTextInt32(Manu_ID,12,gGainData,0,1); 
	SetTextInt32(Manu_ID,14,gCurMatNum,0,1); 
	SetTextInt32(Manu_ID,16,(s8)(gFineTune-80),1,1);
	SetTextValue(Manu_ID,18,SensorMode.DisStr[*SensorMode.PSensorMode]);
	SetTextValue(Manu_ID,20,Polar.DisStrSymbol[*Polar.PPolarA]);
	SetTextFloat(Manu_ID,30,Dis_Pulse_Num,1,1); 
	
#ifdef  _TEST_
SetTextInt32(current_screen_id,60,g_ADC_Buf[0]*8,0,1);              //电流显示
SetTextInt32(current_screen_id,61,gCurrentPulseNum,0,1);         //显示脉冲数
#endif
}

void CenterDis()
{
	float Dis_Pulse_Num;
	Dis_Pulse_Num = (100.0*gCurrentPulseNum/gMAXPulseNum);
	gWorkMode = 2;
	SetProgressValue(Center_ID,29,Dis_Pulse_Num);        //设置进度条的值1000*(1.0*gCurrentPulseNum/gMAXPulseNum)
	
	SetTextInt32(Center_ID,12,gGainData,0,1); 
	SetTextInt32(Center_ID,14,gCurMatNum,0,1); 
	SetTextInt32(Center_ID,16,(s8)(gFineTune-80),1,1);
	SetTextValue(Center_ID,18,SensorMode.DisStr[*SensorMode.PSensorMode]);
	SetTextValue(Center_ID,20,Polar.DisStrSymbol[*Polar.PPolarA]);
	SetTextInt32(Center_ID,22,gCenterLimit,0,1);
	SetTextFloat(Center_ID,30,Dis_Pulse_Num,1,1); 
#ifdef  _TEST_
SetTextInt32(current_screen_id,60,g_ADC_Buf[0]*8,0,1);              //电流显示
SetTextInt32(current_screen_id,61,gCurrentPulseNum,0,1);         //显示脉冲数
#endif
}

/************4种传感器模式对应的显示程序***************/
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
	if(gSPCMode == 0)    //0：内部编码器  1：外部传感器
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

/**************传感器校准界面显示程序*******************/
void NoShelterDis()
{
	switch(MatCal.EPCNum)
	{
		case EPC1:    
			MatCal.t_Sensor1_valueH = SensorL_value;
//			*MatCal.MatValue[*MatCal.CurMatNum].PSensor1_H = SensorL_value;                     //保存传感器1（左边）最大值
			SetTextInt32(current_screen_id,1,SensorL_value,0,1);        //更新到进度条和文本框
			SetTextValue(current_screen_id,3,"EPC1");                    //更新到进度条和文本框
		break;          
		case EPC2:    
			MatCal.t_Sensor2_valueH = SensorR_value;
//			*MatCal.MatValue[*MatCal.CurMatNum].PSensor2_H = SensorR_value;       //保存传感器2（右边）最大值
			SetTextInt32(current_screen_id,1,SensorR_value,0,1);        //更新到进度条和文本框
			SetTextValue(current_screen_id,3,"EPC2");                    //更新到进度条和文本框
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
//			*MatCal.MatValue[*MatCal.CurMatNum].PSensor1_L = SensorL_value;       //保存传感器1（左边）最小值
			SetTextInt32(current_screen_id,1,SensorL_value,0,1);        //更新到进度条和文本框
			SetTextValue(current_screen_id,3,"EPC1");                    //更新到进度条和文本框
		break;          
		case EPC2:    
			MatCal.t_Sensor2_valueL = SensorR_value;
//			*MatCal.MatValue[*MatCal.CurMatNum].PSensor2_L = SensorR_value;       //保存传感器2（右边）最小值
			SetTextInt32(current_screen_id,1,SensorR_value,0,1);        //更新到进度条和文本框
			SetTextValue(current_screen_id,3,"EPC2");                    //更新到进度条和文本框
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
			SetTextInt32(current_screen_id,4,OcclusionL_rate,1,0);        //更新到进度条和文本框
			SetProgressValue(current_screen_id,2,OcclusionL_rate);                       //设置进度条的值
			if((MatCal.t_Sensor1_valueH-MatCal.t_Sensor1_valueL>125))   
			{
				*MatCal.MatValue[*MatCal.CurMatNum].PSensor1_H = MatCal.t_Sensor1_valueH;
				*MatCal.MatValue[*MatCal.CurMatNum].PSensor1_L = MatCal.t_Sensor1_valueL;
				AnimationPlayFrame(Sensor_Calibr4ID,8,0+gLanguageChoice*2);  //显示错误提示中英文
				WriteSensor();
			}
			else if((MatCal.t_Sensor1_valueH-MatCal.t_Sensor1_valueL<-125)) 
			{
				*MatCal.MatValue[*MatCal.CurMatNum].PSensor1_H = MatCal.t_Sensor1_valueL;
				*MatCal.MatValue[*MatCal.CurMatNum].PSensor1_L = MatCal.t_Sensor1_valueH;
				AnimationPlayFrame(Sensor_Calibr4ID,8,0+gLanguageChoice*2);  //显示错误提示中英文
				WriteSensor();
			}		
			else
			{
				AnimationPlayFrame(Sensor_Calibr4ID,8,1+gLanguageChoice*2);  //显示错误提示中英文
			}
		break;          //保存传感器1（左边）最小值
		case EPC2: 
			SetTextValue(current_screen_id,5,"EPC2");				
			SetTextInt32(current_screen_id,4,OcclusionR_rate,1,0);        //更新到进度条和文本框
			SetProgressValue(current_screen_id,2,OcclusionR_rate); 
			if((MatCal.t_Sensor2_valueH-MatCal.t_Sensor2_valueL>125))   
			{
				*MatCal.MatValue[*MatCal.CurMatNum].PSensor2_H = MatCal.t_Sensor2_valueH;
				*MatCal.MatValue[*MatCal.CurMatNum].PSensor2_L = MatCal.t_Sensor2_valueL;
				AnimationPlayFrame(Sensor_Calibr4ID,8,0+gLanguageChoice*2);  //显示校准成功中英文
				WriteSensor();
			}
			else if((MatCal.t_Sensor2_valueH-MatCal.t_Sensor2_valueL<-125)) 
			{
				*MatCal.MatValue[*MatCal.CurMatNum].PSensor2_H = MatCal.t_Sensor2_valueL;
				*MatCal.MatValue[*MatCal.CurMatNum].PSensor2_L = MatCal.t_Sensor2_valueH;
				AnimationPlayFrame(Sensor_Calibr4ID,8,0+gLanguageChoice*2);  //显示校准成功中英文
				WriteSensor();
			}		
			else
			{
				AnimationPlayFrame(Sensor_Calibr4ID,8,1+gLanguageChoice*2);  //显示校准失败中英文
			}			    
		break;          //保存传感器2（右边）最小值
		default :                                break;
	}
}

/*******密码设置相关函数*****/
void InputPassword(u8 *str)
{	
	if((memcmp(Password,str,6)==0)||(memcmp(SysPassword,str,6)==0))
	{
		SetScreen(Parm_Set_ID);
		AnimationPlayFrame(Password_ProtectionID,18,0);    //隐藏错误提示
	}
	else
	{
		AnimationPlayFrame(Password_ProtectionID,18,PasswordSet.CallID+1+gLanguageChoice*2);  //显示错误提示中英文
	}
}

void ChangePassword(u8 *str)
{
	PasswordSet.ChangeFlag = 1;
	strcpy((char *)PasswordSet.t_Password,(const char *)str); //暂存密码,两个指针指向同一个常量空间
	SetScreen(Password_SetID);
}

void RepeatInput(u8 *str)
{
	if((memcmp(PasswordSet.t_Password ,str,6)==0)&& PasswordSet.ChangeFlag==1)//判断再次输入的密码是否相同
	{
		PasswordSet.ChangeFlag = 2;
		SetScreen(Password_SetID);
	}
	else
	{
		AnimationPlayFrame(Password_ProtectionID,18,PasswordSet.CallID+gLanguageChoice*2); //显示两次密码不同
	}
}

void anjiancl(u16 screen_id, u16 control_id, u8  state, ConfirmDataDef *PCData, enum AdjustDataType AData, u16 BackInf)  //结构体指针指向传递过来的结构体数组成员
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
	if(control_id==4)   //取消按下
	{
		*PCData->AdjustData[PCData->MenuCount].tData=ConfirmData.TempData;     //还原原始数据
	}
	ConfirmData.ExitFlag=1;
	SetScreen(PCData->BackInface);
}


/*! 
*  \brief  消息处理流程
*  \param msg 待处理消息
*  \param size 消息长度
*/
void ProcessMessage(PCTRL_MSG msg, u16 size )
{
    u8 cmd_type = msg->cmd_type;                                                  //指令类型
    u8 ctrl_msg = msg->ctrl_msg;                                                  //消息的类型
    u8 control_type = msg->control_type;                                          //控件类型
    u16 screen_id = PTR2U16(&msg->screen_id);                                     //画面ID   16位数据
    u16 control_id = PTR2U16(&msg->control_id);                                   //控件ID   16位数据
    u32 value = PTR2U32(msg->param);                                              //数值

    switch(cmd_type)
    {  
		case NOTIFY_TOUCH_PRESS:                                                        //触摸屏按下
		case NOTIFY_TOUCH_RELEASE:                                                      //触摸屏松开
			NotifyTouchXY(cmd_buffer[1],PTR2U16(cmd_buffer+2),PTR2U16(cmd_buffer+4)); 
			break;                                                                    
		case NOTIFY_WRITE_FLASH_OK:                                                     //写FLASH成功
			NotifyWriteFlash(1);                                                      
			break;                                                                    
		case NOTIFY_WRITE_FLASH_FAILD:                                                  //写FLASH失败
			NotifyWriteFlash(0);                                                      
			break;                                                                    
		case NOTIFY_READ_FLASH_OK:                                                      //读取FLASH成功
			NotifyReadFlash(1,cmd_buffer+2,size-6);                                     //去除帧头帧尾
			break;                                                                    
		case NOTIFY_READ_FLASH_FAILD:                                                   //读取FLASH失败
			NotifyReadFlash(0,0,0);                                                   
			break;                                                                    
		case NOTIFY_READ_RTC:                                                           //读取RTC时间
			NotifyReadRTC(cmd_buffer[2],cmd_buffer[3],cmd_buffer[4],cmd_buffer[5],cmd_buffer[6],cmd_buffer[7],cmd_buffer[8]);
			break;
		case NOTIFY_CONTROL:                                                            //控件更新通知
		{
			if(ctrl_msg==MSG_GET_CURRENT_SCREEN)                                    //画面ID变化通知
			{
				NotifyScreen(screen_id);                                            //画面切换调动的函数
			}
			else 
			{
				switch(control_type)
				{
					case kCtrlButton:                                                   //按钮控件
						NotifyButton(screen_id,control_id,msg->param[1]);                 
						break;                                                             
					case kCtrlText:                                                     //文本控件
						NotifyText(screen_id,control_id,msg->param);                       
						break;                                                             
					case kCtrlProgress:                                                 //进度条控件
						NotifyProgress(screen_id,control_id,value);                     //根据屏幕进度条控件上传的value，更新文本框的值
						break;                                                             
					case kCtrlSlider:                                                   //滑动条控件
						NotifySlider(screen_id,control_id,value);                          
						break;                                                             
					case kCtrlMeter:                                                    //仪表控件
						NotifyMeter(screen_id,control_id,value);                           
						break;                                                             
					case kCtrlMenu:                                                     //菜单控件
						NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);      
						break;                                                              
					case kCtrlSelector:                                                 //选择控件
						NotifySelector(screen_id,control_id,msg->param[0]);                
						break;                                                              
					case kCtrlRTC:                                                      //倒计时控件
						NotifyTimer(screen_id,control_id);
						break;
					default:
						break;
				}
			} 
			break;  
		} 
		case NOTIFY_HandShake:                                                          //握手通知                                                     
			NOTIFYHandShake();
			break;
		default:
			break;
    }
}
/*! 
*  \brief  握手通知
*/
void NOTIFYHandShake()
{
   SetButtonValue(3,2,1);
}

/*! 
*  \brief  画面切换通知
*  \details  当前画面改变时(或调用GetScreen)，执行此函数
*  \param screen_id 当前画面ID
*/
void NotifyScreen(u16 screen_id)                                     //画面切换调动的函数
{
    //TODO: 添加用户代码
    current_screen_id = screen_id;    //在工程配置中开启画面切换通知，记录当前画面ID
    //进到自动界面，画面ID=0
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
			
		/*自动菜单*/
		case Auto_ID:  			    	  AutoDis();         break;
		case Polar_A_AdjustID:      	    		         break;
		case Gain_A_AdjustID:  			 
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gGainData;      ConfirmData.ExitFlag=0;}      //暂存一下原始数据
			SetTextInt32(current_screen_id,5,gGainData,0,1);              
		break;
		case Offset_A_AdjustID:  		 
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gFineTune;     ConfirmData.ExitFlag=0;}     //暂存一下原始数据
			SetTextInt32(current_screen_id,5,(s8)(gFineTune-80),1,1);   
		break;
		
		/*手动菜单*/
		case Manu_ID:               	 ManuDis();          break;

		/*中心菜单*/
		case Center_ID:               	 CenterDis();        break;
		case Center_AdjustID:
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gCenterLimit;     ConfirmData.ExitFlag=0;}  //暂存一下原始数据				
			SetTextInt32(current_screen_id,5,gCenterLimit,0,1);   
		break;
		/*快捷菜单*/
		case Shoutcut_ID:  			         break;
		/*快捷菜单    向导*/
		case Wizard_ID:  			     TravelCal.BackInf = Wizard_ID; break;
		case Manu_W_ID:  			     gWorkMode = 0;   break;
		case Sensor_W_SetID:  			 MatCal.BackInf = Sensor_W_SetID;        break;
		case Test_Manu_W_ID:  			 gWorkMode = 0;   break;
		case Test_Auto_W_ID:  			 gWorkMode = 1;   break;
		case Polar_W_AdjustID:  		                    break;
		case Gain_W_AdjustID:    
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gGainData;  ConfirmData.ExitFlag=0;}     //暂存一下原始数据
			SetTextInt32(current_screen_id,5,gGainData,0,1);    
		break;
		
		/*快捷菜单    参数设置*/
		case Parm_Set_ID:  			         break;
		/*快捷菜单    参数设置    纠偏参数设置*/
		case Rec_P_SetID:  			         break;
		
		case Mode_Rec_P_SetID:           AnimationPlayFrame(Mode_Rec_P_SetID,2,*SensorMode.PSensorMode);     break;                //纠偏模式
		
		case Gain_Rec_P_SetID:  		 SetTextInt32(current_screen_id,4,gGainData,0,1);//纠偏增益
										 SetTextInt32(current_screen_id,5,gDeadZone,0,0);               break;   
										 
		case Gain_Rec_P_AdjustID:  	
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gGainData;    ConfirmData.ExitFlag=0;}   //暂存一下原始数据
			SetTextInt32(current_screen_id,5,gGainData,0,1);               
		break;
		case Dead_Rec_P_AdjustID:
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gDeadZone;  ConfirmData.ExitFlag=0;}     //暂存一下原始数据			
			SetTextInt32(current_screen_id,5,gDeadZone,0,0);               
		break;
		
		case Speed_Rec_P_SetID:          SetTextInt32(current_screen_id,4,gManuSpeed,0,1);     //纠偏速度 
										 SetTextInt32(current_screen_id,5,gAutoSpeed,0,1);       
										 SetTextInt32(current_screen_id,6,gCentSpeed,0,1);            break;
		case MSpeed_Rec_P_AdjustID:  
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gManuSpeed;  ConfirmData.ExitFlag=0;}     //暂存一下原始数据			
			SetTextInt32(current_screen_id,5,gManuSpeed,0,0);            
		break;
		case ASpeed_Rec_P_AdjustID: 
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gAutoSpeed;     ConfirmData.ExitFlag=0;}    //暂存一下原始数据			
			SetTextInt32(current_screen_id,5,gAutoSpeed,0,0);            
		break;
		case CSpeed_Rec_P_AdjustID: 
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gCentSpeed;     ConfirmData.ExitFlag=0;}   //暂存一下原始数据			
			SetTextInt32(current_screen_id,5,gCentSpeed,0,0);            
		break;
		
		case AccDec_Rec_P_SetID:  		 SetTextInt32(current_screen_id,3,gAFlexAcc,0,1);        //更新到进度条和文本框
										 SetTextInt32(current_screen_id,4,gAFlexDec,0,1);        //纠偏加减速
										 SetTextInt32(current_screen_id,18,gMFlexAcc,0,1);        //更新到进度条和文本框
										 SetTextInt32(current_screen_id,25,gMFlexDec,0,1);    break;      //纠偏加减速
		case AAccSpeed_Rec_P_AdjustID:  
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gAFlexAcc;      ConfirmData.ExitFlag=0;}    //暂存一下原始数据	
			SetTextInt32(current_screen_id,5,gAFlexAcc,0,0);     
		break;
		case ADecSpeed_Rec_P_AdjustID: 
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gAFlexDec;      ConfirmData.ExitFlag=0;}   //暂存一下原始数据				
			SetTextInt32(current_screen_id,5,gAFlexDec,0,0);     
		break;
		case MAccSpeed_Rec_P_AdjustID:  
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gMFlexAcc;      ConfirmData.ExitFlag=0;}  //暂存一下原始数据				
			SetTextInt32(current_screen_id,5,gMFlexAcc,0,0);     
		break;
		case MDecSpeed_Rec_P_AdjustID:   
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gMFlexDec;      ConfirmData.ExitFlag=0;} //暂存一下原始数据	
			SetTextInt32(current_screen_id,5,gMFlexDec,0,0);     
		break;
		
		case Current_Rec_P_SetID:
			SetTextInt32(current_screen_id,2,gFuncTorque,0,0); 			
		break;      //纠偏电流
		case Current_Rec_P_AdjustID:     
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gFuncTorque;     ConfirmData.ExitFlag=0;}  //暂存一下原始数据				
			SetTextInt32(current_screen_id,5,gFuncTorque,0,0);
		break;      //纠偏电流调节界面
		case Limit_Rec_P_SetID:  		 SetTextInt32(current_screen_id,5,gBehindLimit,0,1);        //更新到进度条和文本框
										 SetTextInt32(current_screen_id,6,gCenterLimit,0,1);        //更新到进度条和文本框
										 SetTextInt32(current_screen_id,9,gFrontLimit,0,1);       break;      //纠偏SPC
		case Behind_Rec_P_SetID: 
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gBehindLimit;     ConfirmData.ExitFlag=0;}  //暂存一下原始数据				
			SetTextInt32(current_screen_id,5,gBehindLimit,0,1);      
		break;
		case Center_Rec_P_SetID: 
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gCenterLimit;     ConfirmData.ExitFlag=0;}  //暂存一下原始数据				
			SetTextInt32(current_screen_id,5,gCenterLimit,0,1);      
		break;
		case Front_Rec_P_SetID:
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gFrontLimit;      ConfirmData.ExitFlag=0;} //暂存一下原始数据				
			SetTextInt32(current_screen_id,5,gFrontLimit,0,1);       
		break;
		
		case SPC_Rec_P_SetID:  			 SetTextFloat(current_screen_id,4,gSPCStopTime/10.0,1,1);        //更新到进度条和文本框
										 SetTextInt32(current_screen_id,5,gSPCBehindLimit,0,1);        //更新到进度条和文本框
										 SetTextInt32(current_screen_id,6,gSPCFrontLimit,0,1);       break;      //纠偏SPC
		case SPCStop_Rec_P_AdjustID:  
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gSPCStopTime;    ConfirmData.ExitFlag=0;}   //暂存一下原始数据					
			SetTextFloat(current_screen_id,5,gSPCStopTime/10.0,1,1);    
		break;
		case SPCBehind_Rec_P_AdjustID:  
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gSPCBehindLimit;    ConfirmData.ExitFlag=0;}   //暂存一下原始数据					
			SetTextInt32(current_screen_id,5,gSPCBehindLimit,0,1);      
		break;
		case SPCFront_Rec_P_AdjustID:  
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gSPCFrontLimit;      ConfirmData.ExitFlag=0;} //暂存一下原始数据					
			SetTextInt32(current_screen_id,5,gSPCFrontLimit,0,1);       
		break;
		
		case NO_Rec_P_SetID:  			 SetTextInt32(current_screen_id,12,gNoDetectValve,0,1);        //更新到进度条和文本框
										 SetTextFloat(current_screen_id,13,gNoDetectTime/10.0,1,1);        //更新到进度条和文本框
										 SetTextFloat(current_screen_id,14,gNoWaitTime/10.0,1,1);    break;      //纠偏无料
		case NOValue_Rec_P_AdjustID: 
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gNoDetectValve;     ConfirmData.ExitFlag=0;}  //暂存一下原始数据			
			SetTextInt32(current_screen_id,5,gNoDetectValve,0,1);       
		break;
		case NODetect_Rec_P_AdjustID:  
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gNoDetectTime;      ConfirmData.ExitFlag=0;} //暂存一下原始数据			
			SetTextFloat(current_screen_id,5,gNoDetectTime/10.0,1,1);   
		break;
		case NOWait_Rec_P_AdjustID: 
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gNoWaitTime;      ConfirmData.ExitFlag=0;} //暂存一下原始数据			
			SetTextFloat(current_screen_id,5,gNoWaitTime/10.0,1,1);     
		break;
		
		case PolarityID:
			AnimationPlayFrame(PolarityID,4,(*Polar.PPolarA)+gLanguageChoice*2); 
		break;
		/*快捷菜单    参数设置    传感器参数设置*/
		case Sensor_P_SetID:  			   SetTextInt32(current_screen_id,8,(s8)(gFineTune-80),1,1);  MatCal.BackInf = Sensor_P_SetID;  break;
		case Offset_Sensor_P_AdjustID:   
			if(ConfirmData.ExitFlag == 1)  {ConfirmData.TempData = gFineTune;      ConfirmData.ExitFlag=0;} //暂存一下原始数据	
			SetTextInt32(current_screen_id,5,(s8)(gFineTune-80),1,1);  
		break;
		/*快捷菜单    参数设置    推动器参数设置*/
		case Motor_P_SetID:  		TravelCal.BackInf = Motor_P_SetID;     break;
		/*快捷菜单    参数设置    系统参数设置*/
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
		/*快捷菜单    端口定义*/
		case Port_Sys_P_SetID:  		                              break;	
			
/********************密码键盘***********************/		
		case Password_ProtectionID:  			         break;	
			
/*****************************确认参数修改****************************/			
		case Confirm_ID:  		                              break;	
			
/*****************************传感器校准4步****************************/
		case Material_SwitchID:		 SetTextInt32(current_screen_id,2,*MatCal.CurMatNum,0,0);  		break;
		case MatValue_SwitchID:		 
			
		break;
		case Sensor_Calibr1ID:      	               break;
		case Ask_to_SwitchID:				break;
		case Sensor_Calibr2ID:     	 NoShelterDis();   	    break;
		case Sensor_Calibr3ID:       ShelterDis();          break;//进到传感器校准界面，移动材料，查看偏移量变化        实时读取传感器采样值		
		case Sensor_Calibr4ID:       RealTimeDis(); 	    break;
			
/*****************************行程校准****************************/
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
*  \brief  触摸坐标事件响应
*  \param press 1按下触摸屏，3松开触摸屏
*  \param x x坐标
*  \param y y坐标
*/
void NotifyTouchXY(u8 press,u16 x,u16 y)
{ 
    //TODO: 添加用户代码
}

/*! 
*  \brief  更新数据
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
		
		/**********显示警告***************/
		HallData  = (u8)((GPIOC->IDR&0x000001c0)>>6);        //读转子位置    合成GPIOC8|GPIOC7|GPIOC6 
		if(HallData>6||HallData<1)   	Warm[NotConFlag] = NotConFlag;
		else 						   	Warm[NotConFlag] = NoErrFlag; 
		
		PDout(2) = (Warm[LimitFlag]==LimitFlag?1:0);        //限位则打开继电器

		for(i=WarmNum-1;i>0;i--)
		{
			if(Warm[i] != 0)   {WarmFlag = Warm[i];  break;}
			else               {WarmFlag = 0;              }	
		}
		AnimationPlayFrame(current_screen_id,40,WarmFlag+gLanguageChoice*11);  

		/**********按下状态则按键按下计数每隔200ms加1***************/
		DelayInfaceGroup.DelayInface[DelayInfaceGroup.Num].DelayCnt = (DelayInfaceGroup.DelayInface[DelayInfaceGroup.Num].KeyDwon==1?++DelayInfaceGroup.DelayInface[DelayInfaceGroup.Num].DelayCnt:0);
		SetProgressValue(current_screen_id,51,DelayInfaceGroup.DelayInface[DelayInfaceGroup.Num].DelayCnt);  
		if(DelayInfaceGroup.DelayInface[DelayInfaceGroup.Num].DelayCnt == 10)  
		{
			SetScreen(DelayInfaceGroup.DelayInface[DelayInfaceGroup.Num].BackInface);      //按下3s，跳转到指定界面
		}
		
		/**********获取当前屏幕ID***************/
		GetScreen();
		
		if(Dis_Cnt%21==0)
		{
			if(WarmFlag>2&&gBuzzer==1)  SetBuzzer(20);       //限位报警
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
*  \brief  按钮控件通知
*  \details  当按钮状态改变(或调用GetControlValue)时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param state 按钮状态：0弹起，1按下
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
		/*自动菜单*/
		case Auto_ID:                //自动界面	
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
		case Polar_A_AdjustID:                //极性设置
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
			
		case Gain_A_AdjustID:              //增益调节
			anjiancl(screen_id, control_id, state,&ConfirmData,d_Gain_Data,Auto_ID);
		break;     
		case Offset_A_AdjustID:              //偏移量调节
			anjiancl(screen_id, control_id, state,&ConfirmData,d_Fine_Tuning,Auto_ID);
		break; 
		/*手动菜单*/
		case Manu_ID:             //手动界面
			switch(control_id)
			{
				case 5:
					if(state == 1) ClickButton=2;      //点动标志置为2		
					if(state == 0) ClickButton=0; 
				break;
				case 7:
					if(state == 1) ClickButton=1;      //点动标志置为1  
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
		/*中心菜单*/
		case Center_ID:             //中心界面
			switch(control_id)
			{
				case 5:
					if(state == 1) ClickButton=2;      //点动标志置为2		
					if(state == 0) ClickButton=0; 
				break;
				case 7:
					if(state == 1) ClickButton=1;      //点动标志置为1  
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
		
		/*快捷菜单*/
		case Shoutcut_ID:
			if(control_id == 2)   
			{
				PasswordSet.CallID = InputPasswordID;
				AnimationPlayFrame(Password_ProtectionID,17,PasswordSet.CallID+gLanguageChoice*3);
				AnimationPlayFrame(Password_ProtectionID,18,0); //显示修改密码中英文
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
					case 0: gWorkMode=0;  SetScreen(Manu_ID);     break;       //切换到手动界面
					case 1: gWorkMode=1;  SetScreen(Auto_ID);     break;       //切换到自动界面
					case 2: gWorkMode=2;  SetScreen(Center_ID);   break;       //切换到中心界面
					default :                break;
				}	
			}			
		break;
		/*快捷菜单    向导*/
		case Wizard_ID:
		break;
	
		case Manu_W_ID:         //手动操作  
			switch(control_id)
			{
				case 1:
					if(state == 1) ClickButton=2;      //点动标志置为2		
					if(state == 0) ClickButton=0; 
				break;
				case 2:
					if(state == 1) ClickButton=1;      //点动标志置为1  
					if(state == 0) ClickButton=0;                          
				break;
				default :         break;
			}
		break;
		case Sensor_W_SetID:       //传感器设置
		break;
		case Test_Manu_W_ID:         
			switch(control_id)
			{
				case 3:
					if(state == 1) ClickButton=2;      //点动标志置为2		
					if(state == 0) ClickButton=0; 
				break;
				case 4:
					if(state == 1) ClickButton=1;      //点动标志置为1  
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
		/*快捷菜单    参数设置*/
		case Parm_Set_ID:         
		
		break;
		/*快捷菜单    参数设置    纠偏参数设置*/
		case Rec_P_SetID:         
			if(control_id == 3)		   GainBackInf = 1;
		break;
		case Mode_Rec_P_SetID:          //纠偏模式 
			
		break;
		case Gain_Rec_P_SetID:          //纠偏增益
			if(control_id == 6)		   SetTextInt32(Gain_Rec_P_AdjustID,5,gGainData,0,1);           //更新调节界面的值
			else if(control_id == 7)   SetTextInt32(Dead_Rec_P_AdjustID,5,gDeadZone,0,1);
			else if(control_id == 9)   GainBackInf==0?SetScreen(Auto_ID):SetScreen(Rec_P_SetID);
		break;
		case Gain_Rec_P_AdjustID:  
			anjiancl(screen_id, control_id, state,&ConfirmData,d_Gain_Data,Gain_Rec_P_SetID);
		break;
		case Dead_Rec_P_AdjustID:  
			anjiancl(screen_id, control_id, state,&ConfirmData,d_Dead_Zone,Gain_Rec_P_SetID);			
		break;
		
		case Speed_Rec_P_SetID:          //速度限制  
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
		
		case AccDec_Rec_P_SetID:          //柔性加减速 
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
		
		/*快捷菜单    参数设置    传感器参数设置*/
		case Sensor_P_SetID:   		
			if(control_id == 13)	   SetTextInt32(Offset_Sensor_P_AdjustID,5,(s8)(gFineTune-80),1,1);			
		break;
		case Offset_Sensor_P_AdjustID: 
		    anjiancl(screen_id, control_id, state,&ConfirmData,d_Fine_Tuning,Sensor_P_SetID);	
		break;
		
		/*快捷菜单    参数设置    推动器参数设置*/
		case Motor_P_SetID:       
			
		break;
			
		/*快捷菜单    参数设置    系统参数设置*/	
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
				AnimationPlayFrame(Password_ProtectionID,17,PasswordSet.CallID+gLanguageChoice*3); //显示修改密码中英文
				AnimationPlayFrame(Password_ProtectionID,18,0); //显示修改密码中英文
			}
			else if(control_id == 6&&state==1) 
			{
				PasswordSet.CallID = RepeatInputID;
				AnimationPlayFrame(Password_ProtectionID,17,PasswordSet.CallID+gLanguageChoice*3);  //显示重复输入中英文
				AnimationPlayFrame(Password_ProtectionID,18,0); //显示修改密码中英文
			}
			else if(control_id == 7&&state==1&&PasswordSet.ChangeFlag == 2)              //确认更改密码
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
		
		/*快捷菜单    */
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
				gWorkMode=0;         //切换为手动模式，防止备份时候电机卡死，因为备份时候单片机外设停止工作
				AnimationPlayFrame(SYSInitDisID,2,ParaStorage.MenuCount+gLanguageChoice*3);
				delay_ms(1000);
				ParaStorage.fun[ParaStorage.MenuCount]();      //执行对应的函数：系统备份，系统还原，系统初始化
				SetScreen(RestoreBackupID);
				if(ParaStorage.MenuCount==SBackup || ParaStorage.MenuCount==SRestore)   gWorkMode = t_Work_Mode;
				
				InitDIS();   //系统还原和恢复出厂化设置都需要初始化显示
			}
		break;
			
/*****************************密码键盘********************************/				
		case Password_ProtectionID:
			if(control_id == 14)	
			{
				
			}
			else if(control_id == 16)  
			{
				PasswordSet.CallID == 0? SetScreen(Shoutcut_ID): SetScreen(Password_SetID);
			}
		break;	
			
/*****************************确认参数修改****************************/			
		case Confirm_ID:
			Confirm(control_id,&ConfirmData);
		break;
	
/*****************************传感器校准4步****************************/
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
				case 1: MatCal.EPCNum = EPC1;         break;   //选择校准传感器EPC1
				case 2: gWorkMode ==0?SetScreen(Sensor_Calibr2ID):SetScreen(Ask_to_SwitchID);        break;
				case 3: MatCal.EPCNum = EPC2;         break;   //选择校准传感器EPC2
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
			
/*****************************行程校准****************************/		
		case Travel_SetID:            //启动行程自适应，读取转矩
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
*  \brief  文本控件通知
*  \details  当文本通过键盘更新(或调用GetControlValue)时，执行此函数
*  \details  文本控件的内容以字符串形式下发到MCU，如果文本控件内容是浮点值，
*  \details  则需要在此函数中将下发字符串重新转回浮点值。
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param str 文本控件内容
*/
void NotifyText(u16 screen_id, u16 control_id, u8 *str)
{
	if(screen_id == Password_ProtectionID)	
	{
//		sscanf(str,"%ld",&value);      //把字符串转换为整数 
		PasswordSet.fun[PasswordSet.CallID](str);     //根据调用界面CallID判断执行哪个功能函数
	}
}                

/*!                                                                              
*  \brief  进度条控件通知 ,拖动进度条时候执行                                                      
*  \details  调用GetControlValue时，执行此函数                                  
*  \param screen_id 画面ID                                                      
*  \param control_id 控件ID                                                     
*  \param value 值                                                              
*/                                                                              
void NotifyProgress(u16 screen_id, u16 control_id, u32 value)           
{  
    if(screen_id == 3)
    {  
    }    
}                                                                                

/*!                                                                              
*  \brief  滑动条控件通知                                                       
*  \details  当滑动条改变(或调用GetControlValue)时，执行此函数                  
*  \param screen_id 画面ID                                                      
*  \param control_id 控件ID                                                     
*  \param value 值                                                              
*/                                                                              
void NotifySlider(u16 screen_id, u16 control_id, u32 value)             
{                                                             
	if(screen_id == 10)
    {  
	}
}

/*! 
*  \brief  仪表控件通知
*  \details  调用GetControlValue时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param value 值
*/
void NotifyMeter(u16 screen_id, u16 control_id, u32 value)
{
    //TODO: 添加用户代码
}

/*! 
*  \brief  菜单控件通知
*  \details  当菜单项按下或松开时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param item 菜单项索引
*  \param state 按钮状态：0松开，1按下
*/
void NotifyMenu(u16 screen_id, u16 control_id, u8 item, u8 state)
{
    //TODO: 添加用户代码
	if(screen_id == Travel_SetID&&control_id==4 && state==1)         //电机校准转矩选择
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
			case 0:    gNoWaitEN = 0;       break;     //使能
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
			case 1:    *Polar.PPolarM = Reverse;       break;    //使能
			default :            break;
		}
		AnimationPlayFrame(PolarityID,3,(*Polar.PPolarM)+gLanguageChoice*2); 
	}
	else if(screen_id == PolarityID && control_id==8 && state==1)
	{
		switch(item)
		{
			case 0:    *Polar.PPolarA = Forward;       break;
			case 1:    *Polar.PPolarA = Reverse;       break;    //使能
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
	else if(screen_id == Motor_P_SetID&&control_id==9 && state==1)        //推动器设置
	{
		switch(item)
		{
			case 0:   gMotorType = BrushlessMotor4;  break;
			case 1:   gMotorType = BrushlessMotor5;  break;
			default:                                      break;   
		}
		AnimationPlayFrame(Motor_P_SetID,6,gMotorType+gLanguageChoice*2); 
		switch(gMotorType)                                           //查询电机类型
		{
			case 0:                        //如果选择的无刷电机就开CAP,和外部中断作为霍尔检测    4对级
				NUMBERTURN = 7500000;
				SERIES     =   24;          
			break;                   
			case 1:                        //      5对级
				NUMBERTURN = 6000000;
				SERIES     =   30;
			break; 
			default: break;                   //系统初始化  
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
*  \brief  选择控件通知
*  \details  当选择控件变化时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param item 当前选项
*/
void NotifySelector(u16 screen_id, u16 control_id, u8  item)
{

}

/*! 
*  \brief  定时器超时通知处理
*  \param screen_id 画面ID
*  \param control_id 控件ID
*/
void NotifyTimer(u16 screen_id, u16 control_id)
{
    if(screen_id==8&&control_id == 7)
    {
        SetBuzzer(100);
    } 
}

/*! 
*  \brief  读取用户FLASH状态返回
*  \param status 0失败，1成功
*  \param _data 返回数据
*  \param length 数据长度
*/
void NotifyReadFlash(u8 status,u8 *_data,u16 length)
{
    //TODO: 添加用户代码
}

/*! 
*  \brief  写用户FLASH状态返回
*  \param status 0失败，1成功
*/
void NotifyWriteFlash(u8 status)
{
    //TODO: 添加用户代码
}

/*! 
*  \brief  读取RTC时间，注意返回的是BCD码
*  \param year 年（BCD）
*  \param month 月（BCD）
*  \param week 星期（BCD）
*  \param day 日（BCD）
*  \param hour 时（BCD）
*  \param minute 分（BCD）
*  \param second 秒（BCD）
*/
void NotifyReadRTC(u8 year,u8 month,u8 week,u8 day,u8 hour,u8 minute,u8 second)
{

}

void Adjustment_interface(u16 screen_id,u16 control_id,u16 *data,u8 StepData,u8 state)
{
	static u8 Key_time_number=0;
	static u8 key_sun_bit=0;
	
	if(state == 1||state == 2)      //按下状态，开始计时
	{
		switch(control_id)
		{
			case 2:  
				if(key_sun_bit==0)      //短按只执行一次
				{
					key_sun_bit=1;
					(*data)+=StepData;
				}
				else
				{
					Key_time_number<7?++Key_time_number:(*data+=StepData);    //长按700ms，则快速加减
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


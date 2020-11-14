#include <string.h>
#include "delay.h"
#include "lcdtest.h"
#include "main.h"
#include "stmflash.h"
#include "modbus.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

extern  s16 OcclusionL_rate,OcclusionR_rate;             //传感器遮挡率


ParaStorageDef ParaStorage={SRestore,{Backup,Restore,ParameterInit}};
SensorModeDef SensorMode = {&gSensorMode,SensorEPC1Dis,SensorEPC2Dis,SensorCPCDis,SensorSPCDis};
TravelCalDef TravelCal = {0,0,0};						


MatCalDef MatCal = {            //材料校准 
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
	0,0,0,0,0,NoShelterDis,ShelterDis,RealTimeDis
};

ConfirmDataDef ConfirmData = {
	1,0,
	{
		{0,160,&gFineTune,1},            //偏移量
		{1,200,&gGainData,1},            //增益
		{0,80,&gDeadZone,1},             //盲区
		{5,95,&gCenterLimit,1},          //中心限位
		{1,200,&gGainData,1},            //增益
		{1,200,&gGainData,1},            //增益
		{0,80,&gDeadZone,1},             //盲区
		{100,3000,&gManuSpeed,100},    	 //手动速度
		{100,3000,&gAutoSpeed,100},      //自动速度
		{100,3000,&gCentSpeed,100},   	 //中心速度
		
		{20,100,&gAFlexAcc,1},			 //自动加速
		{20,100,&gAFlexDec,1},			 //自动减速
		{20,100,&gMFlexAcc,1},			 //手动加速
		{20,100,&gMFlexDec,1},			 //手动减速
		
		{2,10,&gFuncTorque,1},			 //运行电流
		
		{5,95,&gBehindLimit,1},          //后限位
		{5,95,&gCenterLimit,1},          //中心限位
		{5,95,&gFrontLimit,1},			 //前限位
		
		{2,250,&gSPCStopTime,1},		 //SPC停止时间
		{10,90,&gSPCBehindLimit,1},		 //SPC后限位
		{10,90,&gSPCFrontLimit,1},		 //SPC前限位
		
		{5,95,&gNoDetectValve,1},		 //无料阈值
		{1,100,&gNoDetectTime,1},		 //无料检测时间
		{1,100,&gNoWaitTime,1},			 //无料等待时间

		{0,160,&gFineTune,1},            //偏移量
	},
};


		/**********警告标志计算***************/
		
u8 WarmOut()
{
	u8 icnt,HallData;
	HallData  = (u8)((GPIOC->IDR&0x000001c0)>>6);        //读转子位置    合成GPIOC8|GPIOC7|GPIOC6 
	
	if(HallData>6||HallData<1)   	Warm[NotConFlag] = NotConFlag;
	else 						   	Warm[NotConFlag] = NoErrFlag; 
	
	if(*pHWarmFlag == 1)     		Warm[LockFlag] = LockFlag;
	else if(*pHWarmFlag == 3)  		Warm[ProhibitFlag] = ProhibitFlag;
	else 					   	   {Warm[LockFlag] = NoErrFlag;     Warm[ProhibitFlag] = NoErrFlag;}
	
	/**************打开继电器**********************/
	if(*pHAlarmSwitch == 0)    		PDout(2) = (Warm[LimitFlag]==LimitFlag?1:0);  
	else if(*pHAlarmSwitch == 1)    PDout(2) = (Warm[BreakAlarmFlag]==BreakAlarmFlag?1:0);  
	
	for(icnt=WarmNum-1;icnt>0;icnt--)
	{
		if(Warm[icnt] != 0)   return Warm[icnt];           
	}
	return 0;  
}	  


/************4种传感器模式对应的显示程序***************/
void SensorEPC1Dis()    //EPC1                
{
	if(OcclusionL_rate>=0)   //0~100
	{
		*pSensorRate = OcclusionL_rate&0x00ff;             //低8位显示正   
	}
	else                    //-100~0
	{
		*pSensorRate = ((-OcclusionL_rate)<<8)&0xff00;       //高8位显示负，十进制移位也是乘2  
	}
}

void SensorEPC2Dis()    //EPC2			
{
	if(OcclusionR_rate>=0)
	{
		*pSensorRate = OcclusionR_rate&0x00ff;             //低8位显示正  
	}
	else
	{
		*pSensorRate = ((-OcclusionR_rate)<<8)&0xff00;       //高8位显示负，十进制移位也是乘2    
	}
}

void SensorCPCDis()   //CPC      
{
	s16 DisRate;
	DisRate = (OcclusionR_rate-OcclusionL_rate)>>1;

	if(OcclusionR_rate-OcclusionL_rate>=0)
	{
		*pSensorRate = DisRate&0x00ff;             //低8位显示正  
	}
	else
	{
		*pSensorRate = ((-DisRate)<<8)&0xff00;       //高8位显示负，十进制移位也是乘2 
	}
}

void SensorSPCDis()   //SPC      
{	

	if(gSPCMode == 0)    //0：内部编码器  1：外部传感器
	{
	
	}
	
	*pSensorRate = (((OcclusionL_rate>>1)+50)<<8)|((OcclusionR_rate>>1)+50);             //高8位显示负 左传感器 低8位显示正 右传感器 
}

/**************传感器校准界面显示程序*******************/
u8 NoShelterDis()
{
	switch(*pHSensorNum)
	{
		case EPC1:    
			MatCal.t_Sensor1_valueH = SensorL_value;
			*pSensorValue	=	(SensorL_value|(*pHMatNum<<12))&0x7fff;  //最高位为0，表示EPC1
		break;          
		case EPC2:    
			MatCal.t_Sensor2_valueH = SensorR_value;
			*pSensorValue	=	(SensorR_value|(*pHMatNum<<12))|0x8000;  //最高位为1，表示EPC2
		break;          
		default :                                break;
	}
	return 0 ;
}

u8 ShelterDis()
{
	switch(*pHSensorNum)
	{
		case EPC1:
			MatCal.t_Sensor1_valueL = SensorL_value;
			*pSensorValue	=	(SensorL_value|(*pHMatNum<<12))&0x7fff;  //最高位为0，表示EPC1
			MatCal.CaliFlag = 1;  
		break;          
		case EPC2:    
			MatCal.t_Sensor2_valueL = SensorR_value;
			*pSensorValue	=	(SensorR_value|(*pHMatNum<<12))|0x8000;  //最高位为1，表示EPC2
			MatCal.CaliFlag = 1;     //表示校准完成
		break;          
		default :                                break;
	}
	return 0 ;
}

u8 RealTimeDis()
{
	switch(*pHSensorNum)
	{
		case EPC1: 
			*pSensorValue	=	(SensorL_value|(*pHMatNum<<12))&0x7fff;  //最高位为0，表示EPC1
			SensorEPC1Dis();
			if((MatCal.t_Sensor1_valueH-MatCal.t_Sensor1_valueL > 125))   
			{
				if(MatCal.CaliFlag == 1)   
				{
					*MatCal.MatValue[*pHMatNum].PSensor1_H = MatCal.t_Sensor1_valueH;
					*MatCal.MatValue[*pHMatNum].PSensor1_L = MatCal.t_Sensor1_valueL;
					*MatCal.MatValue[*pHMatNum].PEPC12 = *MatCal.MatValue[*pHMatNum].PEPC12|(1<<1);
					WriteSensor();   
					MatCal.CaliFlag = 0;        //校准完成标志
					return 0 ;
				}
			}else if((MatCal.t_Sensor1_valueH-MatCal.t_Sensor1_valueL < -125)){ 
				if(MatCal.CaliFlag == 1)   
				{
					*MatCal.MatValue[*pHMatNum].PSensor1_H = MatCal.t_Sensor1_valueL;
					*MatCal.MatValue[*pHMatNum].PSensor1_L = MatCal.t_Sensor1_valueH;	
					*MatCal.MatValue[*pHMatNum].PEPC12 = *MatCal.MatValue[*pHMatNum].PEPC12|(1<<1);
					WriteSensor(); 	
					MatCal.CaliFlag = 0;        //校准完成标志
					return 0 ;     		//用于显示校准成功
				}
			}else{
				return 1;         			//用于显示校准失败
			}
		break;          //保存传感器1（左边）最小值
		case EPC2: 
			*pSensorValue	=	(SensorR_value|(*pHMatNum<<12))|0x8000;  //最高位为1，表示EPC2
			SensorEPC2Dis();
			if((MatCal.t_Sensor2_valueH-MatCal.t_Sensor2_valueL>125))   
			{
				if(MatCal.CaliFlag == 1)   
				{
					*MatCal.MatValue[*pHMatNum].PSensor2_H = MatCal.t_Sensor2_valueH;
					*MatCal.MatValue[*pHMatNum].PSensor2_L = MatCal.t_Sensor2_valueL;
					*MatCal.MatValue[*pHMatNum].PEPC12 = *MatCal.MatValue[*pHMatNum].PEPC12|1;
					WriteSensor(); 
					MatCal.CaliFlag = 0;        //校准完成标志
					return 0 ;
				}
			}else if((MatCal.t_Sensor2_valueH-MatCal.t_Sensor2_valueL<-125)) {
				if(MatCal.CaliFlag == 1)   
				{
					*MatCal.MatValue[*pHMatNum].PSensor2_H = MatCal.t_Sensor2_valueL;
					*MatCal.MatValue[*pHMatNum].PSensor2_L = MatCal.t_Sensor2_valueH;
					*MatCal.MatValue[*pHMatNum].PEPC12 = *MatCal.MatValue[*pHMatNum].PEPC12|1;
					WriteSensor(); 	
					MatCal.CaliFlag = 0;        //校准完成标志
					return 0 ;
				}
			}else{
				return 1 ;
			}			    
		break;          //保存传感器2（右边）最小值
		default :         return 1 ;             
	}
}

/*****************备份还原********************/
void BackupRestore(void)        //每隔50ms调用一次
{
	if(*pHSysYes == 0||*pHSysYes == 1||*pHSysYes == 2)   		//系统备份
	{
		gWorkMode = 0;
		delay_ms(1000);
		
		ParaStorage.fun[*pHSysYes]();	
		*pHSysYes = 255;
	}
}

/*****************参数调节********************/
void Adjustment_interface(u16 control_id,u16 *data,u8 StepData,u8 state)
{
	static u8 Key_time_number=0;
	static u8 key_sun_bit=0;
	
	if(state == 1)      //按下状态，开始计时
	{
		switch(control_id)
		{
			case 1:  
				if(key_sun_bit==0)      //短按只执行一次
				{
					key_sun_bit=1;
					(*data) += StepData;
				}else{
					Key_time_number<7?++Key_time_number:(*data+=StepData);    //长按700ms，则快速加减
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

void AddSub()  //结构体指针指向传递过来的结构体数组成员
{
	static u8 TimeCnt;
	u8 state;
	ConfirmDataDef *PCData;
	
	state = *pHAddSub==0?0:1;   //*pHAddSub==0表示加减按钮没有按下
	PCData = &ConfirmData;
	
	if(PCData->ExitFlag == 1) //防止上电时暂存，将*pHAdjustFlag初始化为255
	{
		PCData->TempData = *PCData->AdjustData[*pHAdjustFlag].tData;    
		PCData->ExitFlag=0;
	}   //暂存一下原始数据	
	
	if(TimeCnt>10)
	{
		if((*pHAddSub==1&&(*PCData->AdjustData[*pHAdjustFlag].tData+PCData->AdjustData[*pHAdjustFlag].StepData <= PCData->AdjustData[*pHAdjustFlag].UpperLimit) )||state==0)
		{
			Adjustment_interface(*pHAddSub,PCData->AdjustData[*pHAdjustFlag].tData,PCData->AdjustData[*pHAdjustFlag].StepData,state);
		}else if((*pHAddSub==2&&(*PCData->AdjustData[*pHAdjustFlag].tData-PCData->AdjustData[*pHAdjustFlag].StepData >= PCData->AdjustData[*pHAdjustFlag].LowerLimit) )||state==0){
			Adjustment_interface(*pHAddSub,PCData->AdjustData[*pHAdjustFlag].tData,PCData->AdjustData[*pHAdjustFlag].StepData,state);
		}
		TimeCnt=0;
	}	
	
	if(*pHAdjustFlag==0||*pHAdjustFlag==24)
	{
		*pAdjustData = (*PCData->AdjustData[*pHAdjustFlag].tData)*10;
	}else if(*pHAdjustFlag==18||*pHAdjustFlag==22||*pHAdjustFlag==23){
		*pAdjustData = *PCData->AdjustData[*pHAdjustFlag].tData+800;
	}else{
		*pAdjustData = (*PCData->AdjustData[*pHAdjustFlag].tData)*10+800;
	}
	
	if(*pHParaConfirm==2)   //取消按下
	{
		*ConfirmData.AdjustData[*pHAdjustFlag].tData = ConfirmData.TempData;     //还原原始数据
		ConfirmData.ExitFlag = 1;
		*pHAdjustFlag = 255;
	}else if(*pHParaConfirm==1){
		ConfirmData.ExitFlag = 1;
		*pHAdjustFlag = 255;
	}
	TimeCnt++;
}



